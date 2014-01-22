#ifndef rpsMalloc_h
#define rpsMalloc_h

void *rpsMalloc(size_t size);
void rpsFree(void *addr);

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

#endif // rpsMalloc_h
