SECTION .text

GLOBAL atomic_compare_and_set8
GLOBAL atomic_compare_and_set16
GLOBAL atomic_compare_and_set32
GLOBAL atomic_compare_and_set64

atomic_compare_and_set8:
    PUSH EBP
    MOV  EBP, ESP

    MOV ECX, [EBP + 8]
    MOV EAX, [EBP + 12]
    MOV EDX, [EBP + 16]

    CMPXCHG [ECX], DL

    SETE AL
    AND  EAX, 0xFF

    LEAVE
    RET

atomic_compare_and_set16:
    PUSH EBP
    MOV  EBP, ESP

    MOV ECX, [EBP + 8]
    MOV EAX, [EBP + 12]
    MOV EDX, [EBP + 16]

    CMPXCHG [ECX], DX

    SETE AL
    AND  EAX, 0xFF

    LEAVE
    RET

atomic_compare_and_set32:
    PUSH EBP
    MOV  EBP, ESP

    MOV ECX, [EBP + 8]
    MOV EAX, [EBP + 12]
    MOV EDX, [EBP + 16]

    CMPXCHG [ECX], EDX

    SETE AL
    AND  EAX, 0xFF

    LEAVE
    RET

atomic_compare_and_set64:
    PUSH EBP
    MOV  EBP, ESP

    MOV ESI, [EBP + 8]
    MOV EAX, [EBP + 12]
    MOV EDX, [EBP + 16]
    MOV EBX, [EBP + 20]
    MOV ECX, [EBP + 24]

    CMPXCHG8B [ESI]

    SETE AL
    AND  EAX, 0xFF

    LEAVE
    RET
