#include <rose/string.h>

/* XXX maybe I could do this in asm? */

void *
memset(void *s, int c, size_t n)
{
    uint8_t *ptr = s;

    while(n) {
        *ptr = c;

        ptr++;
        n--;
    }
    return s;
}
