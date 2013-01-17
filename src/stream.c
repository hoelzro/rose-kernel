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

#include <rose/stdarg.h>
#include <rose/stream.h>

#define MAX_DIGITS 64

static char char_map[] = "0123456789ABCDEF";

static const char *
itoa(unsigned long value, unsigned int radix)
{
    static char buffer[MAX_DIGITS + 2];
    char *p;

    p = buffer + sizeof(buffer) - 1;

    *p = '\0';
    --p;

    while(value >= radix) {
        *p = char_map[value % radix];
        --p;
        value /= radix;
    }

    *p = char_map[value];

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
stream_vprintf(struct stream *stream, const char *fmt, va_list args)
{
    int chars_written = 0;

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
                    if(!value) {
                        value = "(nil)";
                    }
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

    return chars_written;
}

int
stream_printf(struct stream *stream, const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);

    return stream_vprintf(stream, fmt, args);
}
