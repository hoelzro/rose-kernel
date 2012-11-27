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

#include <rose/io.h>
#include <rose/serial.h>
#include <rose/stream.h>

/* This code is based on code from this page:
 *
 *   http://wiki.osdev.org/Serial_Ports
 */

#define COM1_PORT 0x03F8

void
serial_init(void)
{
    io_outb(COM1_PORT + 1, 0x00); // Disable all interrupts
    io_outb(COM1_PORT + 3, 0x80); // Enable DLAB (set baud rate divisor)
    io_outb(COM1_PORT + 0, 0x03); // Set divisor to 3 (lo byte) 38400 baud
    io_outb(COM1_PORT + 1, 0x00); //                  (hi byte)
    io_outb(COM1_PORT + 3, 0x03); // 8 bits, no parity, one stop bit
    io_outb(COM1_PORT + 2, 0xC7); // Enable FIFO, clear them, with 14-byte threshold
    io_outb(COM1_PORT + 4, 0x0B); // IRQs enabled, RTS/DSR set
}

static int
is_transmit_empty(void)
{
    return io_inb(COM1_PORT + 5) & 0x20;
}

static int
_write_char_to_serial(struct stream *stream, char c)
{
    (void) stream;

    while(! is_transmit_empty());

    io_outb(COM1_PORT, c);

    return 1;
}

static struct stream serial_stream = {
    .write_char = _write_char_to_serial,
};

int
serial_write_char(char c)
{
    return stream_write_char(&serial_stream, c);
}

int
serial_write_string(const char *s)
{
    return stream_write_string(&serial_stream, s);
}

int
serial_write_integer(int radix, int n)
{
    return stream_write_integer(&serial_stream, radix, n);
}

int
serial_printf(const char *fmt, ...)
{
    va_list args;
    int chars_written;

    va_start(args, fmt);

    chars_written = stream_vprintf(&serial_stream, fmt, args);
    va_end(args);

    return chars_written;
}
