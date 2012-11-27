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

GLOBAL io_outb
GLOBAL io_outw
GLOBAL io_outdw
GLOBAL io_inb
GLOBAL io_inw
GLOBAL io_indw

io_outb:
    MOV DX, [ESP + 4]
    MOV AL, [ESP + 8]

    OUT DX, AL

    RET

io_outw:
    MOV DX, [ESP + 4]
    MOV AX, [ESP + 8]

    OUT DX, AX

    RET

io_outdw:
    MOV DX,  [ESP + 4]
    MOV EAX, [ESP + 8]

    OUT DX, EAX

    RET

io_inb:
    MOV EAX, 0
    MOV DX, [ESP + 4]

    IN AL, DX

    RET

io_inw:
    MOV EAX, 0
    MOV DX, [ESP + 4]

    IN AX, DX

    RET

io_indw:
    MOV DX, [ESP + 4]

    IN EAX, DX

    RET
