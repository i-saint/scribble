.code
RPCCall PROC
    mov rax, rcx                    ; rax: RPCInfo

    mov r10, qword ptr [rax+48h] ; r10: num_args

    mov r11, rax    ; r11: pointer to ArgTypeInfo[]
    add r11, 50h

    mov r12, 10h
    imul r12, r10
    add r12, rax
    add r12, 50h ; r12: pointer to data
    xor r13, r13

    cmp r10, 0
    je Jump@RPCCall

    mov r13, qword ptr [r11+0] ; r13 arg type
    cmp r13, 1
    je Arg1PassFloat@RPCCall
    cmp r13, 2
    je Arg1PassObj@RPCCall
Arg1PassInt@RPCCall:
    mov rcx, qword ptr [r12]
    jmp Arg1End@RPCCall
Arg1PassFloat@RPCCall:
    movsd xmm0, mmword ptr [r12]
    jmp Arg1End@RPCCall
Arg1PassObj@RPCCall:
    mov rcx, r12
Arg1End@RPCCall:
    mov r13, qword ptr [r11+8] ; r13 arg size
    add r12, r13
    add r11, 10h

    cmp r10, 1
    je Jump@RPCCall

    mov r13, qword ptr [r11+0] ; r13 arg type
    cmp r13, 1
    je Arg2PassFloat@RPCCall
    cmp r13, 2
    je Arg2PassObj@RPCCall
Arg2PassInt@RPCCall:
    mov rdx, qword ptr [r12]
    jmp Arg2End@RPCCall
Arg2PassFloat@RPCCall:
    movsd xmm1, mmword ptr [r12]
    jmp Arg2End@RPCCall
Arg2PassObj@RPCCall:
    mov rdx, r12
Arg2End@RPCCall:
    mov r13, qword ptr [r11+8] ; r13 arg size
    add r12, r13
    add r11, 10h


    cmp r10, 2
    je Jump@RPCCall

    mov r13, qword ptr [r11+0] ; r13 arg type
    cmp r13, 1
    je Arg3PassFloat@RPCCall
    cmp r13, 2
    je Arg3PassObj@RPCCall
Arg3PassInt@RPCCall:
    mov r8, qword ptr [r12]
    jmp Arg3End@RPCCall
Arg3PassFloat@RPCCall:
    movsd xmm2, mmword ptr [r12]
    movd r8, xmm2
    jmp Arg3End@RPCCall
Arg3PassObj@RPCCall:
    mov r8, r12
Arg3End@RPCCall:
    mov r13, qword ptr [r11+8] ; r13 arg size
    add r12, r13
    add r11, 10h

    cmp r10, 3
    je Jump@RPCCall


    mov r13, qword ptr [r11+0] ; r13 arg type
    cmp r13, 1
    je Arg4PassFloat@RPCCall
    cmp r13, 2
    je Arg4PassObj@RPCCall
Arg4PassInt@RPCCall:
    mov r9, qword ptr [r12]
    jmp Arg4End@RPCCall
Arg4PassFloat@RPCCall:
    movsd xmm3, mmword ptr [r12]
    jmp Arg4End@RPCCall
Arg4PassObj@RPCCall:
    mov r9, r12
Arg4End@RPCCall:
    mov r13, qword ptr [r11+8] ; r13 arg size
    add r12, r13
    add r11, 10h


Jump@RPCCall:
    jmp qword ptr [rax+40h]
RPCCall ENDP
END
