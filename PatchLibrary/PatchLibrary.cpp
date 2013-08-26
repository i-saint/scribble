#include <windows.h>
#include <string>
#include <vector>
#include <map>
#include <algorithm>


namespace {

struct Symbol
{
    const char *name;
    void *before;
    void *after;
};

class DLL
{
public:
    typedef std::vector<Symbol> SymbolCont;

    DLL();
    ~DLL();
    bool load(const char *path);
    bool unload();
    void patch();
    void unpatch();

    HMODULE getHandle() const { return m_module; }

private:
    SymbolCont m_symbols;
    std::string m_base_path;
    std::string m_dll_path;
    std::string m_pdb_path;
    HMODULE m_module;
    HMODULE m_parent;
};

class PatchLibraryImpl
{
public:
    typedef std::map<std::string, DLL*> DLLCont;

    PatchLibraryImpl();
    ~PatchLibraryImpl();
    HMODULE loadAndPatch(const char *path);
    BOOL unpatchAndUnload(HMODULE mod);

private:
    DLLCont m_dlls;
};



// 以下 DynamicPatcher ( https://github.com/i-saint/DynamicPatcher ) から流用

template<class Container, class F>
inline void dpEach(Container &cont, const F &f)
{
    std::for_each(cont.begin(), cont.end(), f);
}

template<class Container, class F>
inline auto dpFind(Container &cont, const F &f) -> decltype(cont.begin())
{
    return std::find_if(cont.begin(), cont.end(), f);
}

// F: functor(const char *name, void *sym)
template<class F>
inline void dpEnumerateDLLExports(HMODULE module, const F &f)
{
    if(module==NULL) { return; }

    size_t ImageBase = (size_t)module;
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)ImageBase;
    if(pDosHeader->e_magic!=IMAGE_DOS_SIGNATURE) { return; }

    PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)(ImageBase + pDosHeader->e_lfanew);
    DWORD RVAExports = pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
    if(RVAExports==0) { return; }

    IMAGE_EXPORT_DIRECTORY *pExportDirectory = (IMAGE_EXPORT_DIRECTORY *)(ImageBase + RVAExports);
    DWORD *RVANames = (DWORD*)(ImageBase+pExportDirectory->AddressOfNames);
    WORD *RVANameOrdinals = (WORD*)(ImageBase+pExportDirectory->AddressOfNameOrdinals);
    DWORD *RVAFunctions = (DWORD*)(ImageBase+pExportDirectory->AddressOfFunctions);
    for(DWORD i=0; i<pExportDirectory->NumberOfFunctions; ++i) {
        char *pName = (char*)(ImageBase+RVANames[i]);
        void *pFunc = (void*)(ImageBase+RVAFunctions[RVANameOrdinals[i]]);
        f(pName, pFunc);
    }
}

// F: [](size_t size) -> void* : alloc func
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

// fill_gap: .dll ファイルをそのままメモリに移した場合はこれを true にする必要があります。
// LoadLibrary() で正しくロードしたものは section の再配置が行われ、元ファイルとはデータの配置にズレが生じます。
// fill_gap==true の場合このズレを補正します。
char* dpGetPDBPathFromModule(void *pModule, bool fill_gap)
{
    if(!pModule) { return nullptr; }

    struct CV_INFO_PDB70
    {
        DWORD  CvSignature;
        GUID Signature;
        DWORD Age;
        BYTE PdbFileName[1];
    };

    PBYTE pData = (PUCHAR)pModule;
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pData;
    PIMAGE_NT_HEADERS pNtHeaders = (PIMAGE_NT_HEADERS)(pData + pDosHeader->e_lfanew);
    if(pDosHeader->e_magic==IMAGE_DOS_SIGNATURE && pNtHeaders->Signature==IMAGE_NT_SIGNATURE) {
        ULONG DebugRVA = pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG].VirtualAddress;
        if(DebugRVA==0) { return nullptr; }

        PIMAGE_SECTION_HEADER pSectionHeader = IMAGE_FIRST_SECTION(pNtHeaders);
        for(size_t i=0; i<pNtHeaders->FileHeader.NumberOfSections; ++i) {
            PIMAGE_SECTION_HEADER s = pSectionHeader+i;
            if(DebugRVA >= s->VirtualAddress && DebugRVA < s->VirtualAddress+s->SizeOfRawData) {
                pSectionHeader = s;
                break;
            }
        }
        if(fill_gap) {
            DWORD gap = pSectionHeader->VirtualAddress - pSectionHeader->PointerToRawData;
            pData -= gap;
        }

        PIMAGE_DEBUG_DIRECTORY pDebug;
        pDebug = (PIMAGE_DEBUG_DIRECTORY)(pData + DebugRVA);
        if(DebugRVA!=0 && DebugRVA < pNtHeaders->OptionalHeader.SizeOfImage && pDebug->Type==IMAGE_DEBUG_TYPE_CODEVIEW) {
            CV_INFO_PDB70 *pCVI = (CV_INFO_PDB70*)(pData + pDebug->AddressOfRawData);
            if(pCVI->CvSignature=='SDSR') {
                return (char*)pCVI->PdbFileName;
            }
        }
    }
    return nullptr;
}

bool dpCopyFile(const char *srcpath, const char *dstpath)
{
    return ::CopyFileA(srcpath, dstpath, FALSE)==TRUE;
}

bool dpWriteFile(const char *path, const void *data, size_t size)
{
    if(FILE *f=fopen(path, "wb")) {
        fwrite((const char*)data, 1, size, f);
        fclose(f);
        return true;
    }
    return false;
}

bool dpDeleteFile(const char *path)
{
    return ::DeleteFileA(path)==TRUE;
}

bool dpFileExists( const char *path )
{
    return ::GetFileAttributesA(path)!=INVALID_FILE_ATTRIBUTES;
}

size_t dpSeparateDirFile(const char *path, std::string *dir, std::string *file)
{
    size_t f_len=0;
    size_t l = strlen(path);
    for(size_t i=0; i<l; ++i) {
        if(path[i]=='\\' || path[i]=='/') { f_len=i+1; }
    }
    if(dir)  { dir->insert(dir->end(), path, path+f_len); }
    if(file) { file->insert(file->end(), path+f_len, path+l); }
    return f_len;
}

size_t dpSeparateFileExt(const char *filename, std::string *file, std::string *ext)
{
    size_t dir_len=0;
    size_t l = strlen(filename);
    for(size_t i=0; i<l; ++i) {
        if(filename[i]=='.') { dir_len=i+1; }
    }
    if(file){ file->insert(file->end(), filename, filename+dir_len); }
    if(ext) { ext->insert(ext->end(), filename+dir_len, filename+l); }
    return dir_len;
}




DLL::DLL()
    : m_module(nullptr), m_parent(nullptr)
{
}

DLL::~DLL()
{
    unpatch();
    unload();
}

bool DLL::load( const char *path )
{
    {
        std::string dir, file;
        dpSeparateDirFile(m_base_path.c_str(), &dir, &file);
        HMODULE parent = GetModuleHandleA(file.c_str());
        if(!parent) { return false; }
        m_parent = parent;
    }

    // ロード中の dll と関連する pdb はロックがかかってしまい、リビルドに失敗するようになるため、
    // 一時ファイルにコピーする処理をここで行う。
    // dll に含まれる .pdb のパスを書き換えてコピーしないといけない。

    // dll をメモリに map
    void *data = nullptr;
    size_t datasize = 0;
    if(!dpMapFile(path, data, datasize, malloc)) {
        return false;
    }

    // 一時ファイル名を算出
    char rev[8] = {0};
    for(int i=0; i<0xfff; ++i) {
        _snprintf(rev, _countof(rev), "%x", i);
        m_base_path = path;
        m_dll_path.clear();
        std::string ext;
        dpSeparateFileExt(path, &m_dll_path, &ext);
        m_dll_path+=rev;
        m_dll_path+=".";
        m_dll_path+=ext;
        if(!dpFileExists(m_dll_path.c_str())) { break; }
    }

    // pdb のパスを抽出。あればパスを書き換え、書き換え後のパスに元ファイルをコピー
    if(char *pdb=dpGetPDBPathFromModule(data, true)) {
        std::string before;
        before = pdb;
        strncpy(pdb+before.size()-3, rev, 3);
        m_pdb_path = pdb;
        dpCopyFile(before.c_str(), m_pdb_path.c_str());
    }
    // dll を一時ファイルにコピー
    dpWriteFile(m_dll_path.c_str(), data, datasize);
    free(data);

    m_module = ::LoadLibraryA(m_dll_path.c_str());
    if(m_module) {
        return true;
    }
    else {
        unload();
        return false;
    }
}

bool DLL::unload()
{
    bool ret = false;
    if(m_module) {
        ::FreeLibrary(m_module);
        m_module = nullptr;
        ret = true;
    }
    if(!m_dll_path.empty()) {
        dpDeleteFile(m_dll_path.c_str());
        m_dll_path.clear();
    }
    if(!m_pdb_path.empty()) {
        dpDeleteFile(m_pdb_path.c_str());
        m_pdb_path.clear();
    }
    m_base_path.clear();
    return ret;
}

void DLL::patch()
{
    dpEnumerateDLLExports(m_module, [&](const char *name, void *sym){
        Symbol s = {name, sym, ::GetProcAddress(m_parent, name)};
        // todo
        m_symbols.push_back(s);
    });
}

void DLL::unpatch()
{
    dpEach(m_symbols, [&](const Symbol &sym){
        // todo
    });
}



PatchLibraryImpl::PatchLibraryImpl()
{
}

PatchLibraryImpl::~PatchLibraryImpl()
{
    dpEach(m_dlls, [&](DLLCont::value_type &p){ delete p.second; });
    m_dlls.clear();
}

HMODULE PatchLibraryImpl::loadAndPatch( const char *path )
{
    std::string dir, file;
    dpSeparateDirFile(path, &dir, &file);
    dpEach(file, [](char &c){ c=::tolower(c); });

    DLL *&dll = m_dlls[file];
    if(dll) {
        delete dll;
        dll = nullptr;
    }
    dll = new DLL();
    dll->load(path);
    dll->patch();

    return dll->getHandle();
}

BOOL PatchLibraryImpl::unpatchAndUnload( HMODULE mod )
{
    auto i = dpFind(m_dlls, [&](DLLCont::value_type &p){ return p.second->getHandle()==mod; });
    if(i!=m_dlls.end()) {
        delete i->second;
        m_dlls.erase(i);
        return TRUE;
    }
    return FALSE;
}


PatchLibraryImpl g_impl;

} // namespace



HMODULE WINAPI PatchLibrary(const char *path)
{
    return g_impl.loadAndPatch(path);
}

BOOL WINAPI UnpatchLibrary(HMODULE mod)
{
    return g_impl.unpatchAndUnload(mod);
}
