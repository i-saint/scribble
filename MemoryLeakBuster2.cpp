// メモリリーク検出器。
// この .cpp をプロジェクトに含めるだけで有効になり、プログラム終了時にリーク領域の確保時のコールスタックをデバッグ出力に表示します。
// 
// CRT が呼ぶ HeapAlloc/Free を hook することで、new/delete も malloc 一族も、外部 dll のリークも捕捉できます。
// CRT を static link したモジュールの場合追加の手順が必要で、下の g_crtdllnames に対象モジュールを追加する必要があります。


#pragma warning(disable: 4073) // init_seg(lib) は普通は使っちゃダメ的な warning。正当な理由があるので黙らせる
#pragma warning(disable: 4996) // _s じゃない CRT 関数使うとでるやつ
#pragma init_seg(lib) // global オブジェクトの初期化の優先順位上げる
#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "psapi.lib")

#include <windows.h>
#include <dbghelp.h>
#include <psapi.h>
#include <string>
#include <vector>
#include <map>
namespace stl = std;

// windows.h の悪さ対策
#ifdef max
#undef  max
#undef  min
#endif // max

namespace {

// リークチェッカを仕掛ける対象となるモジュール名のリスト。(dll or exe)
// EnumProcessModules でロードされている全モジュールに仕掛けることもできるが、
// 色々誤判定されるので絞ったほうがいいと思われる。
// /MT や /MTd でビルドされたモジュールのリークチェックをしたい場合、このリストに対象モジュールを書けばいけるはず。
const char *g_crtdllnames[] = {
    "msvcr110.dll",
    "msvcr110d.dll",
    "msvcr100.dll",
    "msvcr100d.dll",
    "msvcr90.dll",
    "msvcr90d.dll",
    "msvcr80.dll",
    "msvcr80d.dll",
    "msvcrt.dll",
};

// 以下の関数群はリーク判定しないようにする。
// 一部の CRT 関数などは確保したメモリをモジュール開放時にまとめて開放する仕様になっており、
// リーク情報を出力する時点ではモジュールはまだ開放されていないため、リーク判定されてしまう。そういう関数を無視できるようにしている。
// (たぶん下記以外にもあるはず)
const char *g_ignore_list[] = {
    "!unlock",
    "!fopen",
    "!setlocale",
    "!gmtime32_s",
    "!_getmainargs",
    "!mbtowc_l",
    "!std::time_get",
    "!std::time_put",
};

// 保持する callstack の最大段数
const size_t MaxCallstackDepth = 32;


typedef LPVOID (WINAPI *HeapAllocT)( HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes );
typedef BOOL (WINAPI *HeapFreeT)( HANDLE hHeap, DWORD dwFlags, LPVOID lpMem );

// 乗っ取り前の HeapAlloc/Free
HeapAllocT HeapAlloc_Orig = NULL;
HeapFreeT HeapFree_Orig = NULL;

// 乗っ取り後の HeapAlloc/Free
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



// アロケート時の callstack を保持
struct AllocInfo
{
    void *stack[MaxCallstackDepth];
    int depth;
};


// write protect がかかったメモリ領域を強引に書き換える
template<class T> inline void ForceWrite(T &dst, const T &src)
{
    DWORD old_flag;
    VirtualProtect(&dst, sizeof(T), PAGE_EXECUTE_READWRITE, &old_flag);
    dst = src;
    VirtualProtect(&dst, sizeof(T), old_flag, &old_flag);
}


// dllname: 大文字小文字区別しません
// F: functor。引数は (const char *funcname, void *&imp_func)
template<class F>
bool EachImportFunction(HMODULE module, const char *dllname, const F &f)
{
    if(module==0) { return false; }

    size_t ImageBase = (size_t)module;
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)ImageBase;
    if(pDosHeader->e_magic!=IMAGE_DOS_SIGNATURE) { return false; }
    PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)(ImageBase + pDosHeader->e_lfanew);

    size_t RVAImports = pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
    if(RVAImports==0) { return false; }

    IMAGE_IMPORT_DESCRIPTOR *pImportDesc = (IMAGE_IMPORT_DESCRIPTOR*)(ImageBase + RVAImports);
    while(pImportDesc->Name != 0) {
        if(stricmp((const char*)(ImageBase+pImportDesc->Name), dllname)==0) {
            IMAGE_IMPORT_BY_NAME **func_names = (IMAGE_IMPORT_BY_NAME**)(ImageBase+pImportDesc->Characteristics);
            void **import_table = (void**)(ImageBase+pImportDesc->FirstThunk);
            for(size_t i=0; ; ++i) {
                if((size_t)func_names[i] == 0) { break;}
                const char *funcname = (const char*)(ImageBase+(size_t)func_names[i]->Name);
                f(funcname, import_table[i]);
            }
        }
        ++pImportDesc;
    }
    return true;
}

template<class F>
void EachImportFunctionInEveryModule(const char *dllname, const F &f)
{
    stl::vector<HMODULE> modules;
    DWORD num_modules;
    ::EnumProcessModules(::GetCurrentProcess(), NULL, 0, &num_modules);
    modules.resize(num_modules/sizeof(HMODULE));
    ::EnumProcessModules(::GetCurrentProcess(), &modules[0], num_modules, &num_modules);
    for(size_t i=0; i<modules.size(); ++i) {
        EachImportFunction<F>(modules[i], dllname, f);
    }
}


void HookHeapAlloc()
{
    for(size_t i=0; i<_countof(g_crtdllnames); ++i) {
        EachImportFunction(::GetModuleHandleA(g_crtdllnames[i]), "kernel32.dll", [](const char *funcname, void *&imp_func){
            if(strcmp(funcname, "HeapAlloc")==0) {
                ForceWrite<void*>(imp_func, HeapAlloc_Hooked);
            }
            else if(strcmp(funcname, "HeapFree")==0) {
                ForceWrite<void*>(imp_func, HeapFree_Hooked);
            }
        });
    }
}

void UnhookHeapAlloc()
{
    for(size_t i=0; i<_countof(g_crtdllnames); ++i) {
        EachImportFunction(::GetModuleHandleA(g_crtdllnames[i]), "kernel32.dll", [](const char *funcname, void *&imp_func){
            if(strcmp(funcname, "HeapAlloc")==0) {
                ForceWrite<void*>(imp_func, HeapAlloc_Orig);
            }
            else if(strcmp(funcname, "HeapFree")==0) {
                ForceWrite<void*>(imp_func, HeapFree_Orig);
            }
        });
    }
}

class MemoryLeakBuster
{
public:
    MemoryLeakBuster()
        : m_mutex(NULL)
        , m_leakinfo(NULL)
        , m_enabled(true)
    {
        InitializeDebugSymbol();

        HeapAlloc_Orig = &HeapAlloc;
        HeapFree_Orig = &HeapFree;

        // CRT モジュールの中の import table の HeapAlloc/Free を塗り替えて hook を仕込む
        HookHeapAlloc();
        m_mutex = new (HeapAlloc_Orig((HANDLE)_get_heap_handle(), 0, sizeof(Mutex))) Mutex();
        m_leakinfo = new (HeapAlloc_Orig((HANDLE)_get_heap_handle(), 0, sizeof(DataTableT))) DataTableT();
    }

    ~MemoryLeakBuster()
    {
        Mutex::ScopedLock l(*m_mutex);

        printLeakInfo();

        // hook を解除
        // 解除しないとアンロード時にメモリ解放する系の dll などが g_memory_leak_buster 破棄後に
        // eraseAllocationInfo() を呼ぶため、問題が起きる
        UnhookHeapAlloc();

        m_leakinfo->~DataTableT();
        HeapFree_Orig((HANDLE)_get_heap_handle(), 0, m_leakinfo);
        m_leakinfo = NULL;

        // m_mutex は開放しません
        // 別スレッドから HeapFree_Hooked() が呼ばれて mutex を待ってる間に
        // ここでその mutex を破棄してしまうとクラッシュしてしまうためです。

        FinalizeDebugSymbol();
    }

    void enableLeakCheck(bool v) { m_enabled=v; }

    void addAllocationInfo(void *p)
    {
        if(!m_enabled) { return; }

        AllocInfo cs;
        cs.depth = GetCallstack(cs.stack, _countof(cs.stack), 3);
        {
            Mutex::ScopedLock l(*m_mutex);
            if(m_leakinfo==NULL) { return; }
            (*m_leakinfo)[p] = cs;
        }
    }

    void eraseAllocationInfo(void *p)
    {
        Mutex::ScopedLock l(*m_mutex);
        if(m_leakinfo==NULL) { return; }
        m_leakinfo->erase(p);
    }

    void printLeakInfo()
    {
        if(m_leakinfo==NULL) { return; }
        for(DataTableT::iterator li=m_leakinfo->begin(); li!=m_leakinfo->end(); ++li) {
            stl::string text = CallstackToSymbolNames(li->second.stack, li->second.depth);

            bool ignore = false;
            for(size_t ii=0; ii<_countof(g_ignore_list); ++ii) {
                if(text.find(g_ignore_list[ii])!=stl::string::npos) {
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
    Mutex *m_mutex;
    DataTableT *m_leakinfo;
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
