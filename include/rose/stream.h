#ifndef ROSE_STREAM_H
#define ROSE_STREAM_H

struct stream {
    int (*write_char)(struct stream *, char);
};

int stream_write_char(struct stream *stream, char c);
int stream_write_string(struct stream *stream, const char *s);
int stream_write_integer(struct stream *stream, int radix, int n);
int stream_printf(struct stream *stream, const char *fmt, ...);

#endif
