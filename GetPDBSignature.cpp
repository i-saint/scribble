#include <windows.h>
#include <string>
#include <vector>


struct PDBStream70
{
    DWORD impv;
    DWORD sig;
    DWORD age;
    GUID sig70;
};

PDBStream70* GetPDBSignature(void *mapped_pdb_file)
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
    for(int i=0; i<RootPointersPages; i++) {
        PVOID Page = PAGE(pHeader->dwRootPointers[i]);
        SIZE_T Offset = pHeader->dwPageSize * i;
        memcpy(&RootPointersRaw[0]+Offset, Page, pHeader->dwPageSize);
    }
    DWORD *RootPointers = (DWORD*)&RootPointersRaw[0];

    std::string StreamInfoRaw;
    StreamInfoRaw.resize(RootPages * pHeader->dwPageSize);
    for(int i=0; i<RootPages; i++) {
        PVOID Page = PAGE(RootPointers[i]);
        SIZE_T Offset = pHeader->dwPageSize * i;
        memcpy(&StreamInfoRaw[0]+Offset, Page, pHeader->dwPageSize);
    }
    DWORD StreamCount = *(DWORD*)&StreamInfoRaw[0];
    DWORD *dwStreamSizes = (DWORD*)&StreamInfoRaw[4];

    {
        DWORD StreamSize = dwStreamSizes[PDB_STREAM_PDB];
        DWORD StreamPages = STREAM_SPAN_PAGES(StreamSize);
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


// F: [](size_t size) -> void* : alloc func
template<class F>
inline bool MapFile(const char *path, void *&o_data, size_t &o_size, const F &alloc)
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


int main(int argc, char *argv[])
{
    size_t pdbSize = 0;
    void *pdbData = nullptr;
    MapFile("GetPDBSignature.pdb", pdbData, pdbSize, malloc);

    PDBStream70 *sig = GetPDBSignature(pdbData);
    GUID &guid = sig->sig70;
    printf("{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
        guid.Data1, guid.Data2, guid.Data3,
        guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);

    free(pdbData);
}

// $ cl GetPDBSignature.cpp /Zi /EHsc && ./GetPDBSignature
// {7AD8596A-B1C6-42D3-9002-869FE824B9F2}
// 
// $ dumpbin GetPDBSignature.exe /headers | grep pdb
// 521F2723 cv           3C 000374C0    360C0    Format: RSDS, {7AD8596A-B1C6-42D3-9002-869FE824B9F2}, 1, D:\src\scribble\GetPDBSignature.pdb
