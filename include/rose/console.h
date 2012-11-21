#ifndef ROSE_CONSOLE_H
#define ROSE_CONSOLE_H

#include <rose/stdarg.h>

int console_write_char(char c);
int console_write_string(const char *s);
int console_write_integer(int radix, int n);
int console_printf(const char *fmt, ...);

#endif
