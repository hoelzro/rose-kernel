#include <rose/interrupts.h>
#include <rose/screen.h>
#include <rose/serial.h>
#include <rose/memory.h>

extern void protected_mode_start(void);

void
kmain(void)
{
    interrupts_disable();
    screen_clear();
    memory_init_gdt();
    interrupts_init();
    protected_mode_start();
    serial_init();
    /* XXX turn on interrupts? */

    screen_write_string_at("Hello from rOSe (in protected mode!)", 0, 0);
}
