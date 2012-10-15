#include <windows.h>
#include <dbghelp.h>
#include <string>

#pragma comment(lib, "dbghelp.lib")


std::string AddressToSymbolName(void *address)
{
#ifdef _WIN64
    typedef DWORD64 DWORDX;
    typedef PDWORD64 PDWORDX;
#else
    typedef DWORD DWORDX;
    typedef PDWORD PDWORDX;
#endif

    char buf[1024];
    HANDLE process = ::GetCurrentProcess();
    IMAGEHLP_MODULE imageModule = { sizeof(IMAGEHLP_MODULE) };
    IMAGEHLP_LINE line ={sizeof(IMAGEHLP_LINE)};
    DWORDX dispSym = 0;
    DWORD dispLine = 0;

    char symbolBuffer[sizeof(IMAGEHLP_SYMBOL) + MAX_PATH] = {0};
    IMAGEHLP_SYMBOL * imageSymbol = (IMAGEHLP_SYMBOL*)symbolBuffer;
    imageSymbol->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL);
    imageSymbol->MaxNameLength = MAX_PATH;

    if(!::SymGetModuleInfo(process, (DWORDX)address, &imageModule)) {
        sprintf_s(buf, "[0x%p]\n", address);
    }
    else if(!::SymGetSymFromAddr(process, (DWORDX)address, &dispSym, imageSymbol)) {
        sprintf_s(buf, "%s + 0x%x [0x%p]\n", imageModule.ModuleName, ((size_t)address-(size_t)imageModule.BaseOfImage), address);
    }
    else if(!::SymGetLineFromAddr(process, (DWORDX)address, &dispLine, &line)) {
        sprintf_s(buf, "%s!%s + 0x%x [0x%p]\n", imageModule.ModuleName, imageSymbol->Name, ((size_t)address-(size_t)imageSymbol->Address), address);
    }
    else {
        sprintf_s(buf, "%s(%d): %s!%s + 0x%x [0x%p]\n", line.FileName, line.LineNumber,
            imageModule.ModuleName, imageSymbol->Name, ((size_t)address-(size_t)imageSymbol->Address), address);
    }
    return buf;
}


std::string ExceptionToSymbolNames(EXCEPTION_POINTERS *ep)
{
    EXCEPTION_RECORD &er = *ep->ExceptionRecord;
    CONTEXT &context = *ep->ContextRecord;
    HANDLE hProcess = GetCurrentProcess();
    HANDLE hThread = GetCurrentThread();

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

    std::string ret;
    ret += AddressToSymbolName(er.ExceptionAddress);
    for(;;)
    {
        if( StackWalk64(machineType, hProcess, hThread, &stackFrame, &context, NULL, NULL, NULL, NULL)==FALSE ||
            stackFrame.AddrPC.Offset==stackFrame.AddrReturn.Offset ||
            stackFrame.AddrPC.Offset==0 ||
            stackFrame.AddrReturn.Offset==0 )
        {
            break;
        }
        ret += AddressToSymbolName((void*)stackFrame.AddrReturn.Offset);
    }

    return ret;
}



DWORD FilterFunction(EXCEPTION_POINTERS *ep)
{
    printf("%s\n", ExceptionToSymbolNames(ep).c_str());
    return EXCEPTION_EXECUTE_HANDLER;
}

int main(int argc, char *argv[])
{
    ::SymInitialize(::GetCurrentProcess(), NULL, TRUE);
    ::SymSetOptions(SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES);

    __try
    {
        int *nullpo = NULL;
        *nullpo = 0;
    }
    __except( FilterFunction(GetExceptionInformation()) ) // ここで実行しないと意図した callstack は得られない
    {
    }

    ::SymCleanup(::GetCurrentProcess());
}
/*
$ cl ExceptionCallstack.cpp /EHsc /Zi
$ ./ExceptionCallstack
c:\project\scribble\exceptioncallstack.cpp(108): ExceptionCallstack!main + 0x3e [0x000000013F32184E]
f:\dd\vctools\crt_bld\self_64_amd64\crt\src\crt0.c(278): ExceptionCallstack!__tmainCRTStartup + 0x13b [0x000000013F324F5B]
kernel32!BaseThreadInitThunk + 0xd [0x0000000076A2652D]
ntdll!RtlUserThreadStart + 0x21 [0x0000000076F0C521]
*/
