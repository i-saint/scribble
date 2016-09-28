#include <set>
#include <cstdio>
#include <cstdlib>


int g_count = 0;

void* operator new(std::size_t s) throw(std::bad_alloc)
{
    ++g_count;
    return malloc(s);
}

void operator delete(void *addr) throw()
{
    free(addr);
}

int main()
{
    std::set<double> test;
    printf("count after construction: %d\n", g_count);
    test.insert(0.0);
    printf("count after insertion: %d\n", g_count);
}


// $ g++ -v
// gcc version 4.8.4 (Ubuntu 4.8.4 - 2ubuntu1~14.04.3)
// $ g++ test_set.cpp && . / a.out
// count after construction : 0
// count after insertion : 1

// $ g++ -v
// Apple LLVM version 7.3.0 (clang-703.0.31)
// $ g++ test_set.cpp && . / a.out
// count after construction : 0
// count after insertion : 1

// cl test_set.cpp && test_set
// count after construction : 1
// count after insertion : 2
