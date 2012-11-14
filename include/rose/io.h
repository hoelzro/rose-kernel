#ifndef ROSE_IO_H
#define ROSE_IO_H

#include <rose/stdint.h>

void io_outb(uint16_t port, uint8_t value);
void io_outw(uint16_t port, uint16_t value);
void io_outdw(uint16_t port, uint32_t value);

uint8_t  io_inb(uint16_t port);
uint16_t io_inw(uint16_t port);
uint32_t io_indw(uint16_t port);

#endif
