#include <windows.h>
#include <dbghelp.h>
#include <cstdio>
#include <string>
#include <cmath>
#pragma comment(lib, "dbghelp.lib")


bool GetFunctionFromFileAndLine(
    HMODULE module,
    const char *objfilepath, // optional. must be fullpath
    const char *cppfilepath, // optional. must be fullpath
    int line,
    char *out_funcname, // optional
    int bufsize_funcname,
    void **out_funcaddr // optional
);


void testfunc()
{
    printf("");
}


int main(int argc, char *argv[])
{
    ::SymSetOptions(SYMOPT_DEFERRED_LOADS | SYMOPT_DEBUG);
    ::SymInitialize(::GetCurrentProcess(), NULL, TRUE);

    HMODULE mail_module = ::GetModuleHandleA(nullptr);
    char obj_fullpath[MAX_PATH+1];
    char cpp_fullpath[MAX_PATH+1];
    ::GetFullPathNameA("GetFunctionFromFileAndLine.obj", sizeof(obj_fullpath), obj_fullpath, nullptr);
    ::GetFullPathNameA("GetFunctionFromFileAndLine.cpp", sizeof(cpp_fullpath), cpp_fullpath, nullptr);

    char funcname[MAX_SYM_NAME+1];
    void *funcaddr = nullptr;
    GetFunctionFromFileAndLine(mail_module, obj_fullpath, cpp_fullpath, 22, funcname, sizeof(funcname), &funcaddr);

    printf("%s [0x%p]\n", funcname, funcaddr);
}

/*
$ cl GetFunctionFromFileAndLine.cpp /Zi /EHsc
$ ./GetFunctionFromFileAndLine
testfunc [0x00FD13D3]
*/




bool AddressToSymbolName(void *address, char *o_funcname, size_t buflen)
{
    char buf[sizeof(SYMBOL_INFO)+MAX_SYM_NAME+1];
    PSYMBOL_INFO sinfo = (PSYMBOL_INFO)buf;
    sinfo->SizeOfStruct = sizeof(SYMBOL_INFO);
    sinfo->MaxNameLen = MAX_SYM_NAME+1;
    if(::SymFromAddr(::GetCurrentProcess(), (DWORD64)address, 0, sinfo)==FALSE) {
        return false;
    }
    strncpy(o_funcname, sinfo->Name, buflen);
    return true;
}

struct LineCallbackContext
{
    int line;
    int line_closest;
    void *addr_closest;
};
BOOL CALLBACK LineCallback(PSRCCODEINFO LineInfo, PVOID UserContext)
{
    LineCallbackContext &ctx = *(LineCallbackContext*)UserContext;
    if(std::abs((int)LineInfo->LineNumber-ctx.line) < std::abs((int)LineInfo->LineNumber-ctx.line_closest)) {
        ctx.line_closest = (int)LineInfo->LineNumber;
        ctx.addr_closest = (void*)LineInfo->Address;
    }
    return TRUE;
}

bool GetFunctionFromFileAndLine(
    HMODULE module,
    const char *objfilepath,
    const char *cppfilepath,
    int line,
    char *o_funcname,
    int bufsize_funcname,
    void **o_funcaddr)
{
    LineCallbackContext ctx = {line, INT_MAX, nullptr};
    ::SymEnumLines(::GetCurrentProcess(), (ULONG64)module, objfilepath, cppfilepath, &LineCallback, &ctx);

    if(ctx.addr_closest!=nullptr) {
        if(o_funcname!=nullptr) {
            AddressToSymbolName(ctx.addr_closest, o_funcname, bufsize_funcname);
        }
        if(o_funcaddr!=nullptr) {
            *o_funcaddr = ctx.addr_closest;
        }
        return true;
    }
    return false;
}
