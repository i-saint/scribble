#include <cstdio>
#include <cstdint>
#include <cstring>
#include <functional>
#include <algorithm>
#include <vector>
#include <map>
#include <set>

#ifdef _WIN32
#   include <windows.h>
#   include "elf_for_win.h"
#else  // _WIN32
#   include <sys/mman.h>
#   include <dlfcn.h>
#   include <link.h>
#   include <unistd.h>
#endif // _WIN32

#if defined(_M_X64) || defined(_LP64)
#   define dpX64
#endif
#if defined(_WIN32)
#   define dpWindows
#else
#   define dpPOSIX
#endif

inline Elf32_Word ElfRela_GetSymbol(const Elf32_Rela &v)  { return (v.r_info >> 8); }
inline unsigned char ElfRela_GetType(const Elf32_Rela &v) { return (unsigned char) (v.r_info & 0x0ff); }
inline Elf64_Word ElfRela_GetSymbol(const Elf64_Rela &v)  { return (v.r_info >> 32); }
inline Elf64_Word ElfRela_GetType(const Elf64_Rela &v)    { return (Elf64_Word) (v.r_info & 0xffffffffL); }

#ifdef dpX64
typedef Elf64_Ehdr Elf_Ehdr;
typedef Elf64_Phdr Elf_Phdr;
typedef Elf64_Shdr Elf_Shdr;
typedef Elf64_Sym  Elf_Sym;
typedef Elf64_Rel  Elf_Rel;
typedef Elf64_Rela Elf_Rela;
typedef Elf64_Dyn  Elf_Dyn;
#else  // dpX64
typedef Elf32_Ehdr Elf_Ehdr;
typedef Elf32_Phdr Elf_Phdr;
typedef Elf32_Shdr Elf_Shdr;
typedef Elf32_Sym  Elf_Sym;
typedef Elf32_Rel  Elf_Rel;
typedef Elf32_Rela Elf_Rela;
typedef Elf32_Dyn  Elf_Dyn;
#endif // dpX64

struct dpModuleInfo
{
    const char *name;
    void *addr;
#ifdef dpPOSIX
    void *addr2; // symbol table のために元ファイルを再度メモリにマップしたモジュール
#endif // dpPOSIX
};

enum dpSymbolFlags {
    dpSF_None	= 0x0000,
    dpSF_PIC	= 0x0001,
};
struct dpSymbol
{
    const char  *name;
    void        *addr;
    int			flags;
};
inline bool operator <(const dpSymbol &a, const dpSymbol &b) { return std::strcmp(a.name, b.name)<0; }
inline bool operator==(const dpSymbol &a, const dpSymbol &b) { return std::strcmp(a.name, b.name)==0; }


bool    dpMapFile(const char *path, void *&o_data, size_t &o_size, const std::function<void* (size_t)> &alloc);
void*   dpAllocate(size_t size, void *addr=nullptr);
void*   dpAllocateForward(size_t size, void *addr);
void*   dpAllocateBackward(size_t size, void *addr);
void*   dpAllocateModule(size_t size);
void    dpDeallocate(void *ptr);
bool    dpMakeRWX(void *addr, size_t size);
size_t  dpGetPageSize();

void*   dpGetMainModule();
void    dpEachModules(const std::function<void (const dpModuleInfo&)> &f);
bool    dpElfEachSymbols(void *elf_base, const std::function<void (const dpSymbol &)> &f);
#ifdef dpPOSIX
bool	dpReloadModuleAndEnumerateSymbols(dpModuleInfo &mod, const std::function<void (const dpSymbol &)> &f);
#endif // dpPOSIX


class dpElfFile
{
public:
    dpElfFile();
    ~dpElfFile();
    bool loadFromFile(const char *path_to_elf);
    bool loadFromMemory(void *data);
    void unload();
    bool link();
    const dpSymbol* findSymbol(const char *name);

private:
    typedef std::set<dpSymbol> Symbols;
    typedef std::vector<char*> Pages;
    const dpSymbol* findOrCreateTrampoline(const char *name);

    Symbols m_symbols;
    void *m_elf_file;
    bool m_needs_deallocate;
#ifdef dpX64
    Symbols m_plt_syms;
    Pages m_plt_pages;
#endif // dpX64
};

class dpSymbolManager
{
public:
    static dpSymbolManager* getInstance();
    const dpSymbol* findSymbol(const char *name);

private:
    typedef std::set<dpSymbol> Symbols;
    typedef std::vector<dpModuleInfo> Modules;

    dpSymbolManager();
    ~dpSymbolManager();
    void clear();
    void gatherSymbols();

    Symbols m_syms;
    Modules m_mods;
};



dpElfFile::dpElfFile()
    : m_elf_file(nullptr)
    , m_needs_deallocate(false)
{
    dpSymbolManager::getInstance();
}

dpElfFile::~dpElfFile()
{
    unload();
}

bool dpElfFile::loadFromFile( const char *path_to_elf )
{
    unload();

    size_t size = 0;
    void *data = nullptr;
    if(!dpMapFile(path_to_elf, data, size,
        [](size_t s){ return dpAllocate(s,nullptr); } ))
    {
        return false;
    }

    printf("ElfFile::loadFromFile(): loading %s...\n", path_to_elf);
    if(loadFromMemory(data)) {
        m_needs_deallocate = true;
        printf("ElfFile::loadFromFile(): loading %s succeeded.\n", path_to_elf);
        return true;
    }
    else {
        dpDeallocate(data);
        printf("ElfFile::loadFromFile(): loading %s failed.\n", path_to_elf);
        return false;
    }
}

bool dpElfFile::loadFromMemory(void *data)
{
    char *elf_data = (char*)data;
    Elf_Ehdr *elf_header = (Elf_Ehdr*)elf_data;
    if(memcmp(elf_header->e_ident, "\x7F" "ELF", 4)!=0) {
        printf("ElfFile::loadFromMemory(): invalid elf file.\n");
        unload();
        return false;
    }
#ifdef dpX64
    if(elf_header->e_ident[EI_CLASS]!=ELFCLASS64) {
        printf("ElfFile::load() error: not 64 bit elf file.\n");
        unload();
        return false;
    }
#else  // dpX64
    if(elf_header->e_ident[EI_CLASS]!=ELFCLASS32) {
        printf("ElfFile::load() error: not 32 bit elf file.\n");
        unload();
        return false;
    }
#endif // dpX64

    dpElfEachSymbols(elf_data, [&](const dpSymbol &sym){
        printf("  [%p] %s\n", sym.addr, sym.name);
        m_symbols.insert(sym);
    });
    m_elf_file = data;
    return true;
}


void dpElfFile::unload()
{
    if(m_needs_deallocate && m_elf_file) {
        dpDeallocate(m_elf_file);
    }
    m_elf_file = nullptr;
    m_needs_deallocate = false;
    m_symbols.clear();

#ifdef dpX64
    std::for_each(m_plt_pages.begin(), m_plt_pages.end(), [](void *p){ dpDeallocate(p); });
    m_plt_pages.clear();
    m_plt_syms.clear();
#endif // dpX64
}

bool dpElfFile::link()
{
    if(!m_elf_file) { return false; }

    char *elf_data = (char*)m_elf_file;
    Elf_Ehdr *elf_header = (Elf_Ehdr*)elf_data;

    Elf_Shdr *sections = (Elf_Shdr*)(elf_data + elf_header->e_shoff);
    const char *section_str = (const char*)(elf_data + sections[elf_header->e_shstrndx].sh_offset);

    Elf_Shdr *section_prev = nullptr;
    for(int isec=0; isec<elf_header->e_shnum; ++isec) {
        Elf_Shdr *section = sections + isec;
        const char *section_name = section_str + section->sh_name;
        printf("section_name: %s\n", section_name);
        size_t section_base = (size_t)(elf_data + section->sh_offset);

        if(section->sh_type==SHT_REL) {
            for(int irel=0; irel<section->sh_size; irel+=section->sh_entsize) {
                Elf_Rel *rela = (Elf_Rel*)(section_base + irel);
            }
        }
        else if(section->sh_type==SHT_RELA) {
            Elf_Shdr *r_section = sections + section->sh_link;
            const char *sym_str = (const char*)(elf_data + sections[r_section->sh_link].sh_offset);
            for(int irel=0; irel<section->sh_size; irel+=section->sh_entsize) {
                Elf_Rela *rela = (Elf_Rela*)(section_base + irel);
                Elf_Sym *r_symbol = (Elf_Sym*)(elf_data + r_section->sh_offset + r_section->sh_entsize*ElfRela_GetSymbol(*rela));
                const char *r_name = sym_str + r_symbol->st_name;
                size_t r_data = 0;
                void *addr = (uint32_t*)(elf_data+section_prev->sh_offset + rela->r_offset);
                uint32_t base = *(uint32_t*)addr;
                if(r_symbol->st_shndx!=SHN_UNDEF) {
                    r_data = (size_t)(elf_data + sections[r_symbol->st_shndx].sh_offset + r_symbol->st_value);
                }
                else {
                    const dpSymbol *sym = dpSymbolManager::getInstance()->findSymbol(r_name);
                    if(!sym) {
                        printf("link error: %s not found\n", r_name);
                        return false;
                    }
                    r_data = (size_t)sym->addr;
                }

                switch(ElfRela_GetType(*rela)) {
#ifdef dpX64
                case R_X86_64_32:
                    {
                        *(uint32_t*)(addr) = (uint32_t)(base + r_data);
                        break;
                    }
                case R_X86_64_PC32:
                    {
                        uint32_t rel = (uint32_t)(r_data - (size_t)addr - 0x04);
                        *(uint32_t*)(addr) = (uint32_t)(base + rel);
                        break;
                    }
                case R_X86_64_PLT32:
                    {
                        if(r_data-(size_t)m_elf_file > 0x7fff0000) {
                            r_data = (size_t)findOrCreateTrampoline(r_name)->addr;
                        }
                        uint32_t rel = (uint32_t)(r_data - (size_t)addr - 0x04);
                        *(uint32_t*)(addr) = (uint32_t)(base + rel);
                        break;
                    }
                // todo

#else  // dpX64
                // todo
#endif // dpX64
                }
            }
        }
        section_prev = section;
    }

    return true;
}

const dpSymbol* dpElfFile::findSymbol(const char *name)
{
    dpSymbol s = {name, nullptr, 0};
    auto it = m_symbols.find(s);
    if(it!=m_symbols.end()) {
        return &(*it);
    }
    return nullptr;
}

#ifdef dpX64
const dpSymbol* dpElfFile::findOrCreateTrampoline( const char *name )
{
    dpSymbol psym = {name, nullptr, 0};
    auto it = m_plt_syms.find(psym);
    if(it!=m_plt_syms.end()) {
        return &(*it);
    }
    else {
        const dpSymbol *target = dpSymbolManager::getInstance()->findSymbol(name);
        if(target) {
            // トランポリン生成
            // ff 25 00 00 00 00 [8 byte target address]
            size_t page_size = dpGetPageSize();
            size_t page_index = (m_plt_syms.size() * 0x10) / page_size;
            size_t page_offset = (m_plt_syms.size() * 0x10) % page_size;
            while(m_plt_pages.size()<=page_index) {
                m_plt_pages.push_back((char*)dpAllocateBackward(page_size, m_elf_file));
            }

            char *trampoline = m_plt_pages[page_index] + page_offset;
            char *tp = trampoline;
            tp[0] = 0xff;
            tp[1] = 0x25;
            tp += 2;
            *((uint32_t*)tp) = (uint32_t)0;
            tp += 4;
            *((size_t*)tp) = (size_t)(target->addr);

            psym.addr = trampoline;
            m_plt_syms.insert(psym);
            it = m_plt_syms.find(psym);
            return &(*it);
        }
    }
    return nullptr;
}
#endif // dpX64


dpSymbolManager* dpSymbolManager::getInstance()
{
    static dpSymbolManager s_inst;
    return &s_inst;
}

dpSymbolManager::dpSymbolManager()
{
    gatherSymbols();
}

dpSymbolManager::~dpSymbolManager()
{
    clear();
}

void dpSymbolManager::clear()
{
#ifdef dpPOSIX
    std::for_each(m_mods.begin(), m_mods.end(), [](dpModuleInfo &mod){
        free(mod.addr2);
    });
#endif // dpPOSIX
    m_mods.clear();
    m_syms.clear();
}

void dpSymbolManager::gatherSymbols()
{
    clear();

#if defined(dpWindows)

#elif defined(dpPOSIX)
    dpEachModules([&](const dpModuleInfo &mod){
        printf("module [%p] %s\n", mod.addr, mod.name);
        dpModuleInfo tmp = mod;
        dpReloadModuleAndEnumerateSymbols(tmp, [&](const dpSymbol &sym){
            printf("  [%p] %s\n", sym.addr, sym.name);
            m_syms.insert(sym);
        });
        if(tmp.addr2) {
            m_mods.push_back(tmp);
        }
    });
    printf("%u symbols\n", (uint32_t)m_syms.size());
#endif // dpWindows
}

const dpSymbol* dpSymbolManager::findSymbol(const char *name)
{
    dpSymbol tmp = {name, nullptr, 0};
    auto it = m_syms.find(tmp);
    if(it!=m_syms.end()) {
        return &(*it);
    }
    return nullptr;
}


bool dpMapFile(const char *path, void *&o_data, size_t &o_size, const std::function<void* (size_t)> &alloc)
{
    o_data = nullptr;
    o_size = 0;
    if(FILE *f=fopen(path, "rb")) {
        fseek(f, 0, SEEK_END);
        o_size = ftell(f);
        if(o_size > 0) {
            o_data = alloc(o_size);
            fseek(f, 0, SEEK_SET);
            fread(o_data, 1, o_size, f);
        }
        fclose(f);
        return true;
    }
    return false;
}

void* dpAllocate(size_t size, void *addr)
{
#if defined(dpWindows)
    return ::VirtualAlloc(addr, size, MEM_COMMIT|MEM_RESERVE, PAGE_EXECUTE_READWRITE);
#elif defined(dpPOSIX)
    return ::mmap(addr, size, PROT_EXEC|PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
#endif
}

void* dpAllocateForward(size_t size, void *addr)
{
    size_t pos = (size_t)addr;
    size_t page_size = dpGetPageSize();
    for(int i=0; ; ++i) {
        void *r = dpAllocate(size, (void*)(pos + page_size*i));
        if(r) { return r; }
    }
    return nullptr;
}

void* dpAllocateBackward(size_t size, void *addr)
{
    size_t pos = (size_t)addr;
    size_t page_size = dpGetPageSize();
    for(int i=0; ; ++i) {
        void *r = dpAllocate(size, (void*)(pos - page_size*i));
        if(r) { return r; }
    }
    return nullptr;
}


void dpDeallocate(void *ptr)
{
#if defined(dpWindows)
    ::VirtualFree(ptr, 0, MEM_RELEASE);
#elif defined(dpPOSIX)
    ::munmap(ptr, 0);
#endif
}

#ifdef dpPOSIX
extern Elf_Ehdr __executable_start;
#endif // dpPOSIX

void* dpGetMainModule()
{
#if defined(dpWindows)
    return ::GetModuleHandleA(nullptr);
#elif defined(dpPOSIX)
    Dl_info info;
    dladdr(&__executable_start, &info);
    return (void*)info.dli_fbase;
#endif
}

void dpEachModules(const std::function<void (const dpModuleInfo&)> &f)
{
#if defined(dpWindows)
#elif defined(dpPOSIX)
    // main module
    Dl_info info;
    if(dladdr(&__executable_start, &info)) {
        dpModuleInfo mod = {(const char*)info.dli_fname, (void*)info.dli_fbase, nullptr};
        f(mod);
    }
    // other modules
    link_map *modules = (link_map*)dlopen(nullptr, RTLD_NOW);
    while(modules) {
        if(modules->l_name) {
            dpModuleInfo mod = {(const char*)modules->l_name, (void*)modules->l_addr, nullptr};
            f(mod);
        }
        modules = (link_map*)modules->l_next;
    }
#endif
}

bool dpMakeRWX(void *addr, size_t size)
{
#if defined(dpWindows)
#elif defined(dpPOSIX)
    return mprotect(addr, size, PROT_READ|PROT_WRITE|PROT_EXEC)==0;
#endif
}

size_t dpGetPageSize()
{
#if defined(dpWindows)
#elif defined(dpPOSIX)
    return getpagesize();
#endif
}

bool dpElfEachSymbols(void *_elf_data, const std::function<void (const dpSymbol &)> &f)
{
    if(!_elf_data) { return false; }
    char *elf_data = (char*)_elf_data;
    Elf_Ehdr *elf_header = (Elf_Ehdr*)elf_data;

    bool is_pic = elf_header->e_type==ET_DYN || elf_header->e_type==ET_REL;

    Elf_Shdr *sections = (Elf_Shdr*)(elf_data + elf_header->e_shoff);
    for(int isec=0; isec<elf_header->e_shnum; ++isec) {
        Elf_Shdr *section = sections + isec;

        if(section->sh_type==SHT_SYMTAB || section->sh_type==SHT_DYNSYM) {
            const char *sym_str = (const char*)(elf_data + sections[section->sh_link].sh_offset);
            for(int isym=0; isym<section->sh_size; isym+=section->sh_entsize) {
                Elf_Sym *symbol = (Elf_Sym*)(elf_data + section->sh_offset + isym);
                const char *sym_name = sym_str + symbol->st_name;
                if(symbol->st_name==0 || symbol->st_shndx>=elf_header->e_shnum) { continue; }

                Elf_Shdr *sym_section = sections + symbol->st_shndx;
                char *sym_addr = nullptr;
                if(symbol->st_shndx!=SHN_UNDEF) {
                    if(elf_header->e_phnum==0) {
                        sym_addr = (char*)sym_section->sh_offset + symbol->st_value;
                    }
                    else {
                        sym_addr = (char*)symbol->st_value;
                    }
                    if(is_pic) {
                        sym_addr += (size_t)elf_data;
                    }

                    dpSymbol sym = {sym_name, sym_addr, 0};
                    if(is_pic) { sym.flags|= dpSF_PIC; }
                    f(sym);
                }
            }
        }
    }
    return true;
}

#ifdef dpPOSIX
bool dpReloadModuleAndEnumerateSymbols(dpModuleInfo &mod, const std::function<void (const dpSymbol &)> &f)
{
    void *data;
    size_t size;
    if(!dpMapFile(mod.name, data, size, malloc)) {
        printf("dpLoadElfAndEnumerateSymbols(): loading %s failed\n", mod.name);
        return false;
    }
    mod.addr2 = data;
    dpElfEachSymbols(data, [&](const dpSymbol &sym){
        dpSymbol tmp = { sym.name, sym.addr, sym.flags };
        if(sym.flags & dpSF_PIC) {
            tmp.addr = (void*)((size_t)tmp.addr - (size_t)data + (size_t)mod.addr);
        };
        f(tmp);
    });
    return true;
}
#endif // dpPOSIX





int main(int argc, char *argv[])
{
    dpElfFile elf;
    if(elf.loadFromFile("testobj.o") && elf.link()) {
        typedef int (*test_add_t)(int, int);
        typedef void (*test_call_t)(int);

        if(const dpSymbol *sym = elf.findSymbol("test_add")) {
            test_add_t test_add = (test_add_t)sym->addr;
            printf("add(1,2): %d\n", test_add(1,2));
        }
        else {
            printf("test_add() not found\n");
        }

        if(const dpSymbol *sym = elf.findSymbol("test_call")) {
            test_call_t test_call = (test_call_t)sym->addr;
            test_call(42);
        }
        else {
            printf("test_call() not found\n");
        }
    }
}

/*
g++ -g -c -fPIC testobj.cpp
g++ -g -std=c++11 elf.cpp -ldl
./a.out

...

add(1,2): 3
test_call(42)
*/
