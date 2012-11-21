#include <rose/screen.h>
#include <rose/stdarg.h>
#include <rose/stream.h>

static volatile unsigned short *vram = (unsigned short *) 0xB8000;

#define COLS 80
#define ROWS 25

#define COLOR_GREY 0x07

struct screen_stream {
    struct stream stream;

    volatile unsigned short *buffer;
};

static int
screen_write_char(struct screen_stream *memory, char c)
{
    memory->buffer[0] = (COLOR_GREY << 8) | c;
    memory->buffer++;

    return 1;
}

static struct screen_stream stream = {
    .stream = {
        .write_char = (int (*)(struct stream *, char)) screen_write_char,
    },
};

static void
_set_stream_offset(struct screen_stream *stream, int x, int y)
{
    stream->buffer = vram + y * COLS + x;
}

void
screen_write_char_at(char c, int x, int y)
{
    _set_stream_offset(&stream, x, y);
    stream_write_char((struct stream *) &stream, c);
}

int
screen_write_string_at(const char *s, int x, int y)
{
    _set_stream_offset(&stream, x, y);
    return stream_write_string((struct stream *) &stream, s);
}

int
screen_write_integer_at(long value, int radix, int x, int y)
{
    _set_stream_offset(&stream, x, y);
    return stream_write_integer((struct stream *) &stream, radix, value);
}

void
screen_clear(void)
{
    char line[COLS + 1];
    int i;

    for(i = 0; i < COLS; i++) {
        line[i] = ' ';
    }
    line[COLS] = '\0';

    for(i = 0; i < ROWS; i++) {
        screen_write_string_at(line, 0, i);
    }
}

void
screen_printf(int x, int y, const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    _set_stream_offset(&stream, x, y);

    stream_vprintf((struct stream *) &stream, fmt, args);
    va_end(args);
}
