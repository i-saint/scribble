#include "rps.h"
#include "rpsInlines.h"

class rpsFiles : public rpsIModule
{
public:
    static rpsFiles* getInstance();

    rpsFiles();
    ~rpsFiles();
    virtual const char*		getModuleName() const;
    virtual size_t			getNumHooks() const;
    virtual rpsHookInfo*	getHooks() const;
    virtual void serialize(rpsArchive &ar);

    HANDLE translate(HANDLE h);

private:
};


typedef HANDLE (WINAPI *CreateFileAT)(
    LPCSTR lpFileName,
    DWORD dwDesiredAccess,
    DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes,
    HANDLE hTemplateFile
    );

typedef HANDLE (WINAPI *CreateFileWT)(
    LPCWSTR lpFileName,
    DWORD dwDesiredAccess,
    DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes,
    HANDLE hTemplateFile
    );

typedef HANDLE (WINAPI *CreateFile2T)(
    LPCTSTR lpFileName,
    DWORD dwDesiredAccess,
    DWORD dwShareMode,
    DWORD dwCreationDisposition,
    LPCREATEFILE2_EXTENDED_PARAMETERS pCreateExParams
    );

typedef BOOL (WINAPI *WriteFileT)(
    HANDLE hFile,
    LPCVOID lpBuffer,
    DWORD nNumberOfBytesToWrite,
    LPDWORD lpNumberOfBytesWritten,
    LPOVERLAPPED lpOverlapped
    );

typedef BOOL (WINAPI *ReadFileT)(
    HANDLE hFile,
    LPVOID lpBuffer,
    DWORD nNumberOfBytesToRead,
    LPDWORD lpNumberOfBytesRead,
    LPOVERLAPPED lpOverlapped
    );

CreateFileAT origCreateFileA;
CreateFileWT origCreateFileW;
CreateFile2T origCreateFile2;
WriteFileT origWriteFile;
ReadFileT origReadFile;


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
    CreateFileA;
    HANDLE ret = origCreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
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
    HANDLE ret = origCreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
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
    HANDLE ret = origCreateFile2(lpFileName, dwDesiredAccess, dwShareMode, dwCreationDisposition, pCreateExParams);
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
    BOOL ret = origWriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
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
    BOOL ret = origReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
    return ret;
}

static rpsHookInfo g_rps_hooks[] = {
    rpsHookInfo("kernel32.dll", "CreateFileA", 0, rpsCreateFileA, &(void*&)origCreateFileA),
    rpsHookInfo("kernel32.dll", "CreateFileW", 0, rpsCreateFileW, &(void*&)origCreateFileW),
    rpsHookInfo("kernel32.dll", "CreateFile2", 0, rpsCreateFile2, &(void*&)origCreateFile2),
    rpsHookInfo("kernel32.dll", "WriteFile",   0, rpsWriteFile,   &(void*&)origWriteFile),
    rpsHookInfo("kernel32.dll", "ReadFile",    0, rpsReadFile,    &(void*&)origReadFile),
};


const char*     rpsFiles::getModuleName() const { return "rpsFiles"; }
size_t          rpsFiles::getNumHooks() const   { return _countof(g_rps_hooks); }
rpsHookInfo*    rpsFiles::getHooks() const      { return g_rps_hooks; }


rpsFiles* rpsFiles::getInstance()
{
    static rpsFiles *s_inst;
    if(!s_inst) { s_inst = new rpsFiles(); }
    return s_inst;
}

rpsFiles::rpsFiles()
{
}

rpsFiles::~rpsFiles()
{
}

void rpsFiles::serialize(rpsArchive &ar)
{
}

HANDLE rpsFiles::translate(HANDLE h)
{
    return nullptr;
}

rpsIModule* rpsCreateFiles() { return rpsFiles::getInstance(); }
