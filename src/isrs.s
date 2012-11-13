GLOBAL asm_general_isr

EXTERN general_isr

asm_general_isr:
    PUSHA ; preserve general registers

    ; set up data segments to kernel data segment
    MOV AX, DS
    PUSH EAX

    MOV AX, 0x10
    MOV DS, AX
    MOV ES, AX
    MOV FS, AX
    MOV GS, AX

    CALL general_isr

    ; restore original segment
    POP EAX
    MOV DS, AX
    MOV ES, AX
    MOV FS, AX
    MOV GS, AX

    POPA ; restore general registers

    ADD ESP, 8
    STI
    IRET

%MACRO GEN_ISR 1
GLOBAL isr%1
isr%1:
    CLI
    PUSH DWORD 0
    PUSH DWORD %1
    JMP asm_general_isr
%ENDMACRO

%MACRO GEN_ISR_WITH_ERR 1
GLOBAL isr%1
isr%1:
    CLI
    PUSH BYTE %1
    JMP asm_general_isr
%ENDMACRO

GEN_ISR 0
GEN_ISR 1
GEN_ISR 2
GEN_ISR 3
GEN_ISR 4
GEN_ISR 5
GEN_ISR 6
GEN_ISR 7
GEN_ISR_WITH_ERR 8
GEN_ISR 9
GEN_ISR_WITH_ERR 10
GEN_ISR 11
GEN_ISR 12
GEN_ISR 13
GEN_ISR_WITH_ERR 14
GEN_ISR 15
GEN_ISR 16
GEN_ISR 17
GEN_ISR 18
GEN_ISR 19
GEN_ISR 20
GEN_ISR 21
GEN_ISR 22
GEN_ISR 23
GEN_ISR 24
GEN_ISR 25
GEN_ISR 26
GEN_ISR 27
GEN_ISR 28
GEN_ISR 29
GEN_ISR 30
GEN_ISR 31
