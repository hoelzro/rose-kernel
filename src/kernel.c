#include <rose/screen.h>

void
kmain(void)
{
    screen_clear();
    screen_write_string_at("Hello from rOSe!", 0, 0);
}
