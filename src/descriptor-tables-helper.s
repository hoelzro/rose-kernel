GLOBAL gdt_set
GLOBAL idt_set

gdt_set:
    MOV EAX, [ESP + 4]
    LGDT [EAX]
    RET

idt_set:
    MOV EAX, [ESP + 4]
    LIDT [EAX]
    RET
