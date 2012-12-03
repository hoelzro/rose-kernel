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

#include <rose/capabilities.h>
#include <rose/console.h>
#include <rose/interrupts.h>
#include <rose/screen.h>
#include <rose/serial.h>
#include <rose/stdint.h>
#include <rose/string.h>
#include <rose/memory.h>
#include <rose/multiboot.h>

#define CR0_MP (1<<1)
#define CR0_NE (1<<5)
#define CR0_EM (1<<2)

extern void protected_mode_start(void);

extern char end[];

void
fpu_init(void)
{
    uint32_t mask = 0;

    if(capabilities_has_fpu()) {
        mask = CR0_MP | CR0_NE;
    } else {
        mask = CR0_EM | CR0_NE;
    }

    asm("MOV EAX, CR0;"
        "OR EAX, %0;"
        "MOV CR0, EAX;"
       :
       : "r"(mask)
       : "eax"
       );

    asm("FINIT");
}

void
panic(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);

    console_reset();

    if(fmt) {
        console_printf("Kernel panic: ");
        console_vprintf(fmt, args);
        console_printf("\n");
    } else {
        console_printf("Kernel panic!\n");
    }

    console_dump_registers();
    interrupts_disable();
    asm("HLT");
}

void
kmain(struct multiboot_info *mboot)
{
    interrupts_disable();
    fpu_init();
    screen_clear();
    memory_init_gdt();
    interrupts_init();
    protected_mode_start();
    serial_init();
    memory_init_paging(0, end);
    /* XXX turn on interrupts? */

    console_write_string("Hello from rOSe (in protected mode!)\n");
}
