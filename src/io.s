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
