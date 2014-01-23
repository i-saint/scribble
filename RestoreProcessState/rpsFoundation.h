#ifndef rpsFoundation_h
#define rpsFoundation_h

void rpsInitializeFoundation();

// rpsMalloc() で確保されたメモリはトレースされない。(rpsLoadState() で復元されない)
// 内部実装用。
void* rpsMalloc(size_t size);
void rpsFree(void *addr);

bool rpsIsValidMemory(void *p);
BYTE* rpsAddJumpInstruction(BYTE* from, const BYTE* to);
void* rpsOverrideDLLExport(HMODULE module, const char *funcname, void *hook_, void *trampoline_space);


// rpsMalloc() で実装されたアロケータ。
template<typename T>
class rps_allocator
{
public:
    typedef T value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;

    template<typename U> struct rebind { typedef rps_allocator<U> other; };

public : 
    rps_allocator() {}
    rps_allocator(const rps_allocator&) {}
    template<typename U> rps_allocator(const rps_allocator<U>&) {}
    ~rps_allocator() {}

    pointer address(reference r) { return &r; }
    const_pointer address(const_reference r) { return &r; }

    pointer allocate(size_type cnt, const void *p=nullptr) { p; return (pointer)rpsMalloc(cnt * sizeof(T)); }
    void deallocate(pointer p, size_type) { rpsFree(p); }

    size_type max_size() const { return std::numeric_limits<size_type>::max() / sizeof(T); }

    void construct(pointer p, const T& t) { new(p) T(t); }
    void destroy(pointer p) { p; p->~T(); }

    bool operator==(rps_allocator const&) { return true; }
    bool operator!=(rps_allocator const& a) { return !operator==(a); }
};
template<class T, typename Alloc> inline bool operator==(const rps_allocator<T>& l, const rps_allocator<T>& r) { return (l.equals(r)); }
template<class T, typename Alloc> inline bool operator!=(const rps_allocator<T>& l, const rps_allocator<T>& r) { return (!(l == r)); }


template<class T>
class rpsScopedLock
{
public:
    rpsScopedLock(T &m) : m_lockobj(m) { m_lockobj.lock(); }
    ~rpsScopedLock() { m_lockobj.unlock(); }
private:
    T &m_lockobj;
};

class rpsMutex
{
public:
    typedef rpsScopedLock<rpsMutex> ScopedLock;
    typedef CRITICAL_SECTION Handle;

    rpsMutex();
    ~rpsMutex();
    void lock();
    bool tryLock();
    void unlock();

private:
    Handle m_lockobj;
    rpsMutex(const rpsMutex&);
    rpsMutex& operator=(const rpsMutex&);
};


class rpsArchive
{
public:
    enum Mode
    {
        Writer,
        Reader,
    };

    rpsArchive();
    ~rpsArchive();
    void read(void *dst, size_t size);
    void write(const void *data, size_t size);
    void io(void *dst, size_t size);
    bool open(const char *path_to_file, Mode mode);
    void close();
    bool isReader() const { return m_mode==Reader; }
    bool isWriter() const { return m_mode==Writer; }

private:
    HANDLE m_file;
    Mode m_mode;
};


#endif // rpsFoundation_h
