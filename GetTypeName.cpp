#define _CRT_SECURE_NO_WARNINGS
#include <functional>
#include <string>
#include <cstdint>

#define _NO_CVCONST_H
#include <windows.h>
#include <intrin.h>
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")


struct VariableInfo
{
    std::string type_name;
    std::string value_name;
    void *data;
    void *stackframe;
};
void EachVariables(size_t stack_up, const std::function<void (const VariableInfo&)> &f);
std::string GetTypeNameImpl(const char *value_name, void *value_ptr); // value_ptr forces variable on memory

#define GetTypeName(V) GetTypeNameImpl(#V, &V)


class Test {};

int main(int argc, char *argv[])
{
    ::SymSetOptions(SYMOPT_DEFERRED_LOADS | SYMOPT_DEBUG);
    ::SymInitialize(::GetCurrentProcess(), NULL, TRUE);

    int32_t i;
    std::string str;
    Test test;

    printf("argv: %s\n", GetTypeName(argv).c_str());
    printf("i: %s\n", GetTypeName(i).c_str());
    printf("str: %s\n", GetTypeName(str).c_str());
    printf("test: %s\n", GetTypeName(test).c_str());
}

/*
$ cl /Zi /EHsc GetTypeName.cpp

argv: **char
i: int32_t
str: std::basic_string<char,std::char_traits<char>,std::allocator<char> >
test: Test
*/




enum BasicType
{
    btNoType = 0,
    btVoid = 1,
    btChar = 2,
    btWChar = 3,
    btInt = 6,
    btUInt = 7,
    btFloat = 8,
    btBCD = 9,
    btBool = 10,
    btLong = 13,
    btULong = 14,
    btCurrency = 25,
    btDate = 26,
    btVariant = 27,
    btComplex = 28,
    btBit = 29,
    btBSTR = 30,
    btHresult = 31
};
static void BasetypeToString(DWORD t, ULONG64 size, std::string &ret)
{
    switch(t) {
    case btVoid:  ret += "void"; break;
    case btChar:  ret += "char"; break;
    case btWChar: ret += "wchar_t"; break;
    case btInt:
    case btLong:
        switch(size) {
        case 1: ret+= "int8_t"; break;
        case 2: ret+="int16_t"; break;
        case 4: ret+="int32_t"; break;
        case 8: ret+="int64_t"; break;
        }
        break;
    case btUInt:
    case btULong:
        switch(size) {
        case 1: ret+= "uint8_t"; break;
        case 2: ret+="uint16_t"; break;
        case 4: ret+="uint32_t"; break;
        case 8: ret+="uint64_t"; break;
        }
        break;
    case btFloat:
        switch(size) {
        case 4: ret+="float"; break;
        case 8: ret+="double"; break;
        }
        break;
    }
}


static bool FindClassNameFromVFTable(DWORD t, void *obj, char *out, size_t len)
{
    char buf[sizeof(SYMBOL_INFO)+MAX_SYM_NAME];
    PSYMBOL_INFO sinfo = (PSYMBOL_INFO)buf;
    sinfo->SizeOfStruct = sizeof(SYMBOL_INFO);
    sinfo->MaxNameLen = MAX_SYM_NAME;

    // vftable のシンボル名が "class名::`vftable'" になっているので、そこから class 名を取得
    if(::SymFromAddr(::GetCurrentProcess(), (DWORD64)((void***)obj)[0], nullptr, sinfo)) {
        char vftable[MAX_SYM_NAME];
        ::UnDecorateSymbolName(sinfo->Name, vftable, MAX_SYM_NAME, UNDNAME_NAME_ONLY);
        if(char *colon=strstr(vftable, "::`vftable'")) {
            *colon = '\0';
            strncpy(out, vftable, len);
            return true;
        }
    }
    return false;
}

__declspec(noinline) void TypeToString(SYMBOL_INFO *si, DWORD t, std::string &ret)
{
    HANDLE proc = ::GetCurrentProcess();
    ULONG64 mod = si->ModBase;

    DWORD tag = 0;
    DWORD basetype = 0;
    ::SymGetTypeInfo(proc, mod, t, TI_GET_SYMTAG, &tag );

    if(tag==SymTagArrayType) {
        DWORD count = 0;
        ::SymGetTypeInfo(proc, mod, t, TI_GET_COUNT, &count);
        char a[128];
        sprintf(a, "[%d]", count);
        ret += a;

        DWORD tid = 0;
        ::SymGetTypeInfo(proc, mod, t, TI_GET_TYPEID, &tid);
        return TypeToString(si, tid, ret);
    }
    else if(tag==SymTagPointerType) {
        ret = "*"+ret;

        DWORD tid = 0;
        ::SymGetTypeInfo(proc, mod, t, TI_GET_TYPEID, &tid);
        return TypeToString(si, tid, ret);
    }
    else if(tag==SymTagBaseType) {
        if( ::SymGetTypeInfo(proc, mod, t, TI_GET_BASETYPE, &basetype) ) {
            ULONG64 len;
            ::SymGetTypeInfo(proc, mod, t, TI_GET_LENGTH, &len);
            BasetypeToString(basetype, len, ret);
        }
    }
    else if(tag==SymTagUDT) {
        WCHAR *wname = nullptr;
        ::SymGetTypeInfo(proc, mod, t, TI_GET_SYMNAME, &wname );
        char name[MAX_SYM_NAME];
        size_t num = 0;
        ::wcstombs_s(&num, name, wname, _countof(name));
        ::LocalFree(wname);
        ret += name;
    }
}

struct Ctx_EachVariable
{
    const std::function<void (const VariableInfo&)> *fp;
    VariableInfo vinfo;
};

static BOOL CALLBACK CB_EachVariable( SYMBOL_INFO* si, ULONG size, PVOID p )
{
    if(si) {
        Ctx_EachVariable &ctx = *(Ctx_EachVariable*)p;
        VariableInfo &vi = ctx.vinfo;
        vi.value_name = si->Name;
        vi.data = (void*)((size_t)vi.stackframe + (size_t)si->Address);
        TypeToString(si, si->TypeIndex, vi.type_name);
        (*ctx.fp)(vi);
        vi.type_name.clear();
    }
    return TRUE;
}

__declspec(noinline) void EachVariables(size_t stack_up, const std::function<void (const VariableInfo&)> &f)
{
    // thanks to http://jpassing.com/2008/03/12/walking-the-stack-of-the-current-thread/
    CONTEXT context;
#ifdef _WIN64
    ::RtlCaptureContext(&context);
#else
    ::ZeroMemory( &context, sizeof(context) );
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
    ::ZeroMemory( &stackFrame, sizeof(stackFrame) );
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
    for(size_t i=0; i<stack_up+2; ++i) {
        ::StackWalk64(machineType, hProcess, hThread, &stackFrame, &context, NULL, NULL, NULL, NULL);
    }

    Ctx_EachVariable ctx = {&f};
#ifdef _WIN64
    ctx.vinfo.stackframe = *(void**)(stackFrame.AddrStack.Offset);
#else
    ctx.vinfo.stackframe = *(void**)(stackFrame.AddrFrame.Offset);
#endif

    IMAGEHLP_STACK_FRAME sf; 
    sf.ReturnOffset = stackFrame.AddrReturn.Offset;
    sf.FrameOffset = stackFrame.AddrFrame.Offset;
    sf.StackOffset = stackFrame.AddrStack.Offset;
    sf.InstructionOffset = stackFrame.AddrPC.Offset;
    ::SymSetContext(hProcess, &sf, 0 );
    ::SymEnumSymbols(hProcess, 0, 0, CB_EachVariable, &ctx);
}


__declspec(noinline) std::string GetTypeNameImpl(const char *value_name, void *value)
{
    std::string ret;
    EachVariables(1, [&](const VariableInfo &vi){
        if(vi.value_name==value_name) {
            ret = vi.type_name;
        }
    });
    return ret;
}
