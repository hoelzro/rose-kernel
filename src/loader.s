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
SEGMENT_KERNEL_DATA EQU 0x10
SEGMENT_KERNEL_CODE EQU 0x08

BITS 32

GLOBAL mboot
EXTERN code
EXTERN bss
EXTERN end
EXTERN stack

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
