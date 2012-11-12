#include <rose/screen.h>
#include <rose/descriptor-tables.h>

void
kmain(void)
{
    screen_clear();
    gdt_init();

    screen_write_string_at("Hello from rOSe!", 0, 0);
}
