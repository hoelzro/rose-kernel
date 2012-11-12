#include <rose/screen.h>

static unsigned char *vram = (unsigned char *) 0xB8000;

#define COLS 80
#define ROWS 20

#define WIDTH 2 * COLS

void
screen_write_string_at(const char *s, int x, int y)
{
    int vram_index = WIDTH * y + x;

    while(*s) {
        vram[vram_index]     = *s;
        vram[vram_index + 1] = 0x07;

        s++;
        vram_index += 2;
    }
}

void
screen_clear(void)
{
    char line[COLS + 1];
    int i;

    for(i = 0; i < COLS; i++) {
        line[i] = ' ';
    }
    line[COLS] = '\0';

    for(i = 0; i < ROWS; i++) {
        screen_write_string_at(line, 0, i);
    }
}
