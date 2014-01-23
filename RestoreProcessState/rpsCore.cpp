#include "rpsInternal.h"

bool rpsIsValidMemory(void *p)
{
    if(p==nullptr) { return false; }
    MEMORY_BASIC_INFORMATION meminfo;
    return ::VirtualQuery(p, &meminfo, sizeof(meminfo))!=0 && meminfo.State!=MEM_FREE;
}

BYTE* rpsAddJumpInstruction(BYTE* from, const BYTE* to)
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

void* rpsOverrideDLLExport(HMODULE module, const char *funcname, void *hook_, void *trampoline_space)
{
    if(!rpsIsValidMemory(module)) { return nullptr; }

    HANDLE proc = ::GetCurrentProcess();

    size_t ImageBase = (size_t)module;
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)ImageBase;
    if(pDosHeader->e_magic!=IMAGE_DOS_SIGNATURE) { return nullptr; }

    PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)(ImageBase + pDosHeader->e_lfanew);
    DWORD RVAExports = pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
    if(RVAExports==0) { return nullptr; }

    IMAGE_EXPORT_DIRECTORY *pExportDirectory = (IMAGE_EXPORT_DIRECTORY *)(ImageBase + RVAExports);
    DWORD *RVANames = (DWORD*)(ImageBase+pExportDirectory->AddressOfNames);
    WORD *RVANameOrdinals = (WORD*)(ImageBase+pExportDirectory->AddressOfNameOrdinals);
    DWORD *RVAFunctions = (DWORD*)(ImageBase+pExportDirectory->AddressOfFunctions);
    for(DWORD i=0; i<pExportDirectory->NumberOfFunctions; ++i) {
        char *pName = (char*)(ImageBase+RVANames[i]);
        if(strcmp(pName, funcname)==0) {
            BYTE *hook = (BYTE*)hook_;
            BYTE *target = (BYTE*)(ImageBase+RVAFunctions[RVANameOrdinals[i]]);
            if(trampoline_space) {
                BYTE *trampoline = (BYTE*)trampoline_space;
                rpsAddJumpInstruction(trampoline, hook);
                ::FlushInstructionCache(proc, trampoline, 32);
                rpsForceWrite<DWORD>(RVAFunctions[RVANameOrdinals[i]], (DWORD)(trampoline - ImageBase));
            }
            else {
                rpsForceWrite<DWORD>(RVAFunctions[RVANameOrdinals[i]], (DWORD)(hook - ImageBase));
            }
            return target;
        }
    }
    return nullptr;
}
