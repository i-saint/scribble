.code
RPCCall PROC
    mov rax, rcx
    mov rcx, qword ptr [rax +  08h]
    mov rdx, qword ptr [rax + 010h]
    mov r8,  qword ptr [rax + 018h]
    mov r9,  qword ptr [rax + 018h]
    jmp qword ptr [rax + 00h]
RPCCall ENDP
END
