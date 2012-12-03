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

SEGMENT_KERNEL_DATA EQU 0x10
SEGMENT_KERNEL_CODE EQU 0x08

GLOBAL asm_general_isr

EXTERN _general_isr

; This is based off of code from http://www.jamesmolloy.co.uk/tutorial_html/4.-The%20GDT%20and%20IDT.html
asm_general_isr:
    PUSHA ; preserve general registers

    ; set up data segments to kernel data segment
    MOV AX, DS
    PUSH EAX

    MOV AX, SEGMENT_KERNEL_DATA
    MOV DS, AX
    MOV ES, AX
    MOV FS, AX
    MOV GS, AX

    CALL _general_isr

    ; restore original segment
    POP EAX
    MOV DS, AX
    MOV ES, AX
    MOV FS, AX
    MOV GS, AX

    POPA ; restore general registers

    ADD ESP, 8
    STI
    IRET

%MACRO GEN_ISR 1
GLOBAL isr%1
isr%1:
    CLI
    PUSH DWORD 0
    PUSH DWORD %1
    JMP asm_general_isr
%ENDMACRO

%MACRO GEN_ISR_WITH_ERR 1
GLOBAL isr%1
isr%1:
    CLI
    PUSH BYTE %1
    JMP asm_general_isr
%ENDMACRO

GEN_ISR 0
GEN_ISR 1
GEN_ISR 2
GEN_ISR 3
GEN_ISR 4
GEN_ISR 5
GEN_ISR 6
GEN_ISR 7
GEN_ISR_WITH_ERR 8
GEN_ISR 9
GEN_ISR_WITH_ERR 10
GEN_ISR_WITH_ERR 11
GEN_ISR_WITH_ERR 12
GEN_ISR_WITH_ERR 13
GEN_ISR_WITH_ERR 14
GEN_ISR 15
GEN_ISR 16
GEN_ISR 17
GEN_ISR 18
GEN_ISR 19
GEN_ISR 20
GEN_ISR 21
GEN_ISR 22
GEN_ISR 23
GEN_ISR 24
GEN_ISR 25
GEN_ISR 26
GEN_ISR 27
GEN_ISR 28
GEN_ISR 29
GEN_ISR 30
GEN_ISR 31
