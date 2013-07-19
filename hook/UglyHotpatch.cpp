// "proper" hotpatch example:
// https://github.com/i-saint/scribble/blob/master/Hotpatch.cpp

#include <cstdio>
#include <windows.h>

// 
size_t CopyInstructions(void *dst_, const void *src_, size_t required)
{
    // 不完全につき、未対応の instruction があれば適宜追加すべし
    // 関数の頭 5 byte 以内で実行されるものは多くが mov,sub,push あたりなのでこれだけでも多くに対応はできるハズ
    size_t ret = 0;
    const BYTE *src = (const BYTE*)src_;
    BYTE *dst = (BYTE*)dst_;

    for(; ret<required; ) {
        int size = 0; // instruction size
        bool can_memcpy = true;

        switch(src[ret]) {
        // push
        case 0x55: size=1; break;
        case 0x68:
        case 0x6A: size=5; break;
        case 0xFF:
            switch(src[ret+1]) {
            case 0x74: size=4; break;
            default:   size=3; break;
            }

        // mov
        case 0x8B:
            switch(src[ret+1]) {
            case 0x44: size=4; break;
            case 0x45: size=3; break;
            default:   size=2; break;
            }
            break;
        case 0xB8: size=5; break;

        // sub
        case 0x81: 
            switch(src[ret+1]) {
            case 0xEC: size=6; break;
            default:   size=2; break;
            }
            break;
        case 0x83:
            switch(src[ret+1]) {
            case 0xEC: size=3; break;
            default:   size=2; break;
            }
            break;

        // call & jmp
        case 0xE8:
        case 0xE9:
            {
                can_memcpy = false;
                int rva = *(int*)(src+1);
                dst[ret] = src[ret];
                *(DWORD*)(dst+ret+1) = (ptrdiff_t)(src+ret+rva)-(ptrdiff_t)(dst+ret);
                ret += 5;
            }
            break;

        default: size=1; break;
        }

        if(can_memcpy) {
            memcpy(dst+ret, src+ret, size);
        }
        ret += size;
    }

    return ret;
}

// target: 関数ポインタ。対象関数を上書きしつつ元のコードは退避して、元の関数へのポインタを返す
void* UglyHotpatch( void *target, const void *replacement )
{
    // 元コードの退避先
    BYTE *preserved = (BYTE*)::VirtualAlloc(NULL, 64, MEM_COMMIT|MEM_RESERVE, PAGE_EXECUTE_READWRITE);

    BYTE *f = (BYTE*)target;
    DWORD old;
    ::VirtualProtect(f, 32, PAGE_EXECUTE_READWRITE, &old);

    // 元のコードをコピー & 最後にコピー本へ jmp するコードを付加 (==これを call すれば上書き前の動作をするハズ)
    size_t slice = CopyInstructions(preserved, f, 5);
    preserved[slice]=0xE9; // jmp
    *(DWORD*)(preserved+slice+1) = (ptrdiff_t)(f+slice)-(ptrdiff_t)(preserved+slice)-5;

    // 関数の先頭を hook 関数への jmp に書き換える
    f[0]=0xE9; // jmp
    *(DWORD*)(f+1) = (ptrdiff_t)replacement-(ptrdiff_t)f - 5;
    ::VirtualProtect(f, 32, old, &old);

    return preserved;
}


__declspec(noinline) void Hoge(int arg) { printf("Hoge(%d)\n", arg); }
__declspec(noinline) void Hook(int arg) { printf("Hook(%d)\n", arg); }

int main(int argc, char *argv[])
{
    typedef void (*F)(int);
    Hoge(1);
    F before = (F)UglyHotpatch(&Hoge, &Hook);
    Hoge(2);
    before(3);
}

/*
$ cl UglyHotpatch.cpp && ./UglyHotpatch

Hoge(1)
Hook(2)
Hoge(3)
*/
