#ifndef ROSE_SCREEN_H
#define ROSE_SCREEN_H

void screen_clear(void);
void screen_write_string_at(const char *s, int x, int y);
void screen_write_integer_at(long value, int radix, int x, int y);

#endif
