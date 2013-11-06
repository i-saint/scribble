#include <cstdio>
#include <cstdint>
#include <functional>
#include <algorithm>
#include <vector>

#ifdef _WIN32
#   include <windows.h>
#else  // _WIN32
#   include <sys/mman.h>
#endif // _WIN32

typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Word;
typedef int32_t  Elf32_Sword;
typedef uint64_t Elf32_Xword;
typedef int64_t  Elf32_Sxword;
typedef uint32_t Elf32_Addr;
typedef uint32_t Elf32_Off;
typedef uint16_t Elf32_Section;
typedef uint32_t Elf32_Symndx;
typedef unsigned char Elf32_Byte;

typedef uint16_t Elf64_Half;
typedef uint32_t Elf64_Word;
typedef int32_t  Elf64_Sword;
typedef uint64_t Elf64_Xword;
typedef int64_t  Elf64_Sxword;
typedef uint64_t Elf64_Addr;
typedef uint64_t Elf64_Off;
typedef uint16_t Elf64_Section;
typedef uint64_t Elf64_Symndx;
typedef unsigned char Elf64_Byte;

#define EI_NIDENT (16)


/* Special section indices.  */
#define SHN_UNDEF       0               /* Undefined section */
#define SHN_LORESERVE   0xff00          /* Start of reserved indices */
#define SHN_LOPROC      0xff00          /* Start of processor-specific */
#define SHN_HIPROC      0xff1f          /* End of processor-specific */
#define SHN_ABS         0xfff1          /* Associated symbol is absolute */
#define SHN_COMMON      0xfff2          /* Associated symbol is common */
#define SHN_HIRESERVE   0xffff          /* End of reserved indices */

/* Legal values for sh_type (section type).  */
#define SHT_NULL        0               /* Section header table entry unused */
#define SHT_PROGBITS    1               /* Program data */
#define SHT_SYMTAB      2               /* Symbol table */
#define SHT_STRTAB      3               /* String table */
#define SHT_RELA        4               /* Relocation entries with addends */
#define SHT_HASH        5               /* Symbol hash table */
#define SHT_DYNAMIC     6               /* Dynamic linking information */
#define SHT_NOTE        7               /* Notes */
#define SHT_NOBITS      8               /* Program space with no data (bss) */
#define SHT_REL         9               /* Relocation entries, no addends */
#define SHT_SHLIB       10              /* Reserved */
#define SHT_DYNSYM      11              /* Dynamic linker symbol table */
#define SHT_NUM         12              /* Number of defined types.  */
#define SHT_LOSUNW      0x6ffffffd      /* Sun-specific low bound.  */
#define SHT_GNU_verdef  0x6ffffffd      /* Version definition section.  */
#define SHT_GNU_verneed 0x6ffffffe      /* Version needs section.  */
#define SHT_GNU_versym  0x6fffffff      /* Version symbol table.  */
#define SHT_HISUNW      0x6fffffff      /* Sun-specific high bound.  */
#define SHT_LOPROC      0x70000000      /* Start of processor-specific */
#define SHT_HIPROC      0x7fffffff      /* End of processor-specific */
#define SHT_LOUSER      0x80000000      /* Start of application-specific */
#define SHT_HIUSER      0x8fffffff      /* End of application-specific */

/* Legal values for sh_flags (section flags).  */
#define SHF_WRITE       (1 << 0)        /* Writable */
#define SHF_ALLOC       (1 << 1)        /* Occupies memory during execution */
#define SHF_EXECINSTR   (1 << 2)        /* Executable */
#define SHF_MASKPROC    0xf0000000      /* Processor-specific */


typedef enum {
    CK_NULL       = 0,        /* Invalid, same as EK_NULL */
    CK_DEFAULT    = 0x30,     /* Default type of data for section */
    CK_ALIGN      = 0x31,     /* Alignment for described range */
    CK_INSTR      = 0x32,     /* Instructions */
    CK_DATA       = 0x33,     /* Non-address data */
    CK_SADDR_32   = 0x34,     /* Simple 32-bit addresses */
    CK_GADDR_32   = 0x35,     /* GP-relative 32-bit addresses */
    CK_CADDR_32   = 0x36,     /* Complex 32-bit addresses */
    CK_SADDR_64   = 0x37,     /* Simple 64-bit addresses */
    CK_GADDR_64   = 0x38,     /* GP-relative 64-bit addresses */
    CK_CADDR_64   = 0x39,     /* Complex 64-bit addresses */
    CK_NO_XFORM   = 0x3a,     /* No transformations allowed in this range */
    CK_NO_REORDER = 0x3b,     /* No reordering allowed in this range */
    CK_GP_GROUP   = 0x3c,     /* Text/data in range with length given by
                                 second argument references GP group given
                                 by first. */
    CK_STUBS      = 0x3d      /* Text in range is stub code. ULEB */
} Elf_MIPS_Content_Kind;


// elf header
typedef struct
{
    unsigned char e_ident[EI_NIDENT];     /* Magic number and other info */
    Elf32_Half    e_type;                 /* Object file type */
    Elf32_Half    e_machine;              /* Architecture */
    Elf32_Word    e_version;              /* Object file version */
    Elf32_Addr    e_entry;                /* Entry point virtual address */
    Elf32_Off     e_phoff;                /* Program header table file offset */
    Elf32_Off     e_shoff;                /* Section header table file offset */
    Elf32_Word    e_flags;                /* Processor-specific flags */
    Elf32_Half    e_ehsize;               /* ELF header size in bytes */
    Elf32_Half    e_phentsize;            /* Program header table entry size */
    Elf32_Half    e_phnum;                /* Program header table entry count */
    Elf32_Half    e_shentsize;            /* Section header table entry size */
    Elf32_Half    e_shnum;                /* Section header table entry count */
    Elf32_Half    e_shstrndx;             /* Section header string table index */
} Elf32_Ehdr;
typedef struct
{
    unsigned char e_ident[EI_NIDENT];     /* Magic number and other info */
    Elf64_Half    e_type;                 /* Object file type */
    Elf64_Half    e_machine;              /* Architecture */
    Elf64_Word    e_version;              /* Object file version */
    Elf64_Addr    e_entry;                /* Entry point virtual address */
    Elf64_Off     e_phoff;                /* Program header table file offset */
    Elf64_Off     e_shoff;                /* Section header table file offset */
    Elf64_Word    e_flags;                /* Processor-specific flags */
    Elf64_Half    e_ehsize;               /* ELF header size in bytes */
    Elf64_Half    e_phentsize;            /* Program header table entry size */
    Elf64_Half    e_phnum;                /* Program header table entry count */
    Elf64_Half    e_shentsize;            /* Section header table entry size */
    Elf64_Half    e_shnum;                /* Section header table entry count */
    Elf64_Half    e_shstrndx;             /* Section header string table index */
} Elf64_Ehdr;


// program header
typedef struct
{
    Elf32_Word    p_type;                 /* Segment type */
    Elf32_Off     p_offset;               /* Segment file offset */
    Elf32_Addr    p_vaddr;                /* Segment virtual address */
    Elf32_Addr    p_paddr;                /* Segment physical address */
    Elf32_Word    p_filesz;               /* Segment size in file */
    Elf32_Word    p_memsz;                /* Segment size in memory */
    Elf32_Word    p_flags;                /* Segment flags */
    Elf32_Word    p_align;                /* Segment alignment */
} Elf32_Phdr;
typedef struct
{
    Elf64_Word    p_type;                 /* Segment type */
    Elf64_Off     p_offset;               /* Segment file offset */
    Elf64_Addr    p_vaddr;                /* Segment virtual address */
    Elf64_Addr    p_paddr;                /* Segment physical address */
    Elf64_Word    p_filesz;               /* Segment size in file */
    Elf64_Word    p_memsz;                /* Segment size in memory */
    Elf64_Word    p_flags;                /* Segment flags */
    Elf64_Word    p_align;                /* Segment alignment */
} Elf64_Phdr;


// section header
typedef struct
{
    Elf32_Word    sh_name;                /* Section name (string tbl index) */
    Elf32_Word    sh_type;                /* Section type */
    Elf32_Word    sh_flags;               /* Section flags */
    Elf32_Addr    sh_addr;                /* Section virtual addr at execution */
    Elf32_Off     sh_offset;              /* Section file offset */
    Elf32_Word    sh_size;                /* Section size in bytes */
    Elf32_Word    sh_link;                /* Link to another section */
    Elf32_Word    sh_info;                /* Additional section information */
    Elf32_Word    sh_addralign;           /* Section alignment */
    Elf32_Word    sh_entsize;             /* Entry size if section holds table */
} Elf32_Shdr;
typedef struct
{
    Elf64_Word    sh_name;                /* Section name (string tbl index) */
    Elf64_Word    sh_type;                /* Section type */
    Elf64_Xword   sh_flags;               /* Section flags */
    Elf64_Addr    sh_addr;                /* Section virtual addr at execution */
    Elf64_Off     sh_offset;              /* Section file offset */
    Elf64_Xword   sh_size;                /* Section size in bytes */
    Elf64_Word    sh_link;                /* Link to another section */
    Elf64_Word    sh_info;                /* Additional section information */
    Elf64_Xword   sh_addralign;           /* Section alignment */
    Elf64_Xword   sh_entsize;             /* Entry size if section holds table */
} Elf64_Shdr;


// symbol entry
typedef struct
{
    Elf32_Word    st_name;                /* Symbol name (string tbl index) */
    Elf32_Addr    st_value;               /* Symbol value */
    Elf32_Word    st_size;                /* Symbol size */
    unsigned char st_info;                /* Symbol type and binding */
    unsigned char st_other;               /* No defined meaning, 0 */
    Elf32_Section st_shndx;               /* Section index */
} Elf32_Sym;
typedef struct
{
    Elf64_Word    st_name;                /* Symbol name (string tbl index) */
    unsigned char st_info;                /* Symbol type and binding */
    unsigned char st_other;               /* No defined meaning, 0 */
    Elf64_Section st_shndx;               /* Section index */
    Elf64_Addr    st_value;               /* Symbol value */
    Elf64_Xword   st_size;                /* Symbol size */
} Elf64_Sym;

#ifdef _M_X64
    typedef Elf64_Ehdr Elf_Ehdr;
    typedef Elf64_Phdr Elf_Phdr;
    typedef Elf64_Shdr Elf_Shdr;
    typedef Elf64_Sym  Elf_Sym;
#else  // _M_X64
    typedef Elf32_Ehdr Elf_Ehdr;
    typedef Elf32_Phdr Elf_Phdr;
    typedef Elf32_Shdr Elf_Shdr;
    typedef Elf32_Sym  Elf_Sym;
#endif // _M_X64



// F: [](size_t size) -> void* : alloc func
template<class F>
inline bool dpMapFile(const char *path, void *&o_data, size_t &o_size, const F &alloc)
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
#ifdef _WIN32
    return ::VirtualAlloc(nullptr, size, MEM_COMMIT|MEM_RESERVE, PAGE_EXECUTE_READWRITE);
#else  // _WIN32
    return ::mmap(nullptr, size, PROT_EXEC|PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
#endif // _WIN32
}

void dpDeallocate(void *ptr)
{
#ifdef _WIN32
    ::VirtualFree(ptr, 0, MEM_RELEASE);
#else  // _WIN32
    ::munmap(ptr, 0);
#endif // _WIN32
}


struct ElfSymbol
{
    const char *name;
    void *addr;
};

class ElfFile
{
public:
    ElfFile();
    ~ElfFile();
    bool load(const char *path_to_elf);
    bool unload();
    const ElfSymbol* findSymbol(const char *name);

private:
    std::vector<ElfSymbol> m_symbols;
    void *m_elf_file;
    size_t m_elf_size;
};


ElfFile::ElfFile()
    : m_elf_file(nullptr)
    , m_elf_size(0)
{
}

ElfFile::~ElfFile()
{
    unload();
}

bool ElfFile::load( const char *path_to_elf )
{
    unload();

    if(!dpMapFile(path_to_elf, m_elf_file, m_elf_size, dpAllocateRWX)) {
        return false;
    }

    char *elf_data = (char*)m_elf_file;
    Elf_Ehdr *elf_header = (Elf_Ehdr*)elf_data;
    if(elf_header->e_ident[0]!=0x7F || elf_header->e_ident[1]!='E' || elf_header->e_ident[2]!='L' || elf_header->e_ident[3]!='F') {
        printf("ElfFile::load(): %s is not elf file.\n", path_to_elf);
        unload();
        return false;
    }

    Elf_Phdr *elf_pheader = nullptr;
    if(elf_header->e_phoff!=0) {
        elf_pheader = (Elf_Phdr*)(elf_data + elf_header->e_phoff);
    }

    Elf_Shdr *sections = (Elf_Shdr*)(elf_data + elf_header->e_shoff);
    const char *section_str = (const char*)(elf_data + sections[elf_header->e_shstrndx].sh_offset);

    for(int isec=0; isec<elf_header->e_shnum; ++isec) {
        Elf_Shdr *section = sections + isec;
        const char *section_name = section_str + section->sh_name;
        if(section->sh_type==SHT_SYMTAB) {
            const char *sym_str = (const char*)(elf_data + sections[section->sh_link].sh_offset);
            for(int isym=0; isym<section->sh_size; isym+=section->sh_entsize) {
                Elf_Sym *symbol = (Elf_Sym*)(elf_data + section->sh_offset + isym);
                const char *sym_name = sym_str + symbol->st_name;
                if(symbol->st_name==0 || symbol->st_shndx>=elf_header->e_shnum) { continue; }

                Elf_Shdr *sym_section = sections + symbol->st_shndx;
                char *sym_addr = elf_data + sym_section->sh_offset + symbol->st_value;
                if(elf_pheader) {
                    sym_addr -= elf_pheader->p_vaddr;
                }
                ElfSymbol sym = {sym_name, sym_addr};
                m_symbols.push_back(sym);
            }
        }
    }
    std::sort(m_symbols.begin(), m_symbols.end(),
        [&](ElfSymbol &a, ElfSymbol &b){ return strcmp(a.name, b.name)<0; });

    return true;
}

bool ElfFile::unload()
{
    if(m_elf_file) {
        dpDeallocate(m_elf_file);
        m_elf_file = nullptr;
        m_elf_size = 0;
        m_symbols.clear();
        return true;
    }
    return false;
}

const ElfSymbol* ElfFile::findSymbol(const char *name)
{
    auto iter = std::lower_bound(m_symbols.begin(), m_symbols.end(), name,
        [&](ElfSymbol &sym, const char *n){ return strcmp(sym.name, n)<0; });
    if(iter!=m_symbols.end() && strcmp(iter->name, name)==0) {
        return &(*iter);
    }
    return nullptr;
}



int main(int argc, char *argv[])
{
    if(argc<2) {
        printf("usage: %s [path to .o]\n", argv[0]);
        return 0;
    }

    ElfFile elf;
    if(elf.load(argv[1])) {
        typedef int (*add_t)(int, int);
        if(const ElfSymbol *sym = elf.findSymbol("add")) {
            add_t add = (add_t)sym->addr;
            printf("add(1,2): %d\n", add(1,2));
        }
    }
}
