#include <windows.h>
#include <dbghelp.h>
#include <cstdio>
#pragma comment(lib, "dbghelp.lib")

void* GetThisOfCaller();

void Test()
{
    printf("this of caller: 0x%p\n", GetThisOfCaller());
}

class Hoge
{
public:
    Hoge()
    {
        printf("Hoge: this is 0x%p\n", this);
    }

    void test1()
    {
        Test();
    }

    void test2()
    {
        int a,b,c; // 適当にスタック散らかす
        Test();
        int d,e,f;
    }

    void test3()
    {
        test2();
        test1();
    }
};

void main()
{
    ::SymInitialize(::GetCurrentProcess(), NULL, TRUE);
    ::SymSetOptions(SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES);

    Hoge hoge;
    hoge.test1();
    hoge.test2();
    hoge.test3();
}
/*
Hoge: this is 0x0044FC97
this of caller: 0x0044FC97
this of caller: 0x0044FC97
this of caller: 0x0044FC97
this of caller: 0x0044FC97
*/




BOOL CALLBACK EnumSymbolsCallback( SYMBOL_INFO* si, ULONG SymbolSize, PVOID UserContext ) 
{
    if(si && si->NameLen==4 && strncmp(si->Name, "this", 4)==0) {
        *(ULONG64*)UserContext = si->Address + si->Size;
        return FALSE;
    }
    return TRUE;
}

void* GetThisOfCaller()
{
    CONTEXT context;
    ZeroMemory( &context, sizeof( CONTEXT ) );
    context.ContextFlags = CONTEXT_CONTROL;
    __asm
    {
EIP:
        mov [context.Ebp], ebp;
        mov [context.Esp], esp;
        mov eax, [EIP];
        mov [context.Eip], eax;
    }

    STACKFRAME64 stackFrame;
    memset(&stackFrame, 0, sizeof(stackFrame));

#if defined(_WIN64)
    DWORD machineType = IMAGE_FILE_MACHINE_AMD64;
    stackFrame.AddrPC.Offset = context.Rip;
    stackFrame.AddrPC.Mode = AddrModeFlat;
    stackFrame.AddrFrame.Offset = context.Rbp;
    stackFrame.AddrFrame.Mode = AddrModeFlat;
    stackFrame.AddrStack.Offset = context.Rsp;
    stackFrame.AddrStack.Mode = AddrModeFlat;
#elif defined(_WIN32)
    DWORD machineType = IMAGE_FILE_MACHINE_I386;
    stackFrame.AddrPC.Offset = context.Eip;
    stackFrame.AddrPC.Mode = AddrModeFlat;
    stackFrame.AddrFrame.Offset = context.Ebp;
    stackFrame.AddrFrame.Mode = AddrModeFlat;
    stackFrame.AddrStack.Offset = context.Esp;
    stackFrame.AddrStack.Mode = AddrModeFlat;
#endif

    HANDLE hProcess = ::GetCurrentProcess();
    HANDLE hThread = ::GetCurrentThread();
    StackWalk64(machineType, hProcess, hThread, &stackFrame, &context, NULL, NULL, NULL, NULL); // この関数のスタックフレーム
    StackWalk64(machineType, hProcess, hThread, &stackFrame, &context, NULL, NULL, NULL, NULL); // 呼び出し元
    StackWalk64(machineType, hProcess, hThread, &stackFrame, &context, NULL, NULL, NULL, NULL); // 呼び出し元の呼び出し元 (ターゲット)

    ULONG64 offset = NULL;
    IMAGEHLP_STACK_FRAME sf; 
    sf.ReturnOffset = stackFrame.AddrReturn.Offset;
    sf.FrameOffset = stackFrame.AddrFrame.Offset;
    sf.StackOffset = stackFrame.AddrStack.Offset;
    sf.InstructionOffset = stackFrame.AddrPC.Offset;
    ::SymSetContext(hProcess, &sf, 0 );
    ::SymEnumSymbols(hProcess, 0, 0, EnumSymbolsCallback, &offset); 
    return *(void**)(stackFrame.AddrStack.Offset - offset);
}
