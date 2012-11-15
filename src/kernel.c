#include <rose/io.h>
#include <rose/screen.h>
#include <rose/serial.h>
#include <rose/stdint.h>
#include <rose/descriptor-tables.h>

#define NMI_PORT           0x70
#define NMI_INTERRUPTS_OFF 0x80

extern void protected_mode_start(void);

struct registers {
    uint32_t ds;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t interrupt_no;
    uint32_t error_code;
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
} __attribute__((packed));

void
general_isr(struct registers regs)
{
    screen_write_string_at("Handling interrupt: 0x", 0, 1);
    screen_write_integer_at(regs.interrupt_no, 16, 22, 1);
}

void
disable_interrupts(void)
{
    asm volatile ("CLI"); /* disable maskable interrupts */
    io_outb(NMI_PORT, io_inb(NMI_PORT) | NMI_INTERRUPTS_OFF);
}

void
kmain(void)
{
    disable_interrupts();
    screen_clear();
    gdt_init();
    idt_init();
    protected_mode_start();
    serial_init();
    /* XXX turn on interrupts? */

    screen_write_string_at("Hello from rOSe (in protected mode!)", 0, 0);
}
