#include <rose/screen.h>
#include <rose/stdint.h>
#include <rose/descriptor-tables.h>

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
kmain(void)
{
    screen_clear();
    gdt_init();
    idt_init();
    protected_mode_start();
    /* XXX turn on interrupts? */

    screen_write_string_at("Hello from rOSe (in protected mode!)", 0, 0);
}
