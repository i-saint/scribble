#include <cstdio>
#include <windows.h>

// target: 関数ポインタ。対象関数を hotpatch して元の関数へのポインタを返す
void* Hotpatch( void *target, const void *replacement )
{
    DWORD old;
    BYTE *f = (BYTE*)target;
    void *orig_func = f+2;
    ::VirtualProtect(f-5, 7, PAGE_EXECUTE_READWRITE, &old);
    f[-5]=0xE9; // jmp
    *(DWORD*)(f-4) = (ptrdiff_t)replacement-(ptrdiff_t)f;
    f[0]=0xEB; f[1]=0xF9; // short jmp -7
    ::VirtualProtect(f-5, 7, old, &old);
    return orig_func;
}

void Hoge() { printf("Hoge()\n"); }
void Hook() { printf("Hook()\n"); }

int main(int argc, char *argv[])
{
    Hoge();
    Hotpatch(&Hoge, &Hook);
    Hoge();
    // 2 byte 先を call すれば hotpatch 前の関数を呼べる
    ((void (*)())((size_t)&Hoge+2)) ();
}

/*
$ cl Hotpatch.cpp /hotpatch && ./Hotpatch

Hoge()
Hook()
Hoge()
*/
