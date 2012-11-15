#include <rose/interrupts.h>
#include <rose/screen.h>
#include <rose/serial.h>
#include <rose/descriptor-tables.h>

extern void protected_mode_start(void);

void
kmain(void)
{
    interrupts_disable();
    screen_clear();
    gdt_init();
    interrupts_init();
    protected_mode_start();
    serial_init();
    /* XXX turn on interrupts? */

    screen_write_string_at("Hello from rOSe (in protected mode!)", 0, 0);
}
