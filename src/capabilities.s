FEATURE_FLAGS          EQU 0x01
EXTENDED_FEATURE_FLAGS EQU 0x80000001

APIC_PRESENT     EQU 1<<9
LONGMODE_ENABLED EQU 1<<29

GLOBAL capabilities_is_64bit
GLOBAL capabilities_has_apic

; XXX verify that the CPU supports CPUID!

capabilities_is_64bit:
    PUSH EBX
    PUSH ECX
    PUSH EDX

    MOV EAX, EXTENDED_FEATURE_FLAGS
    CPUID

    MOV EAX, 0
    AND EDX, LONGMODE_ENABLED
    SETNZ AL

    POP EDX
    POP ECX
    POP EBX

    RET

capabilities_has_apic:
    PUSH EBX
    PUSH ECX
    PUSH EDX

    MOV EAX, FEATURE_FLAGS
    CPUID

    MOV EAX, 0
    AND EDX, APIC_PRESENT
    SETNZ AL

    POP EDX
    POP ECX
    POP EBX

    RET
