#include <cstdint>

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

#define EI_CLASS        4               /* File class byte index */
#define ELFCLASSNONE    0               /* Invalid class */
#define ELFCLASS32      1               /* 32-bit objects */
#define ELFCLASS64      2               /* 64-bit objects */

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



// Segment types.
enum {
    PT_NULL    = 0, // Unused segment.
    PT_LOAD    = 1, // Loadable segment.
    PT_DYNAMIC = 2, // Dynamic linking information.
    PT_INTERP  = 3, // Interpreter pathname.
    PT_NOTE    = 4, // Auxiliary information.
    PT_SHLIB   = 5, // Reserved.
    PT_PHDR    = 6, // The program header table itself.
    PT_TLS     = 7, // The thread-local storage template.
    PT_LOOS    = 0x60000000, // Lowest operating system-specific pt entry type.
    PT_HIOS    = 0x6fffffff, // Highest operating system-specific pt entry type.
    PT_LOPROC  = 0x70000000, // Lowest processor-specific program hdr entry type.
    PT_HIPROC  = 0x7fffffff, // Highest processor-specific program hdr entry type.

    // x86-64 program header types.
    // These all contain stack unwind tables.
    PT_GNU_EH_FRAME  = 0x6474e550,
    PT_SUNW_EH_FRAME = 0x6474e550,
    PT_SUNW_UNWIND   = 0x6464e550,

    PT_GNU_STACK  = 0x6474e551, // Indicates stack executability.
    PT_GNU_RELRO  = 0x6474e552, // Read-only after relocation.

    // ARM program header types.
    PT_ARM_ARCHEXT = 0x70000000, // Platform architecture compatibility info
    // These all contain stack unwind tables.
    PT_ARM_EXIDX   = 0x70000001,
    PT_ARM_UNWIND  = 0x70000001
};

// Segment flag bits.
enum {
    PF_X        = 1,         // Execute
    PF_W        = 2,         // Write
    PF_R        = 4,         // Read
    PF_MASKOS   = 0x0ff00000,// Bits for operating system-specific semantics.
    PF_MASKPROC = 0xf0000000 // Bits for processor-specific semantics.
};

// Dynamic table entry for ELF32.
struct Elf32_Dyn
{
	Elf32_Sword d_tag;            // Type of dynamic table entry.
	union
	{
		Elf32_Word d_val;         // Integer value of entry.
		Elf32_Addr d_ptr;         // Pointer value of entry.
	} d_un;
};

// Dynamic table entry for ELF64.
struct Elf64_Dyn
{
	Elf64_Sxword d_tag;           // Type of dynamic table entry.
	union
	{
		Elf64_Xword d_val;        // Integer value of entry.
		Elf64_Addr  d_ptr;        // Pointer value of entry.
	} d_un;
};

// Dynamic table entry tags.
enum {
	DT_NULL         = 0,        // Marks end of dynamic array.
	DT_NEEDED       = 1,        // String table offset of needed library.
	DT_PLTRELSZ     = 2,        // Size of relocation entries in PLT.
	DT_PLTGOT       = 3,        // Address associated with linkage table.
	DT_HASH         = 4,        // Address of symbolic hash table.
	DT_STRTAB       = 5,        // Address of dynamic string table.
	DT_SYMTAB       = 6,        // Address of dynamic symbol table.
	DT_RELA         = 7,        // Address of relocation table (Rela entries).
	DT_RELASZ       = 8,        // Size of Rela relocation table.
	DT_RELAENT      = 9,        // Size of a Rela relocation entry.
	DT_STRSZ        = 10,       // Total size of the string table.
	DT_SYMENT       = 11,       // Size of a symbol table entry.
	DT_INIT         = 12,       // Address of initialization function.
	DT_FINI         = 13,       // Address of termination function.
	DT_SONAME       = 14,       // String table offset of a shared objects name.
	DT_RPATH        = 15,       // String table offset of library search path.
	DT_SYMBOLIC     = 16,       // Changes symbol resolution algorithm.
	DT_REL          = 17,       // Address of relocation table (Rel entries).
	DT_RELSZ        = 18,       // Size of Rel relocation table.
	DT_RELENT       = 19,       // Size of a Rel relocation entry.
	DT_PLTREL       = 20,       // Type of relocation entry used for linking.
	DT_DEBUG        = 21,       // Reserved for debugger.
	DT_TEXTREL      = 22,       // Relocations exist for non-writable segments.
	DT_JMPREL       = 23,       // Address of relocations associated with PLT.
	DT_BIND_NOW     = 24,       // Process all relocations before execution.
	DT_INIT_ARRAY   = 25,       // Pointer to array of initialization functions.
	DT_FINI_ARRAY   = 26,       // Pointer to array of termination functions.
	DT_INIT_ARRAYSZ = 27,       // Size of DT_INIT_ARRAY.
	DT_FINI_ARRAYSZ = 28,       // Size of DT_FINI_ARRAY.
	DT_RUNPATH      = 29,       // String table offset of lib search path.
	DT_FLAGS        = 30,       // Flags.
	DT_ENCODING     = 32,       // Values from here to DT_LOOS follow the rules
	// for the interpretation of the d_un union.

	DT_PREINIT_ARRAY = 32,      // Pointer to array of preinit functions.
	DT_PREINIT_ARRAYSZ = 33,    // Size of the DT_PREINIT_ARRAY array.

	DT_LOOS         = 0x60000000, // Start of environment specific tags.
	DT_HIOS         = 0x6FFFFFFF, // End of environment specific tags.
	DT_LOPROC       = 0x70000000, // Start of processor specific tags.
	DT_HIPROC       = 0x7FFFFFFF, // End of processor specific tags.

	DT_RELACOUNT    = 0x6FFFFFF9, // ELF32_Rela count.
	DT_RELCOUNT     = 0x6FFFFFFA, // ELF32_Rel count.

	DT_FLAGS_1      = 0X6FFFFFFB, // Flags_1.
	DT_VERDEF       = 0X6FFFFFFC, // The address of the version definition table.
	DT_VERDEFNUM    = 0X6FFFFFFD, // The number of entries in DT_VERDEF.
	DT_VERNEED      = 0X6FFFFFFE, // The address of the version Dependency table.
	DT_VERNEEDNUM   = 0X6FFFFFFF, // The number of entries in DT_VERNEED.

	// Mips specific dynamic table entry tags.
	DT_MIPS_RLD_VERSION   = 0x70000001, // 32 bit version number for runtime
	// linker interface.
	DT_MIPS_TIME_STAMP    = 0x70000002, // Time stamp.
	DT_MIPS_ICHECKSUM     = 0x70000003, // Checksum of external strings
	// and common sizes.
	DT_MIPS_IVERSION      = 0x70000004, // Index of version string
	// in string table.
	DT_MIPS_FLAGS         = 0x70000005, // 32 bits of flags.
	DT_MIPS_BASE_ADDRESS  = 0x70000006, // Base address of the segment.
	DT_MIPS_MSYM          = 0x70000007, // Address of .msym section.
	DT_MIPS_CONFLICT      = 0x70000008, // Address of .conflict section.
	DT_MIPS_LIBLIST       = 0x70000009, // Address of .liblist section.
	DT_MIPS_LOCAL_GOTNO   = 0x7000000a, // Number of local global offset
	// table entries.
	DT_MIPS_CONFLICTNO    = 0x7000000b, // Number of entries
	// in the .conflict section.
	DT_MIPS_LIBLISTNO     = 0x70000010, // Number of entries
	// in the .liblist section.
	DT_MIPS_SYMTABNO      = 0x70000011, // Number of entries
	// in the .dynsym section.
	DT_MIPS_UNREFEXTNO    = 0x70000012, // Index of first external dynamic symbol
	// not referenced locally.
	DT_MIPS_GOTSYM        = 0x70000013, // Index of first dynamic symbol
	// in global offset table.
	DT_MIPS_HIPAGENO      = 0x70000014, // Number of page table entries
	// in global offset table.
	DT_MIPS_RLD_MAP       = 0x70000016, // Address of run time loader map,
	// used for debugging.
	DT_MIPS_DELTA_CLASS       = 0x70000017, // Delta C++ class definition.
	DT_MIPS_DELTA_CLASS_NO    = 0x70000018, // Number of entries
	// in DT_MIPS_DELTA_CLASS.
	DT_MIPS_DELTA_INSTANCE    = 0x70000019, // Delta C++ class instances.
	DT_MIPS_DELTA_INSTANCE_NO = 0x7000001A, // Number of entries
	// in DT_MIPS_DELTA_INSTANCE.
	DT_MIPS_DELTA_RELOC       = 0x7000001B, // Delta relocations.
	DT_MIPS_DELTA_RELOC_NO    = 0x7000001C, // Number of entries
	// in DT_MIPS_DELTA_RELOC.
	DT_MIPS_DELTA_SYM         = 0x7000001D, // Delta symbols that Delta
	// relocations refer to.
	DT_MIPS_DELTA_SYM_NO      = 0x7000001E, // Number of entries
	// in DT_MIPS_DELTA_SYM.
	DT_MIPS_DELTA_CLASSSYM    = 0x70000020, // Delta symbols that hold
	// class declarations.
	DT_MIPS_DELTA_CLASSSYM_NO = 0x70000021, // Number of entries
	// in DT_MIPS_DELTA_CLASSSYM.
	DT_MIPS_CXX_FLAGS         = 0x70000022, // Flags indicating information
	// about C++ flavor.
	DT_MIPS_PIXIE_INIT        = 0x70000023, // Pixie information.
	DT_MIPS_SYMBOL_LIB        = 0x70000024, // Address of .MIPS.symlib
	DT_MIPS_LOCALPAGE_GOTIDX  = 0x70000025, // The GOT index of the first PTE
	// for a segment
	DT_MIPS_LOCAL_GOTIDX      = 0x70000026, // The GOT index of the first PTE
	// for a local symbol
	DT_MIPS_HIDDEN_GOTIDX     = 0x70000027, // The GOT index of the first PTE
	// for a hidden symbol
	DT_MIPS_PROTECTED_GOTIDX  = 0x70000028, // The GOT index of the first PTE
	// for a protected symbol
	DT_MIPS_OPTIONS           = 0x70000029, // Address of `.MIPS.options'.
	DT_MIPS_INTERFACE         = 0x7000002A, // Address of `.interface'.
	DT_MIPS_DYNSTR_ALIGN      = 0x7000002B, // Unknown.
	DT_MIPS_INTERFACE_SIZE    = 0x7000002C, // Size of the .interface section.
	DT_MIPS_RLD_TEXT_RESOLVE_ADDR = 0x7000002D, // Size of rld_text_resolve
	// function stored in the GOT.
	DT_MIPS_PERF_SUFFIX       = 0x7000002E, // Default suffix of DSO to be added
	// by rld on dlopen() calls.
	DT_MIPS_COMPACT_SIZE      = 0x7000002F, // Size of compact relocation
	// section (O32).
	DT_MIPS_GP_VALUE          = 0x70000030, // GP value for auxiliary GOTs.
	DT_MIPS_AUX_DYNAMIC       = 0x70000031, // Address of auxiliary .dynamic.
	DT_MIPS_PLTGOT            = 0x70000032, // Address of the base of the PLTGOT.
	DT_MIPS_RWPLT             = 0x70000034  // Points to the base
	// of a writable PLT.
};

// DT_FLAGS values.
enum {
	DF_ORIGIN     = 0x01, // The object may reference $ORIGIN.
	DF_SYMBOLIC   = 0x02, // Search the shared lib before searching the exe.
	DF_TEXTREL    = 0x04, // Relocations may modify a non-writable segment.
	DF_BIND_NOW   = 0x08, // Process all relocations on load.
	DF_STATIC_TLS = 0x10  // Reject attempts to load dynamically.
};

// X86_64 relocations.
enum {
    R_X86_64_NONE       = 0,
    R_X86_64_64         = 1,
    R_X86_64_PC32       = 2,
    R_X86_64_GOT32      = 3,
    R_X86_64_PLT32      = 4,
    R_X86_64_COPY       = 5,
    R_X86_64_GLOB_DAT   = 6,
    R_X86_64_JUMP_SLOT  = 7,
    R_X86_64_RELATIVE   = 8,
    R_X86_64_GOTPCREL   = 9,
    R_X86_64_32         = 10,
    R_X86_64_32S        = 11,
    R_X86_64_16         = 12,
    R_X86_64_PC16       = 13,
    R_X86_64_8          = 14,
    R_X86_64_PC8        = 15,
    R_X86_64_DTPMOD64   = 16,
    R_X86_64_DTPOFF64   = 17,
    R_X86_64_TPOFF64    = 18,
    R_X86_64_TLSGD      = 19,
    R_X86_64_TLSLD      = 20,
    R_X86_64_DTPOFF32   = 21,
    R_X86_64_GOTTPOFF   = 22,
    R_X86_64_TPOFF32    = 23,
    R_X86_64_PC64       = 24,
    R_X86_64_GOTOFF64   = 25,
    R_X86_64_GOTPC32    = 26,
    R_X86_64_GOT64      = 27,
    R_X86_64_GOTPCREL64 = 28,
    R_X86_64_GOTPC64    = 29,
    R_X86_64_GOTPLT64   = 30,
    R_X86_64_PLTOFF64   = 31,
    R_X86_64_SIZE32     = 32,
    R_X86_64_SIZE64     = 33,
    R_X86_64_GOTPC32_TLSDESC = 34,
    R_X86_64_TLSDESC_CALL    = 35,
    R_X86_64_TLSDESC    = 36,
    R_X86_64_IRELATIVE  = 37
};

// i386 relocations.
enum {
    R_386_NONE          = 0,
    R_386_32            = 1,
    R_386_PC32          = 2,
    R_386_GOT32         = 3,
    R_386_PLT32         = 4,
    R_386_COPY          = 5,
    R_386_GLOB_DAT      = 6,
    R_386_JUMP_SLOT     = 7,
    R_386_RELATIVE      = 8,
    R_386_GOTOFF        = 9,
    R_386_GOTPC         = 10,
    R_386_32PLT         = 11,
    R_386_TLS_TPOFF     = 14,
    R_386_TLS_IE        = 15,
    R_386_TLS_GOTIE     = 16,
    R_386_TLS_LE        = 17,
    R_386_TLS_GD        = 18,
    R_386_TLS_LDM       = 19,
    R_386_16            = 20,
    R_386_PC16          = 21,
    R_386_8             = 22,
    R_386_PC8           = 23,
    R_386_TLS_GD_32     = 24,
    R_386_TLS_GD_PUSH   = 25,
    R_386_TLS_GD_CALL   = 26,
    R_386_TLS_GD_POP    = 27,
    R_386_TLS_LDM_32    = 28,
    R_386_TLS_LDM_PUSH  = 29,
    R_386_TLS_LDM_CALL  = 30,
    R_386_TLS_LDM_POP   = 31,
    R_386_TLS_LDO_32    = 32,
    R_386_TLS_IE_32     = 33,
    R_386_TLS_LE_32     = 34,
    R_386_TLS_DTPMOD32  = 35,
    R_386_TLS_DTPOFF32  = 36,
    R_386_TLS_TPOFF32   = 37,
    R_386_TLS_GOTDESC   = 39,
    R_386_TLS_DESC_CALL = 40,
    R_386_TLS_DESC      = 41,
    R_386_IRELATIVE     = 42,
    R_386_NUM           = 43
};


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


typedef struct
{
    Elf32_Addr    r_offset;               /* Address */
    Elf32_Word    r_info;                 /* Relocation type and symbol index */
} Elf32_Rel;

typedef struct
{
    Elf32_Addr    r_offset;               /* Address */
    Elf32_Word    r_info;                 /* Relocation type and symbol index */
    Elf32_Sword   r_addend;               /* Addend */
} Elf32_Rela;


typedef struct
{
    Elf64_Addr    r_offset;
    Elf64_Byte    r_type;                 /* 1st relocation op type */
    Elf64_Byte    r_type2;                /* 2nd relocation op type */
    Elf64_Byte    r_type3;                /* 3rd relocation op type */
    Elf64_Byte    r_ssym;                 /* Special symbol */
    Elf64_Word    r_sym;                  /* Symbol index */
} Elf64_Rel;

typedef struct {
    Elf64_Addr    r_offset; // Location (file byte offset, or program virtual addr).
    Elf64_Xword   r_info;   // Symbol table index and type of relocation to apply.
    Elf64_Sxword  r_addend; // Compute value for relocatable field by adding this.
} Elf64_Rela;
