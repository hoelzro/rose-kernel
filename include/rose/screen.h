#ifndef ROSE_SCREEN_H
#define ROSE_SCREEN_H

void screen_clear(void);
void screen_write_char_at(char c, int x, int y);
int screen_write_string_at(const char *s, int x, int y);
int screen_write_integer_at(long value, int radix, int x, int y);
void screen_printf(int x, int y, const char *fmt, ...);
void screen_scroll_up(void);

#endif
