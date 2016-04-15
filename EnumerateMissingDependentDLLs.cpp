#include <cstdio>

typedef void(*EnumerateDLLCallback)(const char *dllname);
void EnumerateDependentDLLs(const char *dllpath, EnumerateDLLCallback cb);
void EnumerateMissingDependentDLLs(const char *dllpath, EnumerateDLLCallback cb);


// test

void PrintDLLName(const char *dllname)
{
    printf("  %s\n", dllname);
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("usage: %s target_dll\n", argv[0]);
        return 0;
    }

    const char *dllpath = argv[1];
    printf("%s\n", dllpath);
    printf("all dependent dlls:\n");
    EnumerateDependentDLLs(dllpath, PrintDLLName);
    printf("missing dependent dlls:\n");
    EnumerateMissingDependentDLLs(dllpath, PrintDLLName);
}

// example output
// 
//3DGlasses.aex
//all dependent dlls:
//  boost_system.dll
//  M.dll
//  MSVCP120.dll
//  MSVCR120.dll
//  KERNEL32.dll
//missing dependent dlls:
//  boost_system.dll
//  M.dll



// implementation

#include <windows.h>
#include <cstdio>

namespace impl {
inline bool TryLoadModule(const char *dllname)
{
    HMODULE mod = ::GetModuleHandleA(dllname);
    if (mod) { return true; }

    mod = ::LoadLibraryA(dllname);
    // should FreeLibrary(mod) here?
    return mod != nullptr;
}

// F: [](size_t size) -> void* : alloc func (e.g.: malloc)
template<class F>
inline bool MapFile(const char *path, void *&o_data, size_t &o_size, const F &alloc)
{
    o_data = nullptr;
    o_size = 0;
    if (FILE *f = fopen(path, "rb")) {
        fseek(f, 0, SEEK_END);
        o_size = ftell(f);
        if (o_size > 0) {
            o_data = alloc(o_size);
            fseek(f, 0, SEEK_SET);
            fread(o_data, 1, o_size, f);
        }
        fclose(f);
        return true;
    }
    return false;
}

// for internal use
inline void DLLFillGap(size_t& ImageBase, DWORD RVA)
{
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)ImageBase;
    PIMAGE_NT_HEADERS pNtHeaders = (PIMAGE_NT_HEADERS)(ImageBase + pDosHeader->e_lfanew);

    PIMAGE_SECTION_HEADER pSectionHeader = IMAGE_FIRST_SECTION(pNtHeaders);
    for (size_t i = 0; i < pNtHeaders->FileHeader.NumberOfSections; ++i) {
        PIMAGE_SECTION_HEADER s = pSectionHeader + i;
        if (RVA >= s->VirtualAddress && RVA < s->VirtualAddress + s->SizeOfRawData) {
            pSectionHeader = s;
            break;
        }
    }

    DWORD gap = pSectionHeader->VirtualAddress - pSectionHeader->PointerToRawData;
    ImageBase -= gap;
}

// Body: [](const char *dllname) -> void
// fill_gap: should be true if module is directly memory mapped file (= not loaded by LoadModule())
template<class Body>
inline void EnumerateDependentDLLs(HMODULE module, const Body &body, bool fill_gap = false)
{
    if (module == nullptr) { return; }

    size_t ImageBase = (size_t)module;
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)ImageBase;
    if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE) { return; }

    PIMAGE_NT_HEADERS pNtHeaders = (PIMAGE_NT_HEADERS)(ImageBase + pDosHeader->e_lfanew);
    DWORD RVAImports = pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
    if (RVAImports == 0) { return; }

    if (fill_gap) {
        DLLFillGap(ImageBase, RVAImports);
    }

    IMAGE_IMPORT_DESCRIPTOR *pImportDesc = (IMAGE_IMPORT_DESCRIPTOR*)(ImageBase + RVAImports);
    while (pImportDesc->Name != 0) {
        const char *pDLLName = (const char*)(ImageBase + pImportDesc->Name);
        body(pDLLName);
        ++pImportDesc;
    }
    return;
}
} // namespace impl

void EnumerateDependentDLLs(const char *dllpath, EnumerateDLLCallback cb)
{
    void *mapped_dll;
    size_t mapped_size;
    if (!impl::MapFile(dllpath, mapped_dll, mapped_size, malloc)) {
        return;
    }
    impl::EnumerateDependentDLLs((HMODULE)mapped_dll, [&](const char *dllname) {
        cb(dllname);
    }, true);
    free(mapped_dll);
}

void EnumerateMissingDependentDLLs(const char *dllpath, EnumerateDLLCallback cb)
{
    void *mapped_dll;
    size_t mapped_size;
    if (!impl::MapFile(dllpath, mapped_dll, mapped_size, malloc)) {
        return;
    }
    impl::EnumerateDependentDLLs((HMODULE)mapped_dll, [&](const char *dllname) {
        if (!impl::TryLoadModule(dllname)) {
            cb(dllname);
        }
    }, true);
    free(mapped_dll);
}
