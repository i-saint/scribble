#include "rpsPCH.h"
#include "rpsInternal.h"

namespace {

struct rpsFileRecord
{
    rps_wstring path;
    HANDLE rps_handle;
    HANDLE win_handle;
    uint64_t pos;
    DWORD access_mode;
    DWORD share_mode;
    DWORD disposition;
    DWORD attributes;
};
typedef rpsTHandleRecords<rpsFileRecord> rpsFileRecords;

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

    rpsFileRecords* getFileRecords() { return &m_file_records; }

private:
    rpsFileRecords m_file_records;
};
typedef rpsFileModule rpsCurrentModule;
inline rpsCurrentModule* rpsGetCurrentModule() { return rpsCurrentModule::getInstance(); }
inline rpsFileRecords* rpsGetFileRecords() { return rpsGetCurrentModule()->getFileRecords(); }


CreateFileAT        vaCreateFileA;
CreateFileWT        vaCreateFileW;
CreateFile2T        vaCreateFile2;
WriteFileT          vaWriteFile;
ReadFileT           vaReadFile;
SetFilePointerT     vaSetFilePointer;
SetFilePointerExT   vaSetFilePointerEx;
GetFileTypeT        vaGetFileType;
CloseHandleT        vaCloseHandle;

rpsHookAPI HANDLE WINAPI rpsCreateFileA(
    LPCSTR lpFileName,
    DWORD dwDesiredAccess,
    DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes,
    HANDLE hTemplateFile
    )
{
    HANDLE win_handle = vaCreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
    HANDLE rps_handle = rpsCreateHandle(rpsFileModule::getInstance(), win_handle);
    rps_wstring path;
    {
        size_t mblen = strlen(lpFileName);
        size_t len = mbstowcs(nullptr, lpFileName, mblen);
        path.resize(len);
        mbstowcs(&path[0], lpFileName, mblen);
    }
    rpsFileRecord record = {path, rps_handle, win_handle, 0, dwDesiredAccess, dwShareMode, dwCreationDisposition, dwFlagsAndAttributes};
    rpsGetFileRecords()->addRecord(rps_handle, record);
    return rps_handle;
}

rpsHookAPI HANDLE WINAPI rpsCreateFileW(
    LPCWSTR lpFileName,
    DWORD dwDesiredAccess,
    DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes,
    HANDLE hTemplateFile
    )
{
    HANDLE win_handle = vaCreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
    HANDLE rps_handle = rpsCreateHandle(rpsFileModule::getInstance(), win_handle);
    rpsFileRecord record = {lpFileName, rps_handle, win_handle, 0, dwDesiredAccess, dwShareMode, dwCreationDisposition, dwFlagsAndAttributes};
    rpsGetFileRecords()->addRecord(rps_handle, record);
    return rps_handle;
}

rpsHookAPI HANDLE WINAPI rpsCreateFile2(
    LPCTSTR lpFileName,
    DWORD dwDesiredAccess,
    DWORD dwShareMode,
    DWORD dwCreationDisposition,
    LPCREATEFILE2_EXTENDED_PARAMETERS pCreateExParams
    )
{
    HANDLE win_handle = vaCreateFile2(lpFileName, dwDesiredAccess, dwShareMode, dwCreationDisposition, pCreateExParams);
    HANDLE rps_handle = rpsCreateHandle(rpsFileModule::getInstance(), win_handle);
    rps_wstring path;
    {
        size_t mblen = strlen(lpFileName);
        size_t len = mbstowcs(nullptr, lpFileName, mblen);
        path.resize(len);
        mbstowcs(&path[0], lpFileName, mblen);
    }
    rpsFileRecord record = {path, rps_handle, win_handle, 0, dwDesiredAccess, dwShareMode, dwCreationDisposition, pCreateExParams->dwFileAttributes};
    rpsGetFileRecords()->addRecord(rps_handle, record);
    return rps_handle;
}

rpsHookAPI BOOL WINAPI rpsWriteFile(
    HANDLE hFile,
    LPCVOID lpBuffer,
    DWORD nNumberOfBytesToWrite,
    LPDWORD lpNumberOfBytesWritten,
    LPOVERLAPPED lpOverlapped
    )
{
    if(rpsFileRecord *rec=rpsGetFileRecords()->findRecord(hFile)) {
        DWORD written = 0;
        BOOL ret = vaWriteFile(rpsTranslateHandleC(hFile, vaWriteFile), lpBuffer, nNumberOfBytesToWrite, &written, lpOverlapped);
        if(ret) {
            rec->pos += written;
            if(lpNumberOfBytesWritten) {
                *lpNumberOfBytesWritten = written;
            }
        }
        return ret;
    }
    else {
        return vaWriteFile(rpsTranslateHandleC(hFile, vaWriteFile), lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
    }
}

rpsHookAPI BOOL WINAPI rpsReadFile(
    HANDLE hFile,
    LPVOID lpBuffer,
    DWORD nNumberOfBytesToRead,
    LPDWORD lpNumberOfBytesRead,
    LPOVERLAPPED lpOverlapped
    )
{
    if(rpsFileRecord *rec=rpsGetFileRecords()->findRecord(hFile)) {
        DWORD read = 0;
        BOOL ret = vaReadFile(rpsTranslateHandleC(hFile, vaReadFile), lpBuffer, nNumberOfBytesToRead, &read, lpOverlapped);
        if(ret) {
            rec->pos += read;
            if(lpNumberOfBytesRead) {
                *lpNumberOfBytesRead = read;
            }
        }
        return ret;
    }
    else {
        return vaReadFile(rpsTranslateHandleC(hFile, vaReadFile), lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
    }
}

rpsHookAPI DWORD WINAPI rpsSetFilePointer(
    HANDLE hFile,
    LONG lDistanceToMove,
    PLONG lpDistanceToMoveHigh,
    DWORD dwMoveMethod
    )
{
    return vaSetFilePointer(
        rpsTranslateHandleC(hFile, vaSetFilePointer),
        lDistanceToMove,
        lpDistanceToMoveHigh,
        dwMoveMethod);
}

rpsHookAPI BOOL WINAPI rpsSetFilePointerEx(
    HANDLE hFile,
    LARGE_INTEGER liDistanceToMove,
    PLARGE_INTEGER lpNewFilePointer,
    DWORD dwMoveMethod
    )
{
    return vaSetFilePointerEx(
        rpsTranslateHandleC(hFile, vaSetFilePointerEx),
        liDistanceToMove,
        lpNewFilePointer,
        dwMoveMethod);
}

rpsHookAPI DWORD WINAPI rpsGetFileType(HANDLE hFile)
{
    return vaGetFileType(rpsTranslateHandleC(hFile, vaGetFileType));
}

rpsHookAPI BOOL WINAPI rpsCloseHandle(HANDLE hObject)
{
    if(rpsFileRecord *rec=rpsGetFileRecords()->findRecord(hObject)) {
        BOOL ret = vaCloseHandle(rpsTranslateHandleC(hObject, vaCloseHandle));
        if(ret) {
            rpsGetFileRecords()->eraseRecord(hObject);
        }
        return ret;
    }
    else {
        return vaCloseHandle(rpsTranslateHandleC(hObject, vaCloseHandle));
    }
}

rpsHookInfo g_hookinfo[] = {
    rpsHookInfo("kernel32.dll", "CreateFileA",      0, rpsCreateFileA,      &(void*&)vaCreateFileA      ),
    rpsHookInfo("kernel32.dll", "CreateFileW",      0, rpsCreateFileW,      &(void*&)vaCreateFileW      ),
    rpsHookInfo("kernel32.dll", "CreateFile2",      0, rpsCreateFile2,      &(void*&)vaCreateFile2      ),
    rpsHookInfo("kernel32.dll", "WriteFile",        0, rpsWriteFile,        &(void*&)vaWriteFile        ),
    rpsHookInfo("kernel32.dll", "ReadFile",         0, rpsReadFile,         &(void*&)vaReadFile         ),
    rpsHookInfo("kernel32.dll", "SetFilePointer",   0, rpsSetFilePointer,   &(void*&)vaSetFilePointer   ),
    rpsHookInfo("kernel32.dll", "SetFilePointerEx", 0, rpsSetFilePointerEx, &(void*&)vaSetFilePointerEx ),
    rpsHookInfo("kernel32.dll", "GetFileType",      0, rpsGetFileType,      &(void*&)vaGetFileType      ),
    rpsHookInfo("kernel32.dll", "CloseHandle",      0, rpsCloseHandle,      &(void*&)vaCloseHandle      ),

    rpsHookInfo(nullptr, nullptr, 0, nullptr, nullptr),
};



inline rpsArchive& operator&(rpsArchive &ar, rpsFileRecord &v)
{
    ar & v.path & v.rps_handle & v.win_handle & v.pos & v.access_mode & v.share_mode & v.disposition & v.attributes;
    if(ar.isReader()) {
        DWORD disposition = v.disposition;
        if(v.access_mode==GENERIC_WRITE) {
            disposition = OPEN_EXISTING;
        }
        v.win_handle = vaCreateFileW(v.path.c_str(), v.access_mode, v.share_mode, nullptr, disposition, v.attributes, nullptr);
        rpsGetHandleInfo(v.rps_handle)->win_handle = v.win_handle;
        vaSetFilePointerEx(v.win_handle, (LARGE_INTEGER&)v.pos, nullptr, FILE_BEGIN);
    }
    return ar;
}


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
    ar & m_file_records;
}

void rpsFileRecords::serialize(rpsArchive &ar)
{
    if(ar.isReader()) {
        rpsEach(m_records, [](rpsFileRecords::Pair &rec){
            vaCloseHandle(rec.second.win_handle);
        });
    }
    ar & m_records;
}

} // namespace

rpsDLLExport rpsIModule* rpsCreateFileModule() { return rpsFileModule::getInstance(); }
