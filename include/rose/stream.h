#ifndef ROSE_STREAM_H
#define ROSE_STREAM_H

#include <rose/stdarg.h>

struct stream {
    int (*write_char)(struct stream *, char);
};

int stream_write_char(struct stream *stream, char c);
int stream_write_string(struct stream *stream, const char *s);
int stream_write_integer(struct stream *stream, int radix, int n);
int stream_printf(struct stream *stream, const char *fmt, ...);
int stream_vprintf(struct stream *stream, const char *fmt, va_list args);

#endif
