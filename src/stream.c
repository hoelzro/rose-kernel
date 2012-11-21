#include <rose/stdarg.h>
#include <rose/stream.h>

#define MAX_DIGITS 64

static char char_map[] = "0123456789ABCDEF";

static const char *
itoa(long value, int radix)
{
    static char buffer[MAX_DIGITS + 2];
    char *p;
    int is_negative;

    p = buffer + sizeof(buffer) - 1;

    is_negative = value < 0;

    if(is_negative) {
        is_negative *= -1;
    }

    *p = '\0';
    --p;

    while(value >= radix) {
        *p = char_map[value % radix];
        --p;
        value /= radix;
    }

    *p = char_map[value];

    if(is_negative) {
        --p;
        *p = '-';
    }

    return p;
}

int
stream_write_char(struct stream *stream, char c)
{
    return stream->write_char(stream, c);
}

int
stream_write_string(struct stream *stream, const char *s)
{
    int chars_written = 0;

    while(*s) {
        stream_write_char(stream, *s);

        chars_written++;
        s++;
    }

    return chars_written;
}

int
stream_write_integer(struct stream *stream, int radix, int n)
{
    const char *string_form;

    string_form = itoa(n, radix);

    return stream_write_string(stream, string_form);
}

int
stream_printf(struct stream *stream, const char *fmt, ...)
{
    va_list args;
    int chars_written = 0;

    va_start(args, fmt);

    for(; *fmt; fmt++) {
        if(*fmt == '%') {
            fmt++;
            char next = *fmt;

            switch(next) {
                case '\0':
                    fmt--; /* make sure we pick it up in the condition */
                    break;
                case 'd': {
                    int value = va_arg(args, int);
                    chars_written += stream_write_integer(stream, 10, value);
                    break;
                }
                case 'p':
                    chars_written += stream_write_string(stream, "0x");
                case 'x': {
                    int value = va_arg(args, int);
                    chars_written += stream_write_integer(stream, 16, value);
                    break;
                }
                case 's': {
                    const char *value = va_arg(args, const char *);
                    chars_written += stream_write_string(stream, value);
                    break;
                }
                default:
                    stream_write_char(stream, '%');
                    stream_write_char(stream, next);
                    chars_written += 2;
            }
        } else {
            stream_write_char(stream, *fmt);
            chars_written++;
        }
    }

    va_end(args);

    return chars_written;
}
