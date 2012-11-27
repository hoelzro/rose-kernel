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

void
screen_scroll_up(void)
{
    volatile unsigned short *dest = vram;
    volatile unsigned short *src  = vram + COLS;
    int i;

    for(i = 0; i < (ROWS - 1) * COLS; i++) {
        *dest++ = *src++;
    }
    for(i = 0; i < COLS; i++) {
        *dest++ = ' ';
    }
}
