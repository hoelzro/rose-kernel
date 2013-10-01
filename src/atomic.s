; XXX license info
SECTION .text

GLOBAL atomic_compare_and_set8
GLOBAL atomic_compare_and_set16
GLOBAL atomic_compare_and_set32
GLOBAL atomic_compare_and_set64

GLOBAL atomic_incr8
GLOBAL atomic_incr16
GLOBAL atomic_incr32

atomic_compare_and_set8:
    PUSH EBP
    MOV  EBP, ESP

    MOV ECX, [EBP + 8]
    MOV EAX, [EBP + 12]
    MOV EDX, [EBP + 16]

    LOCK CMPXCHG [ECX], DL

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

    LOCK CMPXCHG [ECX], DX

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

    LOCK CMPXCHG [ECX], EDX

    SETE AL
    AND  EAX, 0xFF

    LEAVE
    RET

atomic_compare_and_set64:
    PUSH EBP
    MOV  EBP, ESP

    ; XXX um....
    MOV ESI, [EBP + 8]
    MOV EAX, [EBP + 12]
    MOV EDX, [EBP + 16]
    MOV EBX, [EBP + 20]
    MOV ECX, [EBP + 24]

    LOCK CMPXCHG8B [ESI]

    SETE AL
    AND  EAX, 0xFF

    LEAVE
    RET

atomic_incr8:
    PUSH EBP
    MOV  EBP, ESP

    MOV EAX, [ESP + 8]
    LOCK INC BYTE [EAX]
    MOV EAX, [EAX]

    LEAVE
    RET

atomic_incr16:
    PUSH EBP
    MOV  EBP, ESP

    MOV EAX, [ESP + 8]
    LOCK INC WORD [EAX]
    MOV EAX, [EAX]

    LEAVE
    RET

atomic_incr32:
    PUSH EBP
    MOV  EBP, ESP

    MOV EAX, [ESP + 8]
    LOCK INC DWORD [EAX]
    MOV EAX, [EAX]

    LEAVE
    RET
