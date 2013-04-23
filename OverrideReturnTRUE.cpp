#include <windows.h>
#include <cstdio>


// 4 byte の整数を返す関数の return 1 の箇所を任意の関数への call に書き換える。(WinAPI によくある return TRUE などが該当)
// 
// 4 byte 整数の return 1 は 5 byte の命令 (mov eax, 1 == B8 01 00 00 00) になるため、任意の関数への call に書き換えられる。
// 1 以外にも適用可能だが残念ながら 0 だけは不可能。(xor eax, eax になってしまい、5 byte より小さくなるため)
// ちなみに組み込み型の bool は 1 byte なため、return true は対応不可能。
// また、x64 でも基本的には動くものの、相対アドレスが DWORD に収まらない場合はクラッシュするので注意が必要。
void OverrideReturnTRUE(void *addr, size_t scan_len, void *jmp_dst)
{
    // /link /opt:ref を付けなかった場合、関数呼び出しは関数の実体への jmp を挟む。
    // 書き換えたいのは jmp 先なのでそれを辿る。
    {
        BYTE *data = (BYTE*)addr;
        if(data[0]==0xE9) { // jmp
            addr = data + *(DWORD*)(data+1) + 5;
        }
    }
    {
        BYTE *data = (BYTE*)jmp_dst;
        if(data[0]==0xE9) {
            jmp_dst = data + *(DWORD*)(data+1) + 5;
        }
    }

    DWORD old;
    ::VirtualProtect(addr, scan_len, PAGE_EXECUTE_READWRITE, &old);
    for(size_t i=0; i<scan_len; ++i) {
        BYTE *data = (BYTE*)addr + i;
         // return TRUE; (mov eax, 1 == B8 01 00 00 00) を探して
         // call jmp_dst (E8 [4byte RVA]) に書き換える
        if( data[0]==0xB8 && *(int*)(data+1)==1 )
        {
            data = (BYTE*)addr + i;
            data[0] = 0xE8; // call
            *(DWORD*)(data+1) = (ptrdiff_t)jmp_dst-(ptrdiff_t)data - 5;
            break;
        }
    }
    ::VirtualProtect(addr, scan_len, old, &old);
    ::FlushInstructionCache(::GetCurrentProcess(), addr, scan_len);
}


__declspec(noinline) BOOL Hoge()
{
    printf("Hoge() ");
    return TRUE;
}
 
__declspec(noinline) BOOL Hook()
{
    printf("Hook() ");
    return FALSE;
}

int main(int argc, char *argv[])
{
    printf("ret: %d\n", Hoge());

    OverrideReturnTRUE(&Hoge, 64, &Hook);

    printf("ret: %d\n", Hoge());
}


// $ cl OverrideReturnTRUE.cpp && ./OverrideReturnTRUE
// Hoge() ret: 1
// Hoge() Hook() ret: 0
