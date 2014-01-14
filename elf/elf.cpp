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
    void *addr;
    const char *name;
};

struct dpSymbol
{
    const char  *name;
    void        *addr;
};
inline bool operator <(const dpSymbol &a, const dpSymbol &b) { return std::strcmp(a.name, b.name)<0; }
inline bool operator==(const dpSymbol &a, const dpSymbol &b) { return std::strcmp(a.name, b.name)==0; }


bool    dpMapFile(const char *path, void *&o_data, size_t &o_size, const std::function<void* (size_t)> &alloc);
void*   dpAllocateRWX(size_t size);
void    dpDeallocate(void *ptr);
bool    dpMakeRWX(void *addr, size_t size);
size_t  dpGetPageSize();

void*   dpGetMainModule();
void    dpEachModules(const std::function<void (const dpModuleInfo&)> &f);
bool    dpElfEachSymbols(void *_elf_data, const std::function<void (const dpSymbol &)> &f);


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
    typedef std::set<dpSymbol> SymbolCont;
    SymbolCont m_symbols;
    void *m_elf_file;
    bool m_needs_deallocate;
};

class dpSymbolManager
{
public:
    static dpSymbolManager* getInstance();
    const dpSymbol* findSymbol(const char *name);

private:
    typedef std::set<dpSymbol> SymbolCont;
    dpSymbolManager();
    ~dpSymbolManager();
    void gatherSymbols();

    SymbolCont m_syms;
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
    if(!dpMapFile(path_to_elf, data, size, dpAllocateRWX)) {
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
                case R_X86_64_PC32:  // 
                case R_X86_64_PLT32: // fall through
                    {
                        uint32_t rel = (uint32_t)(r_data - (size_t)addr - 0x04);
                        *(uint32_t*)(addr) = (uint32_t)(base + rel);
                        break;
                    }
                case R_X86_64_32:
                    {
                        *(uint32_t*)(addr) = (uint32_t)(base + r_data);
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
    dpSymbol s = {name, nullptr};
    auto it = m_symbols.find(s);
    if(it!=m_symbols.end()) {
        return &(*it);
    }
    return nullptr;
}


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
}

void dpSymbolManager::gatherSymbols()
{
#ifdef dpWindows
#else  // dpWindows

    dpEachModules([&](const dpModuleInfo &mod){
        printf("module [%p] %s\n", mod.addr, mod.name);
        dpElfEachSymbols(mod.addr, [&](const dpSymbol &sym){
            printf("  [%p] %s\n", sym.addr, sym.name);
            m_syms.insert(sym);
        });
    });
    printf("%u symbols\n", (uint32_t)m_syms.size());

#endif // dpWindows
}

const dpSymbol* dpSymbolManager::findSymbol(const char *name)
{
    dpSymbol sym = {name, nullptr};
    auto it = m_syms.find(sym);
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

void* dpAllocateRWX(size_t size)
{
#ifdef dpWindows
    return ::VirtualAlloc(nullptr, size, MEM_COMMIT|MEM_RESERVE, PAGE_EXECUTE_READWRITE);
#else  // dpWindows
    size_t pos = (size_t)dpGetMainModule();
    size_t page_size = dpGetPageSize();
    for(int i=0; ; ++i) {
        void *r = ::mmap((void*)(pos-page_size*i), size, PROT_EXEC|PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
        printf("dpAllocateRWX(%u): %p\n", (uint32_t)size, r);
        if(r) { return r; }
    }
#endif // dpWindows
}

void dpDeallocate(void *ptr)
{
#ifdef dpWindows
    ::VirtualFree(ptr, 0, MEM_RELEASE);
#else  // dpWindows
    ::munmap(ptr, 0);
#endif // dpWindows
}

void* dpGetMainModule()
{
#ifdef dpWindows
    return ::GetModuleHandleA(nullptr);
#else  // dpWindows
    link_map *modules = (link_map*)dlopen(nullptr, RTLD_NOW);
    return (void*)modules->l_addr;
#endif // dpWindows
}

void dpEachModules(const std::function<void (const dpModuleInfo&)> &f)
{
#ifdef dpWindows
#else  // dpWindows
    link_map *modules = (link_map*)dlopen(nullptr, RTLD_NOW);
    while(modules) {
        dpModuleInfo mod = {(void*)modules->l_addr, (const char*)modules->l_name};
        f(mod);
        modules = (link_map*)modules->l_next;
    }
#endif // dpWindows
}

bool dpMakeRWX(void *addr, size_t size)
{
#ifdef dpWindows
#else  // dpWindows
    return mprotect(addr, size, PROT_READ|PROT_WRITE|PROT_EXEC)==0;
#endif // dpWindows
}

size_t dpGetPageSize()
{
#ifdef dpWindows
#else  // dpWindows
    return getpagesize();
#endif // dpWindows
}

bool dpElfEachSymbols(void *_elf_data, const std::function<void (const dpSymbol &)> &f)
{
    if(!_elf_data) { return false; }
    char *elf_data = (char*)_elf_data;
    Elf_Ehdr *elf_header = (Elf_Ehdr*)elf_data;

    // 有効なモジュールかチェック & section header が読める状態かチェック
    // もうちょっとまともな判別方法ないもんか
    const size_t page_size = dpGetPageSize();
    if(!dpMakeRWX(elf_data, page_size)) {
        return false;
    }
    bool has_section_headers = true;
    size_t size = (elf_header->e_shoff + sizeof(Elf_Shdr)*elf_header->e_shnum);
    if(!dpMakeRWX(elf_data, (size+page_size-1) & ~page_size)) {
        has_section_headers = false;
    }

    Elf_Phdr *elf_pheader_dyn = nullptr;
    for(int i=0; i<elf_header->e_phnum; ++i) {
        Elf_Phdr *ph = (Elf_Phdr*)(elf_data + elf_header->e_phoff) + i;
        if(ph->p_type==PT_DYNAMIC) {
            elf_pheader_dyn = ph;
        }
    }

    if(has_section_headers) { // use section headers
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
                            sym_addr = elf_data + sym_section->sh_offset + symbol->st_value;
                        }
                        else {
                            sym_addr = elf_data + symbol->st_value;
                        }

                        dpSymbol sym = {sym_name, sym_addr};
                        f(sym);
                    }
                }
            }
        }
    }
    else if(elf_pheader_dyn) { // use program headers
        Elf_Dyn *dyn = (Elf_Dyn*)(elf_data + elf_pheader_dyn->p_vaddr);
        char *symtab = nullptr;
        char *strtab = nullptr;
        size_t syment = 0;
        for(; dyn->d_tag!=DT_NULL; ++dyn) {
            if(dyn->d_tag==DT_SYMTAB) {
                symtab = (char*)dyn->d_un.d_ptr;
            }
            else if(dyn->d_tag==DT_SYMENT) {
                syment = dyn->d_un.d_val;
            }
            else if(dyn->d_tag==DT_STRTAB) {
                strtab = (char*)dyn->d_un.d_ptr;
            }
        }
        if(symtab && strtab && syment) {
            // ↓この終了条件すっごく不安…。しかし symbol table の範囲の情報は section header にしか見当たらず
            for(int isym=0; symtab+isym<strtab; isym+=syment) {
                Elf_Sym *symbol = (Elf_Sym*)(symtab+isym);
                const char *sym_name = strtab + symbol->st_name;
                char *sym_addr = elf_data + symbol->st_value;
                if(symbol->st_name==0 || symbol->st_value==0 || symbol->st_shndx>=elf_header->e_shnum) { continue; }

                dpSymbol sym = {sym_name, sym_addr};
                f(sym);
            }
        }
    }
    return true;
}



void test_call(int a)
{
    printf("test_call(%d)\n", a);
}

int main(int argc, char *argv[])
{
    //if(const dpSymbol *sym = dpSymbolManager::getInstance()->findSymbol("malloc")) {
    //    typedef void* (*malloc_t)(size_t);
    //    malloc_t mf = (malloc_t)sym->addr;
    //    printf("malloc: %p %p\n", mf, &malloc);
    //    fflush(stdout);
    //    printf("mf(): %p\n", mf(16));
    //}

    test_call(42);

    dpElfFile elf;
    if(elf.loadFromFile("testobj.o") && elf.link()) {
        typedef int (*test_add_t)(int, int);
        typedef void (*test_call_t)(int);

        if(const dpSymbol *sym = elf.findSymbol("test_add")) {
            test_add_t test_add = (test_add_t)sym->addr;
            printf("add(1,2): %d\n", test_add(1,2));
        }
        if(const dpSymbol *sym = elf.findSymbol("test_call")) {
            test_call_t test_call = (test_call_t)sym->addr;
            test_call(42);
        }
    }
}

/*
g++ -c testobj.cpp
g++ elf.cpp -std=c++11 -fPIC -pie -ldl -Wl,--export-dynamic
./a.out

...

add(1,2): 3
test_call(42)
*/
