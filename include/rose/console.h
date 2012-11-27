#ifndef ROSE_CONSOLE_H
#define ROSE_CONSOLE_H

#include <rose/stdarg.h>

int console_write_char(char c);
int console_write_string(const char *s);
int console_write_integer(int radix, int n);
int console_printf(const char *fmt, ...);

struct registers;
void _console_dump_registers(struct registers *regs);

#define console_dump_registers()\
    asm("PUSHAD");\
    asm("PUSH ESP");\
    asm("CALL _console_dump_registers");\
    asm("ADD ESP, 4");\
    asm("POPAD");

#endif
