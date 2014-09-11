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
inline rpsArchive& operator&(rpsArchive &ar, rpsFileRecord &v)
{
    return ar & v.path & v.rps_handle & v.win_handle & v.pos & v.access_mode & v.share_mode & v.disposition & v.attributes;
}
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
GetFileSizeT        vaGetFileSize;
GetFileSizeExT      vaGetFileSizeEx;
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
    rpsLogInfo("rpsCreateFileA(\"%s\", %x, %x, %p, %x, %x, %p): %p\n",
        lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile, win_handle);
    HANDLE rps_handle = rpsCreateHandle(rpsCurrentModule::getInstance(), win_handle);
    rps_wstring path = rpsL(lpFileName);
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
    rpsLogInfo("rpsCreateFileW(\"%s\", %x, %x, %p, %x, %x, %p): %p\n",
        lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile, win_handle);
    HANDLE rps_handle = rpsCreateHandle(rpsCurrentModule::getInstance(), win_handle);
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
    rpsLogInfo("rpsCreateFile2(\"%s\", %x, %x, %x, %p): %p\n",
        lpFileName, dwDesiredAccess, dwShareMode, dwCreationDisposition, pCreateExParams, win_handle);
    HANDLE rps_handle = rpsCreateHandle(rpsCurrentModule::getInstance(), win_handle);
    rps_wstring path = rpsL(lpFileName);
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
        BOOL ret = vaWriteFile(rpsToWinHandleC(hFile, vaWriteFile), lpBuffer, nNumberOfBytesToWrite, &written, lpOverlapped);
        rpsLogInfo("rpsWriteFile(%p, %p, %u, %p, %p): %u\n",
            hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped, ret);
        if(ret) {
            rec->pos += written;
            if(lpNumberOfBytesWritten) {
                *lpNumberOfBytesWritten = written;
            }
        }
        return ret;
    }
    else {
        return vaWriteFile(rpsToWinHandleC(hFile, vaWriteFile), lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
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
        BOOL ret = vaReadFile(rpsToWinHandleC(hFile, vaReadFile), lpBuffer, nNumberOfBytesToRead, &read, lpOverlapped);
        rpsLogInfo("rpsReadFile(%p, %p, %u, %p, %p): %u\n",
            hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped, ret);
        if(ret) {
            rec->pos += read;
            if(lpNumberOfBytesRead) {
                *lpNumberOfBytesRead = read;
            }
        }
        return ret;
    }
    else {
        return vaReadFile(rpsToWinHandleC(hFile, vaReadFile), lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
    }
}

rpsHookAPI DWORD WINAPI rpsSetFilePointer(
    HANDLE hFile,
    LONG lDistanceToMove,
    PLONG lpDistanceToMoveHigh,
    DWORD dwMoveMethod
    )
{
    DWORD ret = vaSetFilePointer(
        rpsToWinHandleC(hFile, vaSetFilePointer),
        lDistanceToMove,
        lpDistanceToMoveHigh,
        dwMoveMethod);
    return ret;
}

rpsHookAPI BOOL WINAPI rpsSetFilePointerEx(
    HANDLE hFile,
    LARGE_INTEGER liDistanceToMove,
    PLARGE_INTEGER lpNewFilePointer,
    DWORD dwMoveMethod
    )
{
    BOOL ret = vaSetFilePointerEx(
        rpsToWinHandleC(hFile, vaSetFilePointerEx),
        liDistanceToMove,
        lpNewFilePointer,
        dwMoveMethod);
    return ret;
}

rpsHookAPI DWORD WINAPI rpsGetFileType(HANDLE hFile)
{
    DWORD ret = vaGetFileType(rpsToWinHandleC(hFile, vaGetFileType));
    rpsLogInfo("rpsGetFileType(%p): %u\n", hFile, ret);
    return ret;
}

rpsHookAPI DWORD  WINAPI rpsGetFileSize(HANDLE hFile, LPDWORD lpFileSizeHigh)
{
    DWORD ret = vaGetFileSize(rpsToWinHandleC(hFile, vaGetFileSize), lpFileSizeHigh);
    rpsLogInfo("rpsGetFileSize(%p, %p): %u\n", hFile, lpFileSizeHigh, ret);
    return ret;
}

rpsHookAPI BOOL   WINAPI rpsGetFileSizeEx(HANDLE hFile, PLARGE_INTEGER lpFileSize)
{
    DWORD ret = vaGetFileSizeEx(rpsToWinHandleC(hFile, vaGetFileSize), lpFileSize);
    rpsLogInfo("rpsGetFileSizeEx(%p, %p): %u\n", hFile, lpFileSize, ret);
    return ret;
}

rpsHookAPI BOOL WINAPI rpsCloseHandle(HANDLE hObject)
{
    if(rpsFileRecord *rec=rpsGetFileRecords()->findRecord(hObject)) {
        HANDLE win_handle = rpsToWinHandleC(hObject, vaCloseHandle);
        BOOL ret = vaCloseHandle(win_handle);
        rpsLogInfo("rpsCloseHandle(%p): %u\n", win_handle, ret);
        if(ret) {
            rpsGetFileRecords()->eraseRecord(hObject);
        }
        return ret;
    }
    else {
        return vaCloseHandle(rpsToWinHandleC(hObject, vaCloseHandle));
    }
}

rpsHookInfo g_hookinfo[] = {
    rpsDefineHookInfo("kernel32.dll", CreateFileA),
    rpsDefineHookInfo("kernel32.dll", CreateFileW),
    rpsDefineHookInfo("kernel32.dll", CreateFile2),
    rpsDefineHookInfo("kernel32.dll", WriteFile),
    rpsDefineHookInfo("kernel32.dll", ReadFile),
    rpsDefineHookInfo("kernel32.dll", SetFilePointer),
    rpsDefineHookInfo("kernel32.dll", SetFilePointerEx),
    rpsDefineHookInfo("kernel32.dll", GetFileType),
    rpsDefineHookInfo("kernel32.dll", GetFileSize),
    rpsDefineHookInfo("kernel32.dll", GetFileSizeEx),
    rpsDefineHookInfo("kernel32.dll", CloseHandle),

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

    if(ar.isReader()) {
        rpsEach(m_records, [](rpsFileRecords::Pair &rec){
            rpsFileRecord &v = rec.second;
            DWORD disposition = v.disposition;
            if(v.access_mode==GENERIC_WRITE) {
                disposition = OPEN_EXISTING;
            }
            v.win_handle = vaCreateFileW(v.path.c_str(), v.access_mode, v.share_mode, nullptr, disposition, v.attributes, nullptr);
            rpsGetHandleInfo(v.rps_handle)->win_handle = v.win_handle;
            vaSetFilePointerEx(v.win_handle, (LARGE_INTEGER&)v.pos, nullptr, FILE_BEGIN);
        });
    }
}

} // namespace

rpsDLLExport rpsIModule* rpsCreateFileModule() { return rpsFileModule::getInstance(); }
