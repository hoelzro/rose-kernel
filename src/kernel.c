#include <rose/screen.h>
#include <rose/descriptor-tables.h>

extern void protected_mode_start(void);

void
kmain(void)
{
    screen_clear();
    gdt_init();
    protected_mode_start();

    screen_write_string_at("Hello from rOSe (in protected mode!)", 0, 0);
}
