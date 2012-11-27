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

FEATURE_FLAGS          EQU 0x01
EXTENDED_FEATURE_FLAGS EQU 0x80000001

FPU_PRESENT      EQU 1<<0
PSE_PRESENT      EQU 1<<3
PAE_PRESENT      EQU 1<<6
APIC_PRESENT     EQU 1<<9
LONGMODE_ENABLED EQU 1<<29

; XXX verify that the CPU supports CPUID!

; name, EAX_VALUE, REG, BIT
%MACRO GEN_CAPS 4
GLOBAL %1
%1:
    PUSH EBX
    PUSH ECX
    PUSH EDX

    MOV EAX, %2
    CPUID

    MOV EAX, 0
    AND %3, %4
    SETNZ AL

    POP EDX
    POP ECX
    POP EBX

    RET
%ENDMACRO

GEN_CAPS capabilities_is_64bit, EXTENDED_FEATURE_FLAGS, EDX, LONGMODE_ENABLED
GEN_CAPS capabilities_has_apic, FEATURE_FLAGS, EDX, APIC_PRESENT
GEN_CAPS capabilities_has_pae, FEATURE_FLAGS, EDX, PAE_PRESENT
GEN_CAPS capabilities_has_pse, FEATURE_FLAGS, EDX, PSE_PRESENT
GEN_CAPS capabilities_has_fpu, FEATURE_FLAGS, EDX, FPU_PRESENT
