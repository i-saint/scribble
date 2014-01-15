#include <windows.h>
#include <dbghelp.h>
#include <cstdio>
#include <cstdint>
#include <algorithm>
#include <vector>
#include <map>


bool GetFunctionByFileAndLine(
    const char *path_to_obj,
    const char *fullpath_to_cpp, // optional, but recommended. if this is null, search all file's lines
    int line,
    char *out_funcname,
    int len_funcname
    );


void testfunc()
{
    printf("");
}


int main(int argc, char *argv[])
{
    char cpp_fullpath[MAX_PATH+1];
    ::GetFullPathNameA("ParseCV8.cpp", sizeof(cpp_fullpath), cpp_fullpath, nullptr);

    char funcname[MAX_SYM_NAME+1];
    GetFunctionByFileAndLine("ParseCV8.obj", cpp_fullpath, 21, funcname, sizeof(funcname));

    printf("%s\n", funcname);
}

/*
$ cl ParseCV8.cpp /Zi /EHsc
$ ./ParseCV8
?testfunc@@YAXXZ
*/




#ifdef max
#	undef max
#	undef min
#endif // max

#ifdef _M_X64
#	define OnX64(...) __VA_ARGS__
#	define OnX86(...) 
#else  // _M_X64
#	define OnX64(...) 
#	define OnX86(...) __VA_ARGS__
#endif // _M_X64


// F: [](size_t size) -> void* : malloc(), wrapped VirtualAlloc(), etc
template<class F>
inline bool dpMapFile(const char *path, void *&o_data, size_t &o_size, const F &alloc)
{
    o_data = NULL;
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


struct SymbolData
{
    const char *name;
    const char *sourcefile;
    uint32_t section_index;
    uint32_t section_offset;
    uint32_t line_min;
    uint32_t line_max;

    SymbolData(const char *n=nullptr, uint32_t si=0, uint32_t so=0)
        : name(n)
        , sourcefile(nullptr)
        , section_index(si)
        , section_offset(so)
        , line_min(~0)
        , line_max(~0)
    {}

    void setLine(uint32_t l)
    {
        line_min = line_min==~0 ? l : std::min(line_min, l);
        line_max = line_max==~0 ? l : std::max(line_max, l);
    }

    bool isLineInside(uint32_t l)
    {
        return l>=line_min && l<=line_max;
    }
};
inline bool operator<(const SymbolData &a, const SymbolData &b) { return a.section_offset < b.section_offset; }

struct SectionData
{
    typedef std::vector<SymbolData> SymbolDataCont;
    const char *name;
    uint32_t section_size;
    SymbolDataCont symbols;

    void sortSymbolsByOffset()
    {
        std::sort(symbols.begin(), symbols.end());
    }

    SymbolData* findSymbolByOffset(uint32_t offset)
    {
        auto it = std::lower_bound(symbols.begin(), symbols.end(), offset, [](const SymbolData &a, uint32_t o){ return a.section_offset<o; });
        if(it==symbols.end()) { return nullptr; }
        if(it!=symbols.begin() && it->section_offset!=offset) {
            --it;
        }
        return &(*it);
    }

    SymbolData* findSymbolByLine(uint32_t line, const char *sourcefile)
    {
        for(size_t symi=0; symi<symbols.size(); ++symi) {
            if( symbols[symi].isLineInside(line) && (!sourcefile || stricmp(symbols[symi].sourcefile, sourcefile)==0))
            {
                return &symbols[symi];
            }
        }
        return nullptr;
    }
};

static inline const char* dpGetSymbolName(PSTR pStringTable, PIMAGE_SYMBOL pSym)
{
    return pSym->N.Name.Short!=0 ? (const char*)&pSym->N.ShortName : (const char*)(pStringTable + pSym->N.Name.Long);
}


// CV8 data structures
// thanks for http://sage.math.washington.edu/home/wstein/www/home/wbhart/mpir-1.2.2/yasm/modules/dbgfmts/codeview/cv8.txt
 
struct CV8Data
{
    uint32_t type;
    uint32_t length;

    template<class T> T* cast();
};

struct CV8Data_Symbols : public CV8Data
{
    struct LengthTypePair
    {
        uint16_t length;
        uint16_t type;
    };

    LengthTypePair pairs[1];
};

struct CV8Data_Lines : public CV8Data
{
    struct LineInfo
    {
        uint32_t offset;
        uint32_t line;
    };

    uint32_t section_offset;
    uint16_t section_index;
    uint16_t pad;
    uint32_t section_length_covered_by_lines;
    uint32_t fileinfo_index;
    uint32_t num_lines;
    uint32_t size;
    LineInfo lines[1];

    // F: [](LineInfo&) -> void
    template<class F>
    void EachLines(const F &f)
    {
        for(uint32_t i=0; i<num_lines; ++i) {
            LineInfo li = {
                lines[i].offset + section_offset,
                lines[i].line & ~0x80000000
            };
            f(li);
        }
    }
};

struct CV8Data_StrTab : public CV8Data
{
    char strtab[1];
};

struct CV8Data_FileInfo : public CV8Data
{
    struct FileInfo {
        uint32_t strtab_offset;
        uint16_t flags;
        uint8_t md5[16]; // only when flags==0x0110
        uint16_t pad;
    };

    // F: [](FileInfo&) -> void
    template<class F>
    void EachFiles(const F &f)
    {
        for(uint32_t pos=sizeof(CV8Data); pos<length; ) {
            FileInfo *info = (FileInfo*)((size_t)this+pos);
            f(*info);
            pos += info->flags==0x0110 ? sizeof(FileInfo) : sizeof(FileInfo)-16;
        }
    }
};

struct CV8Data_Unknown : public CV8Data
{
    // todo?
};

template<> inline CV8Data_Symbols*	CV8Data::cast()	{ return type==0x000000F1 ? (CV8Data_Symbols*)this : nullptr; }
template<> inline CV8Data_Lines*	CV8Data::cast()	{ return type==0x000000F2 ? (CV8Data_Lines*)this : nullptr; }
template<> inline CV8Data_StrTab*	CV8Data::cast()	{ return type==0x000000F3 ? (CV8Data_StrTab*)this : nullptr; }
template<> inline CV8Data_FileInfo*	CV8Data::cast()	{ return type==0x000000F4 ? (CV8Data_FileInfo*)this : nullptr; }
template<> inline CV8Data_Unknown*	CV8Data::cast()	{ return type==0x000000F5 ? (CV8Data_Unknown*)this : nullptr; }


// F: [](CV8Data&) -> void
template<class F>
inline bool EachCV8Data(void *data_, uint32_t size, const F &f)
{
    char *data = (char*)data_;
    // first 4-byte in CV8 must be 4
    if(*(uint32_t*)data!=4) {
        return false;
    }

    size_t pos = 4;
    for(; pos<size; ) {
        CV8Data *cv8 = (CV8Data*)(data+pos);
        f(*cv8);
        pos += cv8->length + sizeof(CV8Data);
        pos = (pos+3) & ~3; // 4 byte align
    }
    return true;
}



bool GetFunctionByFileAndLineImpl(void *data, const char *fullpath_to_cpp, int line, char *out_funcname, int len_funcname)
{
    size_t ImageBase = (size_t)(data);
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)ImageBase;
#ifdef _M_X64
    if( pDosHeader->e_magic!=IMAGE_FILE_MACHINE_AMD64 || pDosHeader->e_sp!=0 ) {
#else // _M_X64
    if( pDosHeader->e_magic!=IMAGE_FILE_MACHINE_I386 || pDosHeader->e_sp!=0 ) {
#endif // _M_X64
        return false;
    }

    PIMAGE_FILE_HEADER pImageHeader = (PIMAGE_FILE_HEADER)ImageBase;
    PIMAGE_SECTION_HEADER pSectionHeader = (PIMAGE_SECTION_HEADER)(ImageBase + sizeof(IMAGE_FILE_HEADER) + pImageHeader->SizeOfOptionalHeader);
    PIMAGE_SYMBOL pSymbolTable = (PIMAGE_SYMBOL)((size_t)pImageHeader + pImageHeader->PointerToSymbolTable);
    DWORD SymbolCount = pImageHeader->NumberOfSymbols;
    PSTR StringTable = (PSTR)&pSymbolTable[SymbolCount];

    char *CV8StrTab = nullptr;
    char *CV8FileTab = nullptr;
    std::map<uint32_t, SectionData> section_data;

    // gather function symbols
    for( DWORD symi=0; symi < SymbolCount; ++symi ) {
        PIMAGE_SYMBOL sym = pSymbolTable + symi;
        if(sym->SectionNumber > 0) {
            IMAGE_SECTION_HEADER &sect = pSectionHeader[sym->SectionNumber-1];
            if(sym->SectionNumber==IMAGE_SYM_UNDEFINED || (sect.Characteristics&IMAGE_SCN_CNT_CODE)==0) { continue; }
            const char *name = dpGetSymbolName(StringTable, sym);
            if(name[0]!='.' && name[0]!='$') {
                section_data[sym->SectionNumber].symbols.push_back(SymbolData(name, symi, sym->Value));
            }
        }
        symi += pSymbolTable[symi].NumberOfAuxSymbols;
    }
    std::for_each(section_data.begin(), section_data.end(),
        [](std::pair<const uint32_t, SectionData> &p){ p.second.sortSymbolsByOffset(); });

    // relocate .debug$S sections and gather line numbers
    for(DWORD secti=0; secti<pImageHeader->NumberOfSections; ++secti) {
        IMAGE_SECTION_HEADER &sect = pSectionHeader[secti];
        size_t SectionBase = (size_t)(ImageBase + (int)sect.PointerToRawData);
        if(strncmp((char*)sect.Name, ".debug$S", 8)!=0) { continue; }

        // relocation
        {
            DWORD NumRelocations = sect.NumberOfRelocations;
            DWORD FirstRelocation = 0;
            // if NumberOfRelocations==0xffff, first IMAGE_RELOCATION has actual data (due to NumberOfRelocations is 16-bit)
            if(sect.NumberOfRelocations==0xffff && (sect.Characteristics&IMAGE_SCN_LNK_NRELOC_OVFL)!=0) {
                NumRelocations = ((PIMAGE_RELOCATION)(ImageBase + (int)sect.PointerToRelocations))[0].RelocCount;
                FirstRelocation = 1;
            }

            PIMAGE_RELOCATION pRelocation = (PIMAGE_RELOCATION)(ImageBase + (int)sect.PointerToRelocations);
            for(size_t ri=FirstRelocation; ri<NumRelocations; ++ri) {
                PIMAGE_RELOCATION pReloc = pRelocation + ri;
                PIMAGE_SYMBOL rsym = pSymbolTable + pReloc->SymbolTableIndex;
                size_t addr = SectionBase + pReloc->VirtualAddress;

                switch(pReloc->Type) {
                OnX64(case IMAGE_REL_AMD64_SECTION:)
                OnX86(case IMAGE_REL_I386_SECTION:)
                    {
                        *(SHORT*)(addr) = rsym->SectionNumber;
                        break;
                    }
                OnX64(case IMAGE_REL_AMD64_SECREL:)
                OnX86(case IMAGE_REL_I386_SECREL:)
                    {
                        *(DWORD*)(addr) = rsym->Value; // i'm not sure this is correct...
                        break;
                    }
                }
            }
        }

        // get cv8 string tables & source file info tables
        EachCV8Data((void*)SectionBase, sect.SizeOfRawData, [&](CV8Data &cv8){
            if(CV8Data_StrTab *strtab = cv8.cast<CV8Data_StrTab>()) {
                CV8StrTab = strtab->strtab;
            }
            else if(CV8Data_FileInfo *finfo = cv8.cast<CV8Data_FileInfo>()) {
                CV8FileTab = (char*)finfo + 8;
            }
        });
        // gather line numbers
        EachCV8Data((void*)SectionBase, sect.SizeOfRawData, [&](CV8Data &cv8){
            if(CV8Data_Lines *lines = cv8.cast<CV8Data_Lines>()) {
                CV8Data_FileInfo::FileInfo *fileinfo = (CV8Data_FileInfo::FileInfo*)(CV8FileTab + lines->fileinfo_index);
                const char *filepath = CV8StrTab + fileinfo->strtab_offset;

                lines->EachLines([&](CV8Data_Lines::LineInfo &li){
                    if(SymbolData *sym = section_data[lines->section_index].findSymbolByOffset(li.offset)) {
                        sym->setLine(li.line);
                        sym->sourcefile = filepath;
                    }
                });
            }
        });
    }

    // find symbol
    for(auto seci=section_data.begin(); seci!=section_data.end(); ++seci) {
        SectionData &sect = seci->second;
        if(SymbolData *sym=sect.findSymbolByLine(line, fullpath_to_cpp)) {
            strncpy(out_funcname, sym->name, len_funcname);
            return true;
        }
    }
    return false;
}

bool GetFunctionByFileAndLine(
    const char *path_to_obj,
    const char *fullpath_to_cpp,
    int line,
    char *out_funcname,
    int len_funcname )
{
    char *obj_data;
    size_t obj_size;
    if(!dpMapFile(path_to_obj, (void*&)obj_data, obj_size, malloc)) {
        return false;
    }
    bool ret = GetFunctionByFileAndLineImpl(obj_data, fullpath_to_cpp, line, out_funcname, len_funcname);
    free(obj_data);
    return ret;
}
