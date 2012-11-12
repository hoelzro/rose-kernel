GLOBAL gdt_set

gdt_set:
    MOV EAX, [ESP + 4]
    LGDT [EAX]

    MOV EAX, 0x10

    ; set up data segment selectors with kernel space data selector
    MOV DS, EAX
    MOV ES, EAX
    MOV FS, EAX
    MOV GS, EAX
    MOV SS, EAX

    JMP 0x08:flush ; perform far jump to set CS to kernel space code selector
flush:
    RET
