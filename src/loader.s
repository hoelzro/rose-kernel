MBOOT_PAGE_ALIGN    EQU 1<<0
MBOOT_MEM_INFO      EQU 1<<1
MBOOT_MAGIC         EQU 0x1BADB002
MBOOT_FLAGS         EQU MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO
MBOOT_CHECKSUM      EQU -(MBOOT_MAGIC + MBOOT_FLAGS)
CR0_PE              EQU 1<<0
SEGMENT_KERNEL_DATA EQU 0x10
SEGMENT_KERNEL_CODE EQU 0x08

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
    MOV EAX, SEGMENT_KERNEL_DATA

    MOV DS, EAX
    MOV ES, EAX
    MOV FS, EAX
    MOV GS, EAX
    MOV SS, EAX

    MOV EAX, CR0
    OR AL, CR0_PE
    MOV CR0, EAX

    JMP SEGMENT_KERNEL_CODE:flush ; perform far jump to set CS to kernel space code selector

flush:
    RET
