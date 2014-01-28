#include "rpsPCH.h"
#include "rpsInternal.h"

namespace {

class rpsFileModule : public rpsIModule
{
public:
    static rpsFileModule* getInstance();

    rpsFileModule();
    ~rpsFileModule();
    virtual const char*     getModuleName() const;
    virtual rpsHookInfo*    getHooks() const;
    virtual void initialize();
    virtual void serialize(rpsArchive &ar);

    HANDLE translate(HANDLE h);

private:
};


CreateFileAT vaCreateFileA;
CreateFileWT vaCreateFileW;
CreateFile2T vaCreateFile2;
WriteFileT vaWriteFile;
ReadFileT vaReadFile;


HANDLE WINAPI rpsCreateFileA(
    LPCSTR lpFileName,
    DWORD dwDesiredAccess,
    DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes,
    HANDLE hTemplateFile
    )
{
    HANDLE ret = vaCreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
    return ret;
}

HANDLE WINAPI rpsCreateFileW(
    LPCWSTR lpFileName,
    DWORD dwDesiredAccess,
    DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes,
    HANDLE hTemplateFile
    )
{
    HANDLE ret = vaCreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
    return ret;
}

HANDLE WINAPI rpsCreateFile2(
    LPCTSTR lpFileName,
    DWORD dwDesiredAccess,
    DWORD dwShareMode,
    DWORD dwCreationDisposition,
    LPCREATEFILE2_EXTENDED_PARAMETERS pCreateExParams
    )
{
    HANDLE ret = vaCreateFile2(lpFileName, dwDesiredAccess, dwShareMode, dwCreationDisposition, pCreateExParams);
    return ret;
}

BOOL WINAPI rpsWriteFile(
    HANDLE hFile,
    LPCVOID lpBuffer,
    DWORD nNumberOfBytesToWrite,
    LPDWORD lpNumberOfBytesWritten,
    LPOVERLAPPED lpOverlapped
    )
{
    BOOL ret = vaWriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
    return ret;
}

BOOL WINAPI rpsReadFile(
    HANDLE hFile,
    LPVOID lpBuffer,
    DWORD nNumberOfBytesToRead,
    LPDWORD lpNumberOfBytesRead,
    LPOVERLAPPED lpOverlapped
    )
{
    BOOL ret = vaReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
    return ret;
}

rpsHookInfo g_hookinfo[] = {
    rpsHookInfo("kernel32.dll", "CreateFileA", 0, rpsCreateFileA, &(void*&)vaCreateFileA),
    rpsHookInfo("kernel32.dll", "CreateFileW", 0, rpsCreateFileW, &(void*&)vaCreateFileW),
    rpsHookInfo("kernel32.dll", "CreateFile2", 0, rpsCreateFile2, &(void*&)vaCreateFile2),
    rpsHookInfo("kernel32.dll", "WriteFile",   0, rpsWriteFile,   &(void*&)vaWriteFile),
    rpsHookInfo("kernel32.dll", "ReadFile",    0, rpsReadFile,    &(void*&)vaReadFile),

    rpsHookInfo(nullptr, nullptr, 0, nullptr, nullptr),
};

const char*     rpsFileModule::getModuleName() const { return "rpsFileModule"; }
rpsHookInfo*    rpsFileModule::getHooks() const      { return g_hookinfo; }


rpsFileModule* rpsFileModule::getInstance()
{
    static rpsFileModule *s_inst = new rpsFileModule();
    return s_inst;
}

rpsFileModule::rpsFileModule()
{
}

rpsFileModule::~rpsFileModule()
{
}

void rpsFileModule::initialize()
{
}

void rpsFileModule::serialize(rpsArchive &ar)
{
}

HANDLE rpsFileModule::translate(HANDLE h)
{
    // todo
    return h;
}

} // namespace

rpsDLLExport rpsIModule* rpsCreateFileModule() { return rpsFileModule::getInstance(); }
