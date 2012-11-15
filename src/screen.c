#include <rose/stdarg.h>
#include <rose/screen.h>

static volatile unsigned short *vram = (unsigned short *) 0xB8000;

#define COLS 80
#define ROWS 25

#define MAX_DIGITS 64
#define COLOR_GREY 0x07

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
screen_write_char_at(char c, int x, int y)
{
    int vram_index = COLS * y + x;
    vram[vram_index] = (COLOR_GREY << 8) | c;
}

int
screen_write_string_at(const char *s, int x, int y)
{
    int length = 0;

    while(*s) {
        screen_write_char_at(*s, x, y);

        s++;
        x++;
        length++;
    }

    return length;
}

int
screen_write_integer_at(long value, int radix, int x, int y)
{
    const char *string_form;

    string_form = itoa(value, radix);

    return screen_write_string_at(string_form, x, y);
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

void
screen_printf(int x, int y, const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);

    for(; *fmt && x < COLS; fmt++) {
        if(*fmt == '%') {
            fmt++;
            char next = *fmt;

            switch(next) {
                case '\0':
                    fmt--; /* make sure we pick it up in the condition */
                    break;
                case 'd': {
                    int value = va_arg(args, int);
                    x += screen_write_integer_at(value, 10, x, y);
                    break;
                }
                case 'p':
                    x += screen_write_string_at("0x", x, y);
                case 'x': {
                    int value = va_arg(args, int);
                    x += screen_write_integer_at(value, 16, x, y);
                    break;
                }
                case 's': {
                    const char *value = va_arg(args, const char *);
                    x += screen_write_string_at(value, x, y);
                    break;
                }
                default:
                    screen_write_char_at('%', x++, y);
                    screen_write_char_at(next, x++, y);
            }
        } else {
            screen_write_char_at(*fmt, x++, y);
        }
    }

    va_end(args);
}
