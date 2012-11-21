#ifndef ROSE_SERIAL_H
#define ROSE_SERIAL_H

void serial_init(void);
int serial_write_char(char c);
int serial_write_string(const char *s);
int serial_write_integer(int radix, int n);
int serial_printf(const char *fmt, ...);

#endif
