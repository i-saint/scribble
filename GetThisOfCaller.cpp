#include <windows.h>
#include <dbghelp.h>
#include <cstdio>
#include <string>
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
        // 適当にスタック散らかす
        int a=1, b=2, c=3;
        std::string make_it_messy("foo");
        Test();
        int d=4, e=5, f=6;
    }

    void test3()
    {
        int a = 0;
        std::string make_it_messy("foo");
        for(int i=0; i<3; ++i) {
            a += i;
            std::string make_it_more_messy("bar");
            Test();
        }
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
    Test();
}
/*
Hoge: this is 0x003AFD57
this of caller: 0x003AFD57
this of caller: 0x003AFD57
this of caller: 0x003AFD57
this of caller: 0x003AFD57
this of caller: 0x003AFD57
this of caller: 0x00000000
*/




BOOL CALLBACK EnumSymbolsCallback( SYMBOL_INFO* si, ULONG size, PVOID p )
{
    if(si && si->NameLen==4 && strncmp(si->Name, "this", 4)==0) {
        auto *ret = (std::pair<ULONG64,bool>*)p;
        ret->first = si->Address;
        ret->second = true;
        return FALSE;
    }
    return TRUE;
}

void* GetThisOfCaller()
{
    CONTEXT context;
#ifdef _WIN64
    ::RtlCaptureContext(&context);
#else
    ::ZeroMemory( &context, sizeof( CONTEXT ) );
    context.ContextFlags = CONTEXT_CONTROL;
    __asm
    {
        EIP:
        mov [context.Ebp], ebp;
        mov [context.Esp], esp;
        mov eax, [EIP];
        mov [context.Eip], eax;
    }
#endif 

    STACKFRAME64 stackFrame;
    memset(&stackFrame, 0, sizeof(stackFrame));
#ifdef _WIN64
    DWORD machineType = IMAGE_FILE_MACHINE_AMD64;
    stackFrame.AddrPC.Offset = context.Rip;
    stackFrame.AddrPC.Mode = AddrModeFlat;
    stackFrame.AddrFrame.Offset = context.Rbp;
    stackFrame.AddrFrame.Mode = AddrModeFlat;
    stackFrame.AddrStack.Offset = context.Rsp;
    stackFrame.AddrStack.Mode = AddrModeFlat;
#else
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
    ::StackWalk64(machineType, hProcess, hThread, &stackFrame, &context, NULL, NULL, NULL, NULL); // この関数のスタックフレーム
    ::StackWalk64(machineType, hProcess, hThread, &stackFrame, &context, NULL, NULL, NULL, NULL); // 呼び出し元
    ::StackWalk64(machineType, hProcess, hThread, &stackFrame, &context, NULL, NULL, NULL, NULL); // 呼び出し元の呼び出し元 (ターゲット)

    std::pair<ULONG64,bool> ret(0,false);
    IMAGEHLP_STACK_FRAME sf; 
    sf.ReturnOffset = stackFrame.AddrReturn.Offset;
    sf.FrameOffset = stackFrame.AddrFrame.Offset;
    sf.StackOffset = stackFrame.AddrStack.Offset;
    sf.InstructionOffset = stackFrame.AddrPC.Offset;
    ::SymSetContext(hProcess, &sf, 0 );
    ::SymEnumSymbols(hProcess, 0, 0, EnumSymbolsCallback, &ret);

    if(!ret.second) { return NULL; }
#ifdef _WIN64
    return *(void**)(stackFrame.AddrStack.Offset + ret.first);
#else
    return *(void**)(stackFrame.AddrFrame.Offset + ret.first);
#endif
}
