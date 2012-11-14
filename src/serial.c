#include <rose/io.h>
#include <rose/serial.h>

#define PORT 0x03F8

void
serial_init(void)
{
    io_outb(PORT + 1, 0x00); // Disable all interrupts
    io_outb(PORT + 3, 0x80); // Enable DLAB (set baud rate divisor)
    io_outb(PORT + 0, 0x03); // Set divisor to 3 (lo byte) 38400 baud
    io_outb(PORT + 1, 0x00); //                  (hi byte)
    io_outb(PORT + 3, 0x03); // 8 bits, no parity, one stop bit
    io_outb(PORT + 2, 0xC7); // Enable FIFO, clear them, with 14-byte threshold
    io_outb(PORT + 4, 0x0B); // IRQs enabled, RTS/DSR set
}

static int
is_transmit_empty(void)
{
    return io_inb(PORT + 5) & 0x20;
}

void
serial_write(const char *s)
{
    while(*s) {
        while(! is_transmit_empty());

        io_outb(PORT, *s);
        s++;
    }
}
