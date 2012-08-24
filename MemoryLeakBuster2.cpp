// メモリリーク検出器。
// この .cpp をプロジェクトに含めるだけで有効になり、プログラム終了時にリーク領域の確保時のコールスタックをデバッグ出力に表示します。
// 
// CRT が呼ぶ HeapAlloc/Free を hook することで、new/delete も malloc 一族も全て捕捉します。
// CRT を介さないメモリ確保や、static link された CRT のメモリ確保は捕捉できません。
// 
// 現状 CRT の設定が /MD か /MDd の時しか対応していません。
// また、CRT は msvcr100.dll か msvcr100d.dll (VisualStudio2010) にしか対応していませんが、調べて g_vcrt_info に書き足せば他もたぶん動きます。
// (調べ方:
//  適当にデバッグを開始して逆アセンブルモードで malloc() の中を追い、HeapAlloc を呼んでる箇所を見ると、
//  __imp__HeapAlloc みたいな名前のポインタ変数を経由して関数を呼んでいるのが分かります。
//  __imp__HeapAlloc - CRT モジュールの開始アドレス が RVA_imp_HeapAlloc になります)
// 
// 
// 機能に制限があるけどより portable なバージョン:
// https://github.com/i-saint/scribble/blob/master/MemoryLeakBuster.cpp


// /MD or /MDd
#ifdef _DLL

#pragma warning(disable: 4073) // init_seg(lib) は普通は使っちゃダメ的な warning。正当な理由があるので黙らせる
#pragma warning(disable: 4996) // _s じゃない CRT 関数使うとでるやつ
#pragma init_seg(lib) // global オブジェクトの初期化の優先順位上げる
#pragma comment(lib, "imagehlp.lib")

#include <windows.h>
#include <imagehlp.h>
#include <string>
#include <map>
namespace stl = std;

// windows.h の悪さ対策
#ifdef max
#undef  max
#undef  min
#endif // max

namespace {


struct VCRT_INFO
{
    const char *ModuleName;
    size_t RVA_imp_HeapAlloc;
    size_t RVA_imp_HeapFree;
}
g_vcrt_info[] = {
    // import table の HeapAlloc/Free の相対アドレス
    // バージョンが変わったりするとおそらく無効になる (=クラッシュする) ので注意
#ifdef _WIN64
    { "msvcr100.dll",  0x91418, 0x91420 },
    { "msvcr100d.dll", 0x15f458, 0x15f438 },
#else
    { "msvcr100.dll" , 0x11f8, 0x11fc },
    { "msvcr100d.dll", 0x1210, 0x1200 },
#endif
};

const size_t MinimumAlignment = 16;
const size_t MaxCallstackDepth = 32;


typedef LPVOID (WINAPI *HeapAllocT)( HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes );
typedef BOOL (WINAPI *HeapFreeT)( HANDLE hHeap, DWORD dwFlags, LPVOID lpMem );

// 乗っ取り前の HeapAlloc/Free
HeapAllocT HeapAlloc_Orig = NULL;
HeapFreeT HeapFree_Orig = NULL;

// HeapAlloc/Free の hook 版
LPVOID WINAPI HeapAlloc_Hooked( HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes );
BOOL WINAPI HeapFree_Hooked( HANDLE hHeap, DWORD dwFlags, LPVOID lpMem );


template<size_t N>
inline int istsprintf(char (&buf)[N], const char *format, ...)
{
    va_list vl;
    va_start(vl, format);
    int r = _vsnprintf(buf, N, format, vl);
    va_end(vl);
    return r;
}

template<size_t N>
inline int istvsprintf(char (&buf)[N], const char *format, va_list vl)
{
    return _vsnprintf(buf, N, format, vl);
}

#define istPrint(...) DebugPrint(__FILE__, __LINE__, __VA_ARGS__)

static const int DPRINTF_MES_LENGTH  = 4096;
void DebugPrintV(const char* /*file*/, int /*line*/, const char* fmt, va_list vl)
{
    char buf[DPRINTF_MES_LENGTH];
    //istsprintf(buf, "%s:%d - ", file, line);
    //::OutputDebugStringA(buf);
    //WriteLogFile(buf);
    istvsprintf(buf, fmt, vl);
    ::OutputDebugStringA(buf);
}

void DebugPrint(const char* file, int line, const char* fmt, ...)
{
    va_list vl;
    va_start(vl, fmt);
    DebugPrintV(file, line, fmt, vl);
    va_end(vl);
}



bool InitializeDebugSymbol(HANDLE proc=::GetCurrentProcess())
{
    if(!::SymInitialize(proc, NULL, TRUE)) {
        return false;
    }
    ::SymSetOptions(SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES);

    return true;
}

void FinalizeDebugSymbol(HANDLE proc=::GetCurrentProcess())
{
    ::SymCleanup(proc);
}


int GetCallstack(void **callstack, int callstack_size, int skip_size)
{
    return CaptureStackBackTrace(skip_size, callstack_size, callstack, NULL);
}

stl::string AddressToSymbolName(void *address, HANDLE proc=::GetCurrentProcess())
{
#ifdef _WIN64
    typedef DWORD64 DWORDX;
    typedef PDWORD64 PDWORDX;
#else
    typedef DWORD DWORDX;
    typedef PDWORD PDWORDX;
#endif

    char buf[1024];
    HANDLE process = proc;
    IMAGEHLP_MODULE imageModule = { sizeof(IMAGEHLP_MODULE) };
    IMAGEHLP_LINE line ={sizeof(IMAGEHLP_LINE)};
    DWORDX dispSym = 0;
    DWORD dispLine = 0;

    char symbolBuffer[sizeof(IMAGEHLP_SYMBOL) + MAX_PATH] = {0};
    IMAGEHLP_SYMBOL * imageSymbol = (IMAGEHLP_SYMBOL*)symbolBuffer;
    imageSymbol->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL);
    imageSymbol->MaxNameLength = MAX_PATH;

    if(!::SymGetModuleInfo(process, (DWORDX)address, &imageModule)) {
        istsprintf(buf, "[0x%p]\n", address);
    }
    else if(!::SymGetSymFromAddr(process, (DWORDX)address, &dispSym, imageSymbol)) {
        istsprintf(buf, "%s + 0x%x [0x%p]\n", imageModule.ModuleName, ((size_t)address-(size_t)imageModule.BaseOfImage), address);
    }
    else if(!::SymGetLineFromAddr(process, (DWORDX)address, &dispLine, &line)) {
        istsprintf(buf, "%s!%s + 0x%x [0x%p]\n", imageModule.ModuleName, imageSymbol->Name, ((size_t)address-(size_t)imageSymbol->Address), address);
    }
    else {
        istsprintf(buf, "%s(%d): %s!%s + 0x%x [0x%p]\n", line.FileName, line.LineNumber,
            imageModule.ModuleName, imageSymbol->Name, ((size_t)address-(size_t)imageSymbol->Address), address);
    }
    return buf;
}

stl::string CallstackToSymbolNames(void **callstack, int callstack_size, int clamp_head=0, int clamp_tail=0, const char *indent="")
{
    stl::string tmp;
    int begin = stl::max<int>(0, clamp_head);
    int end = stl::max<int>(0, callstack_size-clamp_tail);
    for(int i=begin; i<end; ++i) {
        tmp += indent;
        tmp += AddressToSymbolName(callstack[i]);
    }
    return tmp;
}




template<class T>
class ScopedLock
{
public:
    ScopedLock(T &m) : m_mutex(m) { m_mutex.lock(); }

    template<class F>
    ScopedLock(T &m, const F &f) : m_mutex(m)
    {
        while(!m_mutex.tryLock()) { f(); }
    }

    ~ScopedLock() { m_mutex.unlock(); }

private:
    T &m_mutex;

    ScopedLock& operator=(const ScopedLock &);
};
class Mutex
{
public:
    typedef ScopedLock<Mutex> ScopedLock;
    typedef CRITICAL_SECTION Handle;

    Mutex()          { InitializeCriticalSection(&m_lockobj); }
    ~Mutex()         { DeleteCriticalSection(&m_lockobj); }
    void lock()      { EnterCriticalSection(&m_lockobj); }
    bool tryLock()   { return TryEnterCriticalSection(&m_lockobj)==TRUE; }
    void unlock()    { LeaveCriticalSection(&m_lockobj); }

    Handle getHandle() const { return m_lockobj; }

private:
    Handle m_lockobj;
};


// アロケーション情報を格納するコンテナのアロケータが new / delete を使うと永久再起するので、
// hook を通さないメモリ確保を行うアロケータを用意
template<typename T>
class OrigHeapAllocator {
public : 
    //    typedefs
    typedef T value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;

public : 
    //    convert an allocator<T> to allocator<U>
    template<typename U>
    struct rebind {
        typedef OrigHeapAllocator<U> other;
    };

public : 
    OrigHeapAllocator() {}
    OrigHeapAllocator(const OrigHeapAllocator&) {}
    template<typename U> OrigHeapAllocator(const OrigHeapAllocator<U>&) {}
    ~OrigHeapAllocator() {}

    pointer address(reference r) { return &r; }
    const_pointer address(const_reference r) { return &r; }

    pointer allocate(size_type cnt, const void *p=NULL) { p; return (pointer)HeapAlloc_Orig((HANDLE)_get_heap_handle(), 0, cnt * sizeof(T)); }
    void deallocate(pointer p, size_type) {  HeapFree_Orig((HANDLE)_get_heap_handle(), 0, p); }

    size_type max_size() const { return std::numeric_limits<size_type>::max() / sizeof(T); }

    void construct(pointer p, const T& t) { new(p) T(t); }
    void destroy(pointer p) { p; p->~T(); }

    bool operator==(OrigHeapAllocator const&) { return true; }
    bool operator!=(OrigHeapAllocator const& a) { return !operator==(a); }
};
template<class T, typename Alloc> inline bool operator==(const OrigHeapAllocator<T>& l, const OrigHeapAllocator<T>& r) { return (l.equals(r)); }
template<class T, typename Alloc> inline bool operator!=(const OrigHeapAllocator<T>& l, const OrigHeapAllocator<T>& r) { return (!(l == r)); }


// write protect がかかったメモリ領域を強引に書き換える
template<class T> inline void ForceWrite(T &dst, const T &src)
{
    DWORD old_flag;
    VirtualProtect(&dst, sizeof(T), PAGE_EXECUTE_READWRITE, &old_flag);
    dst = src;
    VirtualProtect(&dst, sizeof(T), old_flag, &old_flag);
}


// アロケート時の callstack を保持
struct AllocInfo
{
    void *stack[MaxCallstackDepth];
    int depth;
};


class MemoryLeakBuster
{
public:
    MemoryLeakBuster()
        : m_leakinfo(NULL)
        , m_mi(0)
        , m_enabled(true)
    {
        InitializeDebugSymbol();

        // import table の HeapAlloc/Free の位置を調べる時用
        //{
        //    void *t = malloc(1024);
        //    free(t);
        //}

        HeapAlloc_Orig = &HeapAlloc;
        HeapFree_Orig = &HeapFree;

        // CRT モジュールの中の import table の HeapAlloc/Free を塗り替えて hook を仕込む
        for(m_mi=0; m_mi<_countof(g_vcrt_info); ++m_mi) {
            VCRT_INFO &crti = g_vcrt_info[m_mi];
            size_t msvcr = (size_t)::GetModuleHandleA(crti.ModuleName);
            if(msvcr==0) { continue; }

            {
                void **__imp__HeapAlloc = (void**)(msvcr+crti.RVA_imp_HeapAlloc);
                HeapAlloc_Orig = (HeapAllocT)(*__imp__HeapAlloc);
                ForceWrite<void*>(*__imp__HeapAlloc, HeapAlloc_Hooked);
            }
            {
                void **__imp__HeapFree = (void**)(msvcr+crti.RVA_imp_HeapFree);
                HeapFree_Orig = (HeapFreeT)(*__imp__HeapFree);
                ForceWrite<void*>(*__imp__HeapFree, HeapFree_Hooked);
            }
            break;
        }

        m_leakinfo = new (HeapAlloc_Orig((HANDLE)_get_heap_handle(), 0, sizeof(DataTableT))) DataTableT();
    }

    ~MemoryLeakBuster()
    {
        Mutex::ScopedLock l(m_mutex);

        // hook を解除
        if(m_mi < _countof(g_vcrt_info)) {
            VCRT_INFO &crti = g_vcrt_info[m_mi];
            size_t msvcr = (size_t)::GetModuleHandleA(crti.ModuleName);
            if(msvcr!=0) {
                {
                    void **__imp__HeapAlloc = (void**)(msvcr+crti.RVA_imp_HeapAlloc);
                    ForceWrite<void*>(*__imp__HeapAlloc, HeapAlloc_Orig);
                }
                {
                    void **__imp__HeapFree = (void**)(msvcr+crti.RVA_imp_HeapFree);
                    ForceWrite<void*>(*__imp__HeapFree, HeapFree_Orig);
                }

            }
        }

        printLeakInfo();

        m_leakinfo->~DataTableT();
        HeapFree_Orig((HANDLE)_get_heap_handle(), 0, m_leakinfo);
        m_leakinfo = NULL;

        FinalizeDebugSymbol();
    }

    void enableLeakCheck(bool v) { m_enabled=v; }

    void addAllocationInfo(void *p)
    {
        if(!m_enabled) { return; }

        AllocInfo cs;
        cs.depth = GetCallstack(cs.stack, _countof(cs.stack), 3);
        {
            Mutex::ScopedLock l(m_mutex);
            if(m_leakinfo==NULL) { return; }
            (*m_leakinfo)[p] = cs;
        }
    }

    void eraseAllocationInfo(void *p)
    {
        Mutex::ScopedLock l(m_mutex);
        if(m_leakinfo==NULL) { return; }
        m_leakinfo->erase(p);
    }

    void printLeakInfo()
    {
        // 一部の CRT 関数は、確保したメモリをモジュール開放時にまとめて開放する仕様になっており、
        // ここに来た時点ではモジュールはまだ開放されていないため、リーク判定されてしまう。
        // そやつらを除外する必要がある。以下は該当関数群。 (たぶん他にもある)
        const char *ignore_list[] = {
            "!unlock",
            "!fopen",
            "!setlocale",
            "!gmtime32_s",
            "!_getmainargs",
        };

        Mutex::ScopedLock l(m_mutex);
        for(DataTableT::iterator li=m_leakinfo->begin(); li!=m_leakinfo->end(); ++li) {
            stl::string text = CallstackToSymbolNames(li->second.stack, li->second.depth);

            bool ignore = false;
            for(size_t ii=0; ii<_countof(ignore_list); ++ii) {
                if(text.find(ignore_list[ii])!=stl::string::npos) {
                    ignore = true;
                    break;
                }
            }
            if(ignore) { continue; }

            istPrint("memory leak: %p\n", li->first);
            istPrint(text.c_str());
            istPrint("\n");
        }
    }

private:
    typedef stl::map<void*, AllocInfo, stl::less<void*>, OrigHeapAllocator<stl::pair<const void*, AllocInfo> > > DataTableT;
    DataTableT *m_leakinfo;
    Mutex m_mutex;
    size_t m_mi;
    bool m_enabled;
};

// global 変数にすることで main 開始前に初期化、main 抜けた後に終了処理をさせる。
// entry point を乗っ取ってもっとスマートにやりたかったが、
// WinMainCRTStartup() は main を呼んだ後 exit() してしまい、main の後にリーク箇所を出力することができないため断念
MemoryLeakBuster g_memory_leak_buster;


LPVOID WINAPI HeapAlloc_Hooked( HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes )
{
    LPVOID p = HeapAlloc_Orig(hHeap, dwFlags, dwBytes);
    g_memory_leak_buster.addAllocationInfo(p);
    return p;
}

BOOL WINAPI HeapFree_Hooked( HANDLE hHeap, DWORD dwFlags, LPVOID lpMem )
{
    BOOL r = HeapFree_Orig(hHeap, dwFlags, lpMem);
    g_memory_leak_buster.eraseAllocationInfo(lpMem);
    return r;
}

} /// namespace
#endif // _DLL
// メモリリーク検出器。
// この .cpp をプロジェクトに含めるだけで有効になり、プログラム終了時にリーク領域の確保時のコールスタックをデバッグ出力に表示します。
// 
// CRT が呼ぶ HeapAlloc/Free を hook することで、new/delete も malloc 一族も全て捕捉します。
// CRT を介さないメモリ確保や、static link された CRT のメモリ確保は捕捉できません。
// 
// 現状 CRT の設定が /MD か /MDd の時しか対応していません。
// また、CRT は msvcr100.dll か msvcr100d.dll (VisualStudio2010) にしか対応していませんが、調べて g_vcrt_info に書き足せば他もたぶん動きます。
// (調べ方:
//  適当にデバッグを開始して逆アセンブルモードで malloc() の中を追い、HeapAlloc を呼んでる箇所を見ると、
//  __imp__HeapAlloc みたいな名前のポインタ変数を経由して関数を呼んでいるのが分かります。
//  __imp__HeapAlloc - CRT モジュールの開始アドレス が RVA_imp_HeapAlloc になります)


// /MD or /MDd
#ifdef _DLL

#pragma warning(disable: 4073) // init_seg(lib) は普通は使っちゃダメ的な warning。正当な理由があるので黙らせる
#pragma warning(disable: 4996) // _s じゃない CRT 関数使うとでるやつ
#pragma init_seg(lib) // global オブジェクトの初期化の優先順位上げる
#pragma comment(lib, "imagehlp.lib")

#include <windows.h>
#include <imagehlp.h>
#include <string>
#include <map>
namespace stl = std;

// windows.h の悪さ対策
#ifdef max
#undef  max
#undef  min
#endif // max

namespace {


struct VCRT_INFO
{
    const char *ModuleName;
    size_t RVA_imp_HeapAlloc;
    size_t RVA_imp_HeapFree;
}
g_vcrt_info[] = {
    // import table の HeapAlloc/Free の相対アドレス
    // バージョンが変わったりするとおそらく無効になる (=クラッシュする) ので注意
#ifdef _WIN64
    { "msvcr100.dll",  0x91418, 0x91420 },
    { "msvcr100d.dll", 0x15f458, 0x15f438 },
#else
    { "msvcr100.dll" , 0x11f8, 0x11fc },
    { "msvcr100d.dll", 0x1210, 0x1200 },
#endif
};

const size_t MinimumAlignment = 16;
const size_t MaxCallstackDepth = 32;


typedef LPVOID (WINAPI *HeapAllocT)( HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes );
typedef BOOL (WINAPI *HeapFreeT)( HANDLE hHeap, DWORD dwFlags, LPVOID lpMem );

// 乗っ取り前の HeapAlloc/Free
HeapAllocT HeapAlloc_Orig = NULL;
HeapFreeT HeapFree_Orig = NULL;

// HeapAlloc/Free の hook 版
LPVOID WINAPI HeapAlloc_Hooked( HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes );
BOOL WINAPI HeapFree_Hooked( HANDLE hHeap, DWORD dwFlags, LPVOID lpMem );


template<size_t N>
inline int istsprintf(char (&buf)[N], const char *format, ...)
{
    va_list vl;
    va_start(vl, format);
    int r = _vsnprintf(buf, N, format, vl);
    va_end(vl);
    return r;
}

template<size_t N>
inline int istvsprintf(char (&buf)[N], const char *format, va_list vl)
{
    return _vsnprintf(buf, N, format, vl);
}

#define istPrint(...) DebugPrint(__FILE__, __LINE__, __VA_ARGS__)

static const int DPRINTF_MES_LENGTH  = 4096;
void DebugPrintV(const char* /*file*/, int /*line*/, const char* fmt, va_list vl)
{
    char buf[DPRINTF_MES_LENGTH];
    //istsprintf(buf, "%s:%d - ", file, line);
    //::OutputDebugStringA(buf);
    //WriteLogFile(buf);
    istvsprintf(buf, fmt, vl);
    ::OutputDebugStringA(buf);
}

void DebugPrint(const char* file, int line, const char* fmt, ...)
{
    va_list vl;
    va_start(vl, fmt);
    DebugPrintV(file, line, fmt, vl);
    va_end(vl);
}



bool InitializeDebugSymbol(HANDLE proc=::GetCurrentProcess())
{
    if(!::SymInitialize(proc, NULL, TRUE)) {
        return false;
    }
    ::SymSetOptions(SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES);

    return true;
}

void FinalizeDebugSymbol(HANDLE proc=::GetCurrentProcess())
{
    ::SymCleanup(proc);
}


int GetCallstack(void **callstack, int callstack_size, int skip_size)
{
    return CaptureStackBackTrace(skip_size, callstack_size, callstack, NULL);
}

stl::string AddressToSymbolName(void *address, HANDLE proc=::GetCurrentProcess())
{
#ifdef _WIN64
    typedef DWORD64 DWORDX;
    typedef PDWORD64 PDWORDX;
#else
    typedef DWORD DWORDX;
    typedef PDWORD PDWORDX;
#endif

    char buf[1024];
    HANDLE process = proc;
    IMAGEHLP_MODULE imageModule = { sizeof(IMAGEHLP_MODULE) };
    IMAGEHLP_LINE line ={sizeof(IMAGEHLP_LINE)};
    DWORDX dispSym = 0;
    DWORD dispLine = 0;

    char symbolBuffer[sizeof(IMAGEHLP_SYMBOL) + MAX_PATH] = {0};
    IMAGEHLP_SYMBOL * imageSymbol = (IMAGEHLP_SYMBOL*)symbolBuffer;
    imageSymbol->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL);
    imageSymbol->MaxNameLength = MAX_PATH;

    if(!::SymGetModuleInfo(process, (DWORDX)address, &imageModule)) {
        istsprintf(buf, "[0x%p]\n", address);
    }
    else if(!::SymGetSymFromAddr(process, (DWORDX)address, &dispSym, imageSymbol)) {
        istsprintf(buf, "%s + 0x%x [0x%p]\n", imageModule.ModuleName, ((size_t)address-(size_t)imageModule.BaseOfImage), address);
    }
    else if(!::SymGetLineFromAddr(process, (DWORDX)address, &dispLine, &line)) {
        istsprintf(buf, "%s!%s + 0x%x [0x%p]\n", imageModule.ModuleName, imageSymbol->Name, ((size_t)address-(size_t)imageSymbol->Address), address);
    }
    else {
        istsprintf(buf, "%s(%d): %s!%s + 0x%x [0x%p]\n", line.FileName, line.LineNumber,
            imageModule.ModuleName, imageSymbol->Name, ((size_t)address-(size_t)imageSymbol->Address), address);
    }
    return buf;
}

stl::string CallstackToSymbolNames(void **callstack, int callstack_size, int clamp_head=0, int clamp_tail=0, const char *indent="")
{
    stl::string tmp;
    int begin = stl::max<int>(0, clamp_head);
    int end = stl::max<int>(0, callstack_size-clamp_tail);
    for(int i=begin; i<end; ++i) {
        tmp += indent;
        tmp += AddressToSymbolName(callstack[i]);
    }
    return tmp;
}




template<class T>
class ScopedLock
{
public:
    ScopedLock(T &m) : m_mutex(m) { m_mutex.lock(); }

    template<class F>
    ScopedLock(T &m, const F &f) : m_mutex(m)
    {
        while(!m_mutex.tryLock()) { f(); }
    }

    ~ScopedLock() { m_mutex.unlock(); }

private:
    T &m_mutex;

    ScopedLock& operator=(const ScopedLock &);
};
class Mutex
{
public:
    typedef ScopedLock<Mutex> ScopedLock;
    typedef CRITICAL_SECTION Handle;

    Mutex()          { InitializeCriticalSection(&m_lockobj); }
    ~Mutex()         { DeleteCriticalSection(&m_lockobj); }
    void lock()      { EnterCriticalSection(&m_lockobj); }
    bool tryLock()   { return TryEnterCriticalSection(&m_lockobj)==TRUE; }
    void unlock()    { LeaveCriticalSection(&m_lockobj); }

    Handle getHandle() const { return m_lockobj; }

private:
    Handle m_lockobj;
};


// アロケーション情報を格納するコンテナのアロケータが new / delete を使うと永久再起するので、
// hook を通さないメモリ確保を行うアロケータを用意
template<typename T>
class OrigHeapAllocator {
public : 
    //    typedefs
    typedef T value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;

public : 
    //    convert an allocator<T> to allocator<U>
    template<typename U>
    struct rebind {
        typedef OrigHeapAllocator<U> other;
    };

public : 
    OrigHeapAllocator() {}
    OrigHeapAllocator(const OrigHeapAllocator&) {}
    template<typename U> OrigHeapAllocator(const OrigHeapAllocator<U>&) {}
    ~OrigHeapAllocator() {}

    pointer address(reference r) { return &r; }
    const_pointer address(const_reference r) { return &r; }

    pointer allocate(size_type cnt, const void *p=NULL) { p; return (pointer)HeapAlloc_Orig((HANDLE)_get_heap_handle(), 0, cnt * sizeof(T)); }
    void deallocate(pointer p, size_type) {  HeapFree_Orig((HANDLE)_get_heap_handle(), 0, p); }

    size_type max_size() const { return std::numeric_limits<size_type>::max() / sizeof(T); }

    void construct(pointer p, const T& t) { new(p) T(t); }
    void destroy(pointer p) { p; p->~T(); }

    bool operator==(OrigHeapAllocator const&) { return true; }
    bool operator!=(OrigHeapAllocator const& a) { return !operator==(a); }
};
template<class T, typename Alloc> inline bool operator==(const OrigHeapAllocator<T>& l, const OrigHeapAllocator<T>& r) { return (l.equals(r)); }
template<class T, typename Alloc> inline bool operator!=(const OrigHeapAllocator<T>& l, const OrigHeapAllocator<T>& r) { return (!(l == r)); }


// write protect がかかったメモリ領域を強引に書き換える
template<class T> inline void ForceWrite(T &dst, const T &src)
{
    DWORD old_flag;
    VirtualProtect(&dst, sizeof(T), PAGE_EXECUTE_READWRITE, &old_flag);
    dst = src;
    VirtualProtect(&dst, sizeof(T), old_flag, &old_flag);
}


// アロケート時の callstack を保持
struct AllocInfo
{
    void *stack[MaxCallstackDepth];
    int depth;
};


class MemoryLeakBuster
{
public:
    MemoryLeakBuster()
        : m_leakinfo(NULL)
        , m_mi(0)
        , m_enabled(true)
    {
        InitializeDebugSymbol();

        // import table の HeapAlloc/Free の位置を調べる時用
        //{
        //    void *t = malloc(1024);
        //    free(t);
        //}

        HeapAlloc_Orig = &HeapAlloc;
        HeapFree_Orig = &HeapFree;

        // CRT モジュールの中の import table の HeapAlloc/Free を塗り替えて hook を仕込む
        for(m_mi=0; m_mi<_countof(g_vcrt_info); ++m_mi) {
            VCRT_INFO &crti = g_vcrt_info[m_mi];
            size_t msvcr = (size_t)::GetModuleHandleA(crti.ModuleName);
            if(msvcr==0) { continue; }

            {
                void **__imp__HeapAlloc = (void**)(msvcr+crti.RVA_imp_HeapAlloc);
                HeapAlloc_Orig = (HeapAllocT)(*__imp__HeapAlloc);
                ForceWrite<void*>(*__imp__HeapAlloc, HeapAlloc_Hooked);
            }
            {
                void **__imp__HeapFree = (void**)(msvcr+crti.RVA_imp_HeapFree);
                HeapFree_Orig = (HeapFreeT)(*__imp__HeapFree);
                ForceWrite<void*>(*__imp__HeapFree, HeapFree_Hooked);
            }
            break;
        }

        m_leakinfo = new (HeapAlloc_Orig((HANDLE)_get_heap_handle(), 0, sizeof(DataTableT))) DataTableT();
    }

    ~MemoryLeakBuster()
    {
        Mutex::ScopedLock l(m_mutex);

        // hook を解除
        if(m_mi < _countof(g_vcrt_info)) {
            VCRT_INFO &crti = g_vcrt_info[m_mi];
            size_t msvcr = (size_t)::GetModuleHandleA(crti.ModuleName);
            if(msvcr!=0) {
                {
                    void **__imp__HeapAlloc = (void**)(msvcr+crti.RVA_imp_HeapAlloc);
                    ForceWrite<void*>(*__imp__HeapAlloc, HeapAlloc_Orig);
                }
                {
                    void **__imp__HeapFree = (void**)(msvcr+crti.RVA_imp_HeapFree);
                    ForceWrite<void*>(*__imp__HeapFree, HeapFree_Orig);
                }

            }
        }

        printLeakInfo();

        m_leakinfo->~DataTableT();
        HeapFree_Orig((HANDLE)_get_heap_handle(), 0, m_leakinfo);
        m_leakinfo = NULL;

        FinalizeDebugSymbol();
    }

    void enableLeakCheck(bool v) { m_enabled=v; }

    void addAllocationInfo(void *p)
    {
        if(!m_enabled) { return; }

        AllocInfo cs;
        cs.depth = GetCallstack(cs.stack, _countof(cs.stack), 3);
        {
            Mutex::ScopedLock l(m_mutex);
            if(m_leakinfo==NULL) { return; }
            (*m_leakinfo)[p] = cs;
        }
    }

    void eraseAllocationInfo(void *p)
    {
        Mutex::ScopedLock l(m_mutex);
        if(m_leakinfo==NULL) { return; }
        m_leakinfo->erase(p);
    }

    void printLeakInfo()
    {
        // 一部の CRT 関数は、確保したメモリをモジュール開放時にまとめて開放する仕様になっており、
        // ここに来た時点ではモジュールはまだ開放されていないため、リーク判定されてしまう。
        // そやつらを除外する必要がある。以下は該当関数群。 (たぶん他にもある)
        const char *ignore_list[] = {
            "!unlock",
            "!fopen",
            "!setlocale",
            "!gmtime32_s",
            "!_getmainargs",
        };

        Mutex::ScopedLock l(m_mutex);
        for(DataTableT::iterator li=m_leakinfo->begin(); li!=m_leakinfo->end(); ++li) {
            stl::string text = CallstackToSymbolNames(li->second.stack, li->second.depth);

            bool ignore = false;
            for(size_t ii=0; ii<_countof(ignore_list); ++ii) {
                if(text.find(ignore_list[ii])!=stl::string::npos) {
                    ignore = true;
                    break;
                }
            }
            if(ignore) { continue; }

            istPrint("memory leak: %p\n", li->first);
            istPrint(text.c_str());
            istPrint("\n");
        }
    }

private:
    typedef stl::map<void*, AllocInfo, stl::less<void*>, OrigHeapAllocator<stl::pair<const void*, AllocInfo> > > DataTableT;
    DataTableT *m_leakinfo;
    Mutex m_mutex;
    size_t m_mi;
    bool m_enabled;
};

// global 変数にすることで main 開始前に初期化、main 抜けた後に終了処理をさせる。
// entry point を乗っ取ってもっとスマートにやりたかったが、
// WinMainCRTStartup() は main を呼んだ後 exit() してしまい、main の後にリーク箇所を出力することができないため断念
MemoryLeakBuster g_memory_leak_buster;


LPVOID WINAPI HeapAlloc_Hooked( HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes )
{
    LPVOID p = HeapAlloc_Orig(hHeap, dwFlags, dwBytes);
    g_memory_leak_buster.addAllocationInfo(p);
    return p;
}

BOOL WINAPI HeapFree_Hooked( HANDLE hHeap, DWORD dwFlags, LPVOID lpMem )
{
    BOOL r = HeapFree_Orig(hHeap, dwFlags, lpMem);
    g_memory_leak_buster.eraseAllocationInfo(lpMem);
    return r;
}

} /// namespace
#endif // _DLL
