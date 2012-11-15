GLOBAL gdt_set

gdt_set:
    MOV EAX, [ESP + 4]
    LGDT [EAX]
    RET
