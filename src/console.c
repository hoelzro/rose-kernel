#include <rose/console.h>
#include <rose/screen.h>
#include <rose/stream.h>

#define COLS 80
#define ROWS 25

struct console_stream {
    struct stream stream;

    int row;
    int column;
};

static int
_console_write_char(struct console_stream *stream, char c)
{
    switch(c) {
        case '\n':
            stream->row++;
        case '\r':
            stream->column = 0;
            break;
        default:
            screen_write_char_at(c, stream->column, stream->row);
            stream->column++;

            if(stream->column > COLS) {
                stream->column = 0;
                stream->row++;
            }
    }
    while(stream->row >= ROWS) {
        screen_scroll_up();
        stream->row--;
    }

    return 1;
}

struct console_stream stream = {
    .stream = {
        .write_char = (int (*)(struct stream *, char)) _console_write_char,
    },
    .row    = 0,
    .column = 0,
};

static struct stream *
_get_stream(void)
{
    return (struct stream *) &stream;
}

int
console_write_char(char c)
{
    return stream_write_char(_get_stream(), c);
}

int
console_write_string(const char *s)
{
    return stream_write_string(_get_stream(), s);
}

int
console_write_integer(int radix, int n)
{
    return stream_write_integer(_get_stream(), radix, n);
}

int
console_printf(const char *fmt, ...)
{
    va_list args;
    int chars_written;

    va_start(args, fmt);

    chars_written = stream_vprintf(_get_stream(), fmt, args);
    va_end(args);

    return chars_written;
}
