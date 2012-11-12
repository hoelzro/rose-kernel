#include <rose/screen.h>

static unsigned char *vram = (unsigned char *) 0xB8000;

#define COLS 80
#define ROWS 20

#define WIDTH 2 * COLS
#define MAX_DIGITS 64

static char char_map[] = "0123456789";

static const char *
itoa(long value, int radix)
{
    static char buffer[MAX_DIGITS + 2];
    char *p;
    int is_negative;

    p = buffer + sizeof(buffer) - 1;

    is_negative = value < 0;

    if(is_negative) {
        is_negative *= -1;
    }

    *p = '\0';
    --p;

    while(value >= radix) {
        *p = char_map[value % radix];
        --p;
        value /= radix;
    }

    *p = char_map[value];

    if(is_negative) {
        --p;
        *p = '-';
    }

    return p;
}

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
screen_write_integer_at(long value, int radix, int x, int y)
{
    const char *string_form;

    string_form = itoa(value, radix);

    screen_write_string_at(string_form, x, y);
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
