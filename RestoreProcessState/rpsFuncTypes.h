#ifndef rpsFuncTypes_h
#define rpsFuncTypes_h


typedef BOOL (WINAPI *CloseHandleT)(_In_  HANDLE hObject);

// memory
typedef LPVOID (WINAPI *HeapAllocT)( HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes );
typedef LPVOID (WINAPI *HeapReAllocT)( HANDLE hHeap, DWORD dwFlags, LPVOID lpMem, SIZE_T dwBytes );
typedef BOOL   (WINAPI *HeapFreeT)( HANDLE hHeap, DWORD dwFlags, LPVOID lpMem );
typedef BOOL   (WINAPI *HeapValidateT)( HANDLE hHeap, DWORD dwFlags, LPCVOID lpMem );
typedef SIZE_T (WINAPI *HeapSizeT)(HANDLE hHeap, DWORD dwFlags, LPCVOID lpMem);

typedef HGLOBAL (WINAPI *GlobalAllocT)(UINT uFlags, SIZE_T dwBytes);
typedef HGLOBAL (WINAPI *GlobalReAllocT)(HGLOBAL hMem, SIZE_T dwBytes, UINT uFlags);
typedef HGLOBAL (WINAPI *GlobalFreeT)(HGLOBAL hMem);
typedef SIZE_T  (WINAPI *GlobalSizeT)(HGLOBAL hMem);

typedef HLOCAL  (WINAPI *LocalAllocT)(UINT uFlags, SIZE_T uBytes);
typedef HLOCAL  (WINAPI *LocalReAllocT)(HLOCAL hMem, SIZE_T uBytes, UINT uFlags);
typedef HLOCAL  (WINAPI *LocalFreeT)(HLOCAL hMem);
typedef UINT    (WINAPI *LocalSizeT)(HLOCAL hMem);

typedef LPVOID (WINAPI *VirtualAllocT)(LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect);
typedef BOOL   (WINAPI *VirtualFreeT)(LPVOID lpAddress, SIZE_T dwSize, DWORD dwFreeType);
typedef LPVOID (WINAPI *VirtualAllocExT)(HANDLE hProcess, LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect);
typedef BOOL   (WINAPI *VirtualFreeExT)(HANDLE hProcess, LPVOID lpAddress, SIZE_T dwSize, DWORD dwFreeType);

// handle
typedef BOOL  (WINAPI *CloseHandleT)(HANDLE hObject);
typedef DWORD (WINAPI *WaitForSingleObjectT)(HANDLE hHandle, DWORD dwMilliseconds);

// thread
typedef HANDLE (WINAPI *CreateThreadT)(LPSECURITY_ATTRIBUTES lpThreadAttributes, SIZE_T dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId);
typedef HANDLE (WINAPI *OpenThreadT)(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwThreadId);
typedef BOOL   (WINAPI *GetThreadTimesT)(HANDLE hThread, LPFILETIME lpCreationTime, LPFILETIME lpExitTime, LPFILETIME lpKernelTime, LPFILETIME lpUserTime);
typedef DWORD  (WINAPI *GetThreadIdT)(HANDLE Thread);

// critical section
typedef void  (WINAPI *DeleteCriticalSectionT)(LPCRITICAL_SECTION lpCriticalSection);
typedef void  (WINAPI *EnterCriticalSectionT)(LPCRITICAL_SECTION lpCriticalSection);
typedef void  (WINAPI *InitializeCriticalSectionT)(LPCRITICAL_SECTION lpCriticalSection);
typedef BOOL  (WINAPI *InitializeCriticalSectionAndSpinCountT)(LPCRITICAL_SECTION lpCriticalSection, DWORD dwSpinCount);
typedef void  (WINAPI *LeaveCriticalSectionT)(LPCRITICAL_SECTION lpCriticalSection);
typedef DWORD (WINAPI *SetCriticalSectionSpinCountT)(LPCRITICAL_SECTION lpCriticalSection, DWORD dwSpinCount);
typedef BOOL  (WINAPI *TryEnterCriticalSectionT)(LPCRITICAL_SECTION lpCriticalSection);

// event
typedef HANDLE (WINAPI *CreateEventAT)(LPSECURITY_ATTRIBUTES lpEventAttributes, BOOL bManualReset, BOOL bInitialState, LPCSTR lpName);
typedef HANDLE (WINAPI *CreateEventWT)(LPSECURITY_ATTRIBUTES lpEventAttributes, BOOL bManualReset, BOOL bInitialState, LPCWSTR lpName);
typedef HANDLE (WINAPI *CreateEventExAT)(LPSECURITY_ATTRIBUTES lpEventAttributes, LPCSTR lpName, DWORD dwFlags, DWORD dwDesiredAccess);
typedef HANDLE (WINAPI *CreateEventExWT)(LPSECURITY_ATTRIBUTES lpEventAttributes, LPCWSTR lpName, DWORD dwFlags, DWORD dwDesiredAccess);
typedef HANDLE (WINAPI *OpenEventAT)(DWORD dwDesiredAccess, BOOL bInheritHandle, LPCSTR lpName);
typedef HANDLE (WINAPI *OpenEventWT)(DWORD dwDesiredAccess, BOOL bInheritHandle, LPCWSTR lpName);
typedef BOOL   (WINAPI *PulseEventT)(HANDLE hEvent);
typedef BOOL   (WINAPI *ResetEventT)(HANDLE hEvent);
typedef BOOL   (WINAPI *SetEventT)(HANDLE hEvent);

// mutex
typedef HANDLE (WINAPI *CreateMutexAT)(LPSECURITY_ATTRIBUTES lpMutexAttributes, BOOL bInitialOwner, LPCSTR lpName);
typedef HANDLE (WINAPI *CreateMutexWT)(LPSECURITY_ATTRIBUTES lpMutexAttributes, BOOL bInitialOwner, LPCWSTR lpName);
typedef HANDLE (WINAPI *CreateMutexExAT)(LPSECURITY_ATTRIBUTES lpMutexAttributes, LPCSTR lpName, DWORD dwFlags, DWORD dwDesiredAccess);
typedef HANDLE (WINAPI *CreateMutexExWT)(LPSECURITY_ATTRIBUTES lpMutexAttributes, LPCWSTR lpName, DWORD dwFlags, DWORD dwDesiredAccess);
typedef HANDLE (WINAPI *OpenMutexAT)(DWORD dwDesiredAccess, BOOL bInheritHandle, LPCSTR lpName);
typedef HANDLE (WINAPI *OpenMutexWT)(DWORD dwDesiredAccess, BOOL bInheritHandle, LPCWSTR lpName);
typedef BOOL   (WINAPI *ReleaseMutexT)(HANDLE hMutex);

// semaphore
typedef HANDLE (WINAPI *CreateSemaphoreAT)(LPSECURITY_ATTRIBUTES lpSemaphoreAttributes, LONG lInitialCount, LONG lMaximumCount, LPCSTR lpName);
typedef HANDLE (WINAPI *CreateSemaphoreWT)(LPSECURITY_ATTRIBUTES lpSemaphoreAttributes, LONG lInitialCount, LONG lMaximumCount, LPCWSTR lpName);
typedef HANDLE (WINAPI *CreateSemaphoreExAT)(LPSECURITY_ATTRIBUTES lpSemaphoreAttributes, LONG lInitialCount, LONG lMaximumCount, LPCSTR lpName, DWORD dwFlags, DWORD dwDesiredAccess);
typedef HANDLE (WINAPI *CreateSemaphoreExWT)(LPSECURITY_ATTRIBUTES lpSemaphoreAttributes, LONG lInitialCount, LONG lMaximumCount, LPCWSTR lpName, DWORD dwFlags, DWORD dwDesiredAccess);
typedef HANDLE (WINAPI *OpenSemaphoreAT)(DWORD dwDesiredAccess, BOOL bInheritHandle, LPCSTR lpName);
typedef HANDLE (WINAPI *OpenSemaphoreWT)(DWORD dwDesiredAccess, BOOL bInheritHandle, LPCWSTR lpName);
typedef BOOL   (WINAPI *ReleaseSemaphoreT)(HANDLE hSemaphore, LONG lReleaseCount, LPLONG lpPreviousCount);

// file
typedef HANDLE (WINAPI *CreateFileAT)(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
typedef HANDLE (WINAPI *CreateFileWT)(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
typedef HANDLE (WINAPI *CreateFile2T)(LPCTSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, DWORD dwCreationDisposition, LPCREATEFILE2_EXTENDED_PARAMETERS pCreateExParams);
typedef BOOL   (WINAPI *WriteFileT)(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped);
typedef BOOL   (WINAPI *ReadFileT)(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped);
typedef DWORD  (WINAPI *SetFilePointerT)(HANDLE hFile, LONG lDistanceToMove, PLONG lpDistanceToMoveHigh, DWORD dwMoveMethod);
typedef BOOL   (WINAPI *SetFilePointerExT)(HANDLE hFile, LARGE_INTEGER liDistanceToMove, PLARGE_INTEGER lpNewFilePointer, DWORD dwMoveMethod);
typedef DWORD  (WINAPI *GetFileTypeT)(HANDLE hFile);
typedef DWORD  (WINAPI *GetFileSizeT)(HANDLE hFile, LPDWORD lpFileSizeHigh);
typedef BOOL   (WINAPI *GetFileSizeExT)(HANDLE hFile, PLARGE_INTEGER lpFileSize);

// time
typedef BOOL (WINAPI *QueryPerformanceFrequencyT)( LARGE_INTEGER *lpFrequency );
typedef BOOL (WINAPI *QueryPerformanceCounterT)( LARGE_INTEGER *lpPerformanceCount );
typedef DWORD (*timeGetTimeT)(void);

#endif // rpsFuncTypes_h
