#include "plInternal.h"


// 位置指定版 VirtualAlloc()
// location より大きいアドレスの最寄りの位置にメモリを確保する。
void* plAllocateForward(size_t size, void *location)
{
    if(size==0) { return NULL; }
    static size_t base = (size_t)location;

    // ドキュメントには、アドレス指定の VirtualAlloc() は指定先が既に予約されている場合最寄りの領域を返す、
    // と書いてるように見えるが、実際には NULL が返ってくるようにしか見えない。
    // なので成功するまでアドレスを進めつつリトライ…。
    void *ret = NULL;
    const size_t step = 0x10000; // 64kb
    for(size_t i=0; ret==NULL; ++i) {
        ret = ::VirtualAlloc((void*)((size_t)base+(step*i)), size, MEM_COMMIT|MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    }
    return ret;
}

// 位置指定版 VirtualAlloc()
// location より小さいアドレスの最寄りの位置にメモリを確保する。
void* plAllocateBackward(size_t size, void *location)
{
    if(size==0) { return NULL; }
    static size_t base = (size_t)location;

    void *ret = NULL;
    const size_t step = 0x10000; // 64kb
    for(size_t i=0; ret==NULL; ++i) {
        ret = ::VirtualAlloc((void*)((size_t)base-(step*i)), size, MEM_COMMIT|MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    }
    return ret;
}

void plDeallocate(void *location, size_t size)
{
    ::VirtualFree(location, size, MEM_RELEASE);
}

bool plCopyFile(const char *srcpath, const char *dstpath)
{
    return ::CopyFileA(srcpath, dstpath, FALSE)==TRUE;
}

bool plWriteFile(const char *path, const void *data, size_t size)
{
    if(FILE *f=fopen(path, "wb")) {
        fwrite((const char*)data, 1, size, f);
        fclose(f);
        return true;
    }
    return false;
}

bool plDeleteFile(const char *path)
{
    return ::DeleteFileA(path)==TRUE;
}

bool plFileExists( const char *path )
{
    return ::GetFileAttributesA(path)!=INVALID_FILE_ATTRIBUTES;
}

size_t plSeparateDirFile(const char *path, std::string *dir, std::string *file)
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

size_t plSeparateFileExt(const char *filename, std::string *file, std::string *ext)
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

class plTrampolineAllocator::Page
{
public:
    struct Block {
        union {
            char data[block_size];
            Block *next;
        };
    };
    Page(void *base);
    ~Page();
    void* allocate();
    bool deallocate(void *v);
    bool isInsideMemory(void *p) const;
    bool isInsideJumpRange(void *p) const;

private:
    void *m_data;
    Block *m_freelist;
};

plTrampolineAllocator::Page::Page(void *base)
    : m_data(nullptr), m_freelist(nullptr)
{
    m_data = plAllocateBackward(page_size, base);
    m_freelist = (Block*)m_data;
    size_t n = page_size / block_size;
    for(size_t i=0; i<n-1; ++i) {
        m_freelist[i].next = m_freelist+i+1;
    }
    m_freelist[n-1].next = nullptr;
}

plTrampolineAllocator::Page::~Page()
{
    plDeallocate(m_data, page_size);
}

void* plTrampolineAllocator::Page::allocate()
{
    void *ret = nullptr;
    if(m_freelist) {
        ret = m_freelist;
        m_freelist = m_freelist->next;
    }
    return ret;
}

bool plTrampolineAllocator::Page::deallocate(void *v)
{
    if(v==nullptr) { return false; }
    bool ret = false;
    if(isInsideMemory(v)) {
        Block *b = (Block*)v;
        b->next = m_freelist;
        m_freelist = b;
        ret = true;
    }
    return ret;
}

bool plTrampolineAllocator::Page::isInsideMemory(void *p) const
{
    size_t loc = (size_t)p;
    size_t base = (size_t)m_data;
    return loc>=base && loc<base+page_size;
}

bool plTrampolineAllocator::Page::isInsideJumpRange( void *p ) const
{
    size_t loc = (size_t)p;
    size_t base = (size_t)m_data;
    size_t dist = base<loc ? loc-base : base-loc;
    return dist < 0x7fff0000;
}

plTrampolineAllocator::plTrampolineAllocator()
{
}

plTrampolineAllocator::~plTrampolineAllocator()
{
    plEach(m_pages, [](Page *p){ delete p; });
    m_pages.clear();
}

void* plTrampolineAllocator::allocate(void *location)
{
    void *ret = nullptr;
    if(Page *page=findCandidatePage(location)) {
        ret = page->allocate();
    }
    if(!ret) {
        Page *page = createPage(location);
        ret = page->allocate();
    }
    return ret;
}

bool plTrampolineAllocator::deallocate(void *v)
{
    if(Page *page=findOwnerPage(v)) {
        return page->deallocate(v);
    }
    return false;
}

plTrampolineAllocator::Page* plTrampolineAllocator::createPage(void *location)
{
    Page *p = new Page(location);
    m_pages.push_back(p);
    return p;
}

plTrampolineAllocator::Page* plTrampolineAllocator::findOwnerPage(void *location)
{
    auto p = plFind(m_pages, [=](const Page *p){ return p->isInsideMemory(location); });
    return p==m_pages.end() ? nullptr : *p;
}

plTrampolineAllocator::Page* plTrampolineAllocator::findCandidatePage(void *location)
{
    auto p = plFind(m_pages, [=](const Page *p){ return p->isInsideJumpRange(location); });
    return p==m_pages.end() ? nullptr : *p;
}

BYTE* plAddJumpInstruction(BYTE* from, BYTE* to)
{
    // 距離が 32bit に収まる範囲であれば、0xe9 RVA
    // そうでない場合、0xff 0x25 [メモリアドレス] + 対象アドレス
    // の形式で jmp する必要がある。
    BYTE* jump_from = from + 5;
    size_t distance = jump_from > to ? jump_from - to : to - jump_from;
    if (distance <= 0x7fff0000) {
        from[0] = 0xe9;
        from += 1;
        *((DWORD*)from) = (DWORD)(to - jump_from);
        from += 4;
    }
    else {
        from[0] = 0xff;
        from[1] = 0x25;
        from += 2;
#ifdef _M_IX86
        *((DWORD*)from) = (DWORD)(from + 4);
#elif defined(_M_X64)
        *((DWORD*)from) = (DWORD)0;
#endif
        from += 4;
        *((DWORD_PTR*)from) = (DWORD_PTR)(to);
        from += 8;
    }
    return from;
}


// fill_gap: .dll ファイルをそのままメモリに移した場合はこれを true にする必要があります。
// LoadLibrary() で正しくロードしたものは section の再配置が行われ、元ファイルとはデータの配置にズレが生じます。
// fill_gap==true の場合このズレを補正します。
CV_INFO_PDB70* plGetPDBInfoFromModule(void *pModule, bool fill_gap)
{
    if(!pModule) { return nullptr; }

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
                return pCVI;
            }
        }
    }
    return nullptr;
}

// pdb ファイルから Age & GUID 情報を抽出します
PDBStream70* plGetPDBSignature(void *mapped_pdb_file)
{
    // thanks to https://code.google.com/p/pdbparser/

#define ALIGN_UP(x, align)      ((x+align-1) & ~(align-1))
#define STREAM_SPAN_PAGES(size) (ALIGN_UP(size,pHeader->dwPageSize)/pHeader->dwPageSize)
#define PAGE(x)                 (pImageBase + pHeader->dwPageSize*(x))
#define PDB_STREAM_PDB    1

    struct MSF_Header
    {
        char szMagic[32];          // 0x00  Signature
        DWORD dwPageSize;          // 0x20  Number of bytes in the pages (i.e. 0x400)
        DWORD dwFpmPage;           // 0x24  FPM (free page map) page (i.e. 0x2)
        DWORD dwPageCount;         // 0x28  Page count (i.e. 0x1973)
        DWORD dwRootSize;          // 0x2c  Size of stream directory (in bytes; i.e. 0x6540)
        DWORD dwReserved;          // 0x30  Always zero.
        DWORD dwRootPointers[0x49];// 0x34  Array of pointers to root pointers stream. 
    };

    BYTE *pImageBase = (BYTE*)mapped_pdb_file;
    MSF_Header *pHeader = (MSF_Header*)pImageBase;

    DWORD RootPages = STREAM_SPAN_PAGES(pHeader->dwRootSize);
    DWORD RootPointersPages = STREAM_SPAN_PAGES(RootPages*sizeof(DWORD));

    std::string RootPointersRaw;
    RootPointersRaw.resize(RootPointersPages * pHeader->dwPageSize);
    for(DWORD i=0; i<RootPointersPages; i++) {
        PVOID Page = PAGE(pHeader->dwRootPointers[i]);
        SIZE_T Offset = pHeader->dwPageSize * i;
        memcpy(&RootPointersRaw[0]+Offset, Page, pHeader->dwPageSize);
    }
    DWORD *RootPointers = (DWORD*)&RootPointersRaw[0];

    std::string StreamInfoRaw;
    StreamInfoRaw.resize(RootPages * pHeader->dwPageSize);
    for(DWORD i=0; i<RootPages; i++) {
        PVOID Page = PAGE(RootPointers[i]);
        SIZE_T Offset = pHeader->dwPageSize * i;
        memcpy(&StreamInfoRaw[0]+Offset, Page, pHeader->dwPageSize);
    }
    DWORD StreamCount = *(DWORD*)&StreamInfoRaw[0];
    DWORD *dwStreamSizes = (DWORD*)&StreamInfoRaw[4];

    {
        DWORD *StreamPointers = &dwStreamSizes[StreamCount];
        DWORD page = 0;
        for(DWORD i=0; i<PDB_STREAM_PDB; i++) {
            DWORD nPages = STREAM_SPAN_PAGES(dwStreamSizes[i]);
            page += nPages;
        }
        DWORD *pdwStreamPointers = &StreamPointers[page];

        PVOID Page = PAGE(pdwStreamPointers[0]);
        return (PDBStream70*)Page;
    }

#undef PDB_STREAM_PDB
#undef PAGE
#undef STREAM_SPAN_PAGES
#undef ALIGN_UP
}
