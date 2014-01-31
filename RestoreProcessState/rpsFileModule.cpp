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

    void addFileRecord(HANDLE rps_handle, const rpsFileRecord &stat)
    {
        rpsMutex::ScopedLock lock(m_mutex);
        m_files[rps_handle] = stat;
    }

    bool eraseFileRecord(HANDLE rps_handle)
    {
        if(!rpsIsRpsHandle(rps_handle)) { return nullptr; }

        rpsMutex::ScopedLock lock(m_mutex);
        auto it = m_files.find(rps_handle);
        if(it!=m_files.end()) {
            m_files.erase(it);
            return true;
        }
        return false;
    }

    rpsFileRecord* findFileRecord(HANDLE rps_handle)
    {
        if(!rpsIsRpsHandle(rps_handle)) { return nullptr; }

        rpsMutex::ScopedLock lock(m_mutex);
        auto it = m_files.find(rps_handle);
        if(it!=m_files.end()) {
            return &it->second;
        }
        return nullptr;
    }

private:
    typedef std::map<HANDLE, rpsFileRecord, std::less<HANDLE>, rps_allocator<std::pair<HANDLE, rpsFileRecord> > > FileRecords;
    rpsMutex m_mutex;
    FileRecords m_files;
};
typedef rpsFileModule rpsCurrentModule;
inline rpsCurrentModule* rpsGetCurrentModule() { return rpsCurrentModule::getInstance(); }


CreateFileAT        vaCreateFileA;
CreateFileWT        vaCreateFileW;
CreateFile2T        vaCreateFile2;
WriteFileT          vaWriteFile;
ReadFileT           vaReadFile;
SetFilePointerT     vaSetFilePointer;
SetFilePointerExT   vaSetFilePointerEx;
GetFileTypeT        vaGetFileType;
CloseHandleT        vaCloseHandle;

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
    rpsGetCurrentModule()->addFileRecord(rps_handle, record);
    return rps_handle;
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
    HANDLE win_handle = vaCreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
    HANDLE rps_handle = rpsCreateHandle(rpsFileModule::getInstance(), win_handle);
    rpsFileRecord record = {lpFileName, rps_handle, win_handle, 0, dwDesiredAccess, dwShareMode, dwCreationDisposition, dwFlagsAndAttributes};
    rpsGetCurrentModule()->addFileRecord(rps_handle, record);
    return rps_handle;
}

HANDLE WINAPI rpsCreateFile2(
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
    rpsGetCurrentModule()->addFileRecord(rps_handle, record);
    return rps_handle;
}

BOOL WINAPI rpsWriteFile(
    HANDLE hFile,
    LPCVOID lpBuffer,
    DWORD nNumberOfBytesToWrite,
    LPDWORD lpNumberOfBytesWritten,
    LPOVERLAPPED lpOverlapped
    )
{
    if(rpsFileRecord *rec=rpsGetCurrentModule()->findFileRecord(hFile)) {
        DWORD written = 0;
        BOOL ret = vaWriteFile(rec->win_handle, lpBuffer, nNumberOfBytesToWrite, &written, lpOverlapped);
        if(ret) {
            rec->pos += written;
            if(lpNumberOfBytesWritten) {
                *lpNumberOfBytesWritten = written;
            }
        }
        return ret;
    }
    else {
        return vaWriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
    }
}

BOOL WINAPI rpsReadFile(
    HANDLE hFile,
    LPVOID lpBuffer,
    DWORD nNumberOfBytesToRead,
    LPDWORD lpNumberOfBytesRead,
    LPOVERLAPPED lpOverlapped
    )
{
    if(rpsFileRecord *rec=rpsGetCurrentModule()->findFileRecord(hFile)) {
        DWORD read = 0;
        BOOL ret = vaReadFile(rec->win_handle, lpBuffer, nNumberOfBytesToRead, &read, lpOverlapped);
        if(ret) {
            rec->pos += read;
            if(lpNumberOfBytesRead) {
                *lpNumberOfBytesRead = read;
            }
        }
        return ret;
    }
    else {
        return vaReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
    }
}

DWORD WINAPI rpsSetFilePointer(
    HANDLE hFile,
    LONG lDistanceToMove,
    PLONG lpDistanceToMoveHigh,
    DWORD dwMoveMethod
    )
{
    return vaSetFilePointer(rpsTranslateHandle(hFile), lDistanceToMove, lpDistanceToMoveHigh, dwMoveMethod);
}

BOOL WINAPI rpsSetFilePointerEx(
    HANDLE hFile,
    LARGE_INTEGER liDistanceToMove,
    PLARGE_INTEGER lpNewFilePointer,
    DWORD dwMoveMethod
    )
{
    return vaSetFilePointerEx(rpsTranslateHandle(hFile), liDistanceToMove, lpNewFilePointer, dwMoveMethod);
}

DWORD WINAPI rpsGetFileType(HANDLE hFile)
{
    return vaGetFileType(rpsTranslateHandle(hFile));
}

BOOL WINAPI rpsCloseHandle(HANDLE hObject)
{
    if(rpsFileRecord *rec=rpsGetCurrentModule()->findFileRecord(hObject)) {
        BOOL ret = vaCloseHandle(rec->win_handle);
        if(ret) {
            rpsGetCurrentModule()->eraseFileRecord(hObject);
        }
        return ret;
    }
    else {
        return vaCloseHandle(hObject);
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
    rpsHookInfo("kernel32.dll", "CloseHandle",       0, rpsCloseHandle,      &(void*&)vaCloseHandle      ),

    rpsHookInfo(nullptr, nullptr, 0, nullptr, nullptr),
};



inline rpsArchive& operator&(rpsArchive &ar, HANDLE &v)
{
    return ar & (size_t&)v;
}

inline rpsArchive& operator&(rpsArchive &ar, rpsFileRecord &v)
{
    ar & v.path & v.rps_handle & v.win_handle & v.pos & v.access_mode & v.share_mode & v.disposition & v.attributes;
    if(ar.isReader()) {
        DWORD disposition = v.disposition;
        if(v.access_mode==GENERIC_WRITE) {
            disposition = OPEN_EXISTING;
        }
        v.win_handle = vaCreateFileW(v.path.c_str(), v.access_mode, v.share_mode, nullptr, v.disposition, v.attributes, nullptr);
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
    if(ar.isReader()) {
        rpsEach(m_files, [](FileRecords::value_type &rec){
            vaCloseHandle(rec.second.win_handle);
        });
    }
    ar & m_files;
}

} // namespace

rpsDLLExport rpsIModule* rpsCreateFileModule() { return rpsFileModule::getInstance(); }
