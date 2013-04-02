public atomic_add8
public atomic_add16
public atomic_add32
public atomic_add64

.code
atomic_add8:
	lock xadd byte ptr [rcx], dl
	mov rax, rdx
	ret

atomic_add16:
	lock xadd word ptr [rcx], dx
	mov rax, rdx
	ret

atomic_add32:
	lock xadd dword ptr [rcx], edx
	mov rax, rdx
	ret

atomic_add64:
	lock xadd qword ptr [rcx], rdx
	mov rax, rdx
	ret
end