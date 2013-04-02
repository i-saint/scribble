public atomic_inc8
public atomic_inc16
public atomic_inc32

public atomic_dec8
public atomic_dec16
public atomic_dec32

public atomic_add8
public atomic_add16
public atomic_add32

public atomic_sub8
public atomic_sub16
public atomic_sub32

public atomic_swap8
public atomic_swap16
public atomic_swap32

public atomic_cas8
public atomic_cas16
public atomic_cas32

.code

atomic_inc8:
    mov dl, 1
    lock xadd byte ptr [rcx], dl
    mov al, dl
    ret

atomic_inc16:
    mov dx, 1
    lock xadd word ptr [rcx], dx
    mov ax, dx
    ret

atomic_inc32:
    mov edx, 1
    lock xadd dword ptr [rcx], edx
    mov eax, edx
    ret


atomic_dec8:
    mov dl, -1
    lock xadd byte ptr [rcx], dl
    mov al, dl
    ret

atomic_dec16:
    mov dx, -1
    lock xadd word ptr [rcx], dx
    mov ax, dx
    ret

atomic_dec32:
    mov edx, -1
    lock xadd dword ptr [rcx], edx
    mov eax, edx
    ret


atomic_add8:
    lock xadd byte ptr [rcx], dl
    mov al, dl
    ret

atomic_add16:
    lock xadd word ptr [rcx], dx
    mov ax, dx
    ret

atomic_add32:
    lock xadd dword ptr [rcx], edx
    mov eax, edx
    ret



atomic_sub8:
    xor al, al
    sub al, dl
    lock xadd byte ptr [rcx], al
    ret

atomic_sub16:
    xor ax, ax
    sub ax, dx
    lock xadd word ptr [rcx], ax
    ret

atomic_sub32:
    xor eax, eax
    sub eax, edx
    lock xadd dword ptr [rcx], eax
    ret


atomic_swap8:
    lock xchg byte ptr [rcx], dl
    mov al, dl
    ret

atomic_swap16:
    lock xchg word ptr [rcx], dx
    mov ax, dx
    ret

atomic_swap32:
    lock xchg dword ptr [rcx], edx
    mov eax, edx
    ret


atomic_cas8:
    mov al, dl
    lock cmpxchg byte ptr [rcx], r8b
    ret

atomic_cas16:
    mov ax, dx
    lock cmpxchg word ptr [rcx], r8w
    ret

atomic_cas32:
    mov eax, edx
    lock cmpxchg dword ptr [rcx], r8d
    ret

end
