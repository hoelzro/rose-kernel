; Copyright (c) 2012 Rob Hoelz <rob at hoelz.ro>
;
; Permission is hereby granted, free of charge, to any person obtaining a
; copy of this software and associated documentation files (the "Software"),
; to deal in the Software without restriction, including without limitation
; the rights to use, copy, modify, merge, publish, distribute, sublicense,
; and/or sell copies of the Software, and to permit persons to whom the
; Software is furnished to do so, subject to the following conditions:
;
; The above copyright notice and this permission notice shall be included in
; all copies or substantial portions of the Software.
;
; THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
; IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
; FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
; THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
; LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
; FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
; DEALINGS IN THE SOFTWARE.

; A lot of this code is based on http://www.jamesmolloy.co.uk/tutorial_html/2.-Genesis.html
MBOOT_PAGE_ALIGN    EQU 1<<0
MBOOT_MEM_INFO      EQU 1<<1
MBOOT_MAGIC         EQU 0x1BADB002
MBOOT_FLAGS         EQU MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO
MBOOT_CHECKSUM      EQU -(MBOOT_MAGIC + MBOOT_FLAGS)
CR0_PE              EQU 1<<0
CR0_PG              EQU 1<<31
SEGMENT_KERNEL_DATA EQU 0x10
SEGMENT_KERNEL_CODE EQU 0x08

BITS 32

GLOBAL mboot
EXTERN code
EXTERN bss
EXTERN end
EXTERN stack
GLOBAL start

SECTION .establish_higher_half

mboot:
    DD MBOOT_MAGIC
    DD MBOOT_FLAGS
    DD MBOOT_CHECKSUM
    DD mboot
    DD code
    DD bss
    DD end
    DD start

start:
    CLI

    ; preserve EAX and EBX; we'll need them in higher_half_start
    LGDT [fake_gdt_ptr]
    MOV ECX, SEGMENT_KERNEL_DATA
    MOV SS, ECX
    MOV DS, ECX
    MOV ES, ECX
    MOV FS, ECX
    MOV GS, ECX

    MOV ECX, kernel_page_table
    MOV EDX, 0x00000003 ; XXX magic number!
map_kernel_loop:
    MOV [ECX], EDX
    ADD ECX, 4
    ADD EDX, 0x1000
    CMP EDX, (end - 0xC0000000) ; XXX magic number
    JB map_kernel_loop

    MOV ECX, kernel_page_directory
    MOV EDX, kernel_page_table
    OR  EDX, 0x003
    MOV [ECX + (768 * 4)], EDX

    MOV EDX, kernel_page_table
    OR  EDX, 0x003
    MOV [ECX], EDX

    MOV EDX, kernel_page_directory
    OR  EDX, 0x003
    MOV [ECX + (1023 * 4)], EDX

    MOV CR3, ECX

    MOV ECX, CR0
    OR ECX, CR0_PE | CR0_PG
    MOV CR0, ECX

    JMP SEGMENT_KERNEL_CODE:higher_half_start

fake_gdt:
    DQ 0x0000000000000000
    DQ 0x00CF9A000000FFFF
    DQ 0x00CF92000000FFFF

fake_gdt_ptr:
    DW $ - fake_gdt - 1
    DD fake_gdt

ALIGN 0x1000
kernel_page_directory:
    TIMES 0x1000 DB 0

ALIGN 0x1000
kernel_page_table:
    TIMES 0x1000 DB 0

SECTION .text

GLOBAL higher_half_start
EXTERN kmain

higher_half_start:
    MOV ESP, stack
    ; XXX I should probably check that EAX == 0x2BADB002 here
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
