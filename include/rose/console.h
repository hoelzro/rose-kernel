/*
 * Copyright (c) 2012 Rob Hoelz <rob at hoelz.ro>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef ROSE_CONSOLE_H
#define ROSE_CONSOLE_H

#include <rose/stdarg.h>

int console_write_char(char c);
int console_write_string(const char *s);
int console_write_integer(int radix, int n);
int console_printf(const char *fmt, ...);
int console_vprintf(const char *fmt, va_list args);

void console_reset(void);
void console_get_cursor(int *x, int *y);
void console_set_cursor(int x, int y);

struct registers;
void _console_dump_registers(struct registers *regs);

#define console_dump_registers()\
    asm("PUSHAD");\
    asm("PUSH ESP");\
    asm("CALL _console_dump_registers");\
    asm("ADD ESP, 4");\
    asm("POPAD");

#endif
