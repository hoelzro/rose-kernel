MBOOT_PAGE_ALIGN EQU 1<<0
MBOOT_MEM_INFO   EQU 1<<1
MBOOT_MAGIC      EQU 0x1BADB002
MBOOT_FLAGS      EQU MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO
MBOOT_CHECKSUM   EQU -(MBOOT_MAGIC + MBOOT_FLAGS)

BITS 32

GLOBAL mboot
EXTERN code
EXTERN bss
EXTERN end

mboot:
    DD MBOOT_MAGIC
    DD MBOOT_FLAGS
    DD MBOOT_CHECKSUM
    DD mboot
    DD code
    DD bss
    DD end
    DD start

GLOBAL start
EXTERN kmain

start:
    PUSH EBX
    CALL kmain
_hang:
    HLT
    JMP _hang

GLOBAL protected_mode_start

protected_mode_start:
    MOV EAX, 0x10

    ; set up data segment selectors with kernel space data selector
    MOV DS, EAX
    MOV ES, EAX
    MOV FS, EAX
    MOV GS, EAX
    MOV SS, EAX

    ; turn on protected mode
    MOV EAX, CR0
    OR AL, 1
    MOV CR0, EAX

    JMP 0x08:flush ; perform far jump to set CS to kernel space code selector

flush:
    RET
