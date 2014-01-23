#ifndef rpsFuncTypes_h
#define rpsFuncTypes_h


typedef BOOL (WINAPI *CloseHandleT)(_In_  HANDLE hObject);

// heap
typedef LPVOID (WINAPI *HeapAllocT)( HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes );
typedef LPVOID (WINAPI *HeapReAllocT)( HANDLE hHeap, DWORD dwFlags, LPVOID lpMem, SIZE_T dwBytes );
typedef BOOL (WINAPI *HeapFreeT)( HANDLE hHeap, DWORD dwFlags, LPVOID lpMem );
typedef BOOL (WINAPI *HeapValidateT)( HANDLE hHeap, DWORD dwFlags, LPCVOID lpMem );

// thread
typedef LPVOID (WINAPI *CreateThreadT)(
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    SIZE_T dwStackSize,
    LPTHREAD_START_ROUTINE lpStartAddress,
    LPVOID lpParameter,
    DWORD dwCreationFlags,
    LPDWORD lpThreadId
    );


// critical section
typedef void (WINAPI *InitializeCriticalSectionT)(LPCRITICAL_SECTION lpCriticalSection);
typedef void (WINAPI *DeleteCriticalSectionT)(LPCRITICAL_SECTION lpCriticalSection);
typedef void (WINAPI *EnterCriticalSectionT)(LPCRITICAL_SECTION lpCriticalSection);
typedef void (WINAPI *LeaveCriticalSectionT)(LPCRITICAL_SECTION lpCriticalSection);
typedef BOOL (WINAPI *TryEnterCriticalSectionT)(LPCRITICAL_SECTION lpCriticalSection);


// file
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

#endif // rpsFuncTypes_h
