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

void *
memchr(const void *s, int c, size_t n)
{
    const char *p = s;
    while(n) {
        if(*p == c) {
            return (void *) p;
        }
        p++;
        n--;
    }
    return NULL;
}

int
memcmp(const void *lhs, const void *rhs, size_t n)
{
    const char *lhs_c = lhs;
    const char *rhs_c = rhs;

    while(n) {
        if(*lhs_c != *rhs_c) {
            return *lhs_c - *rhs_c;
        }

        lhs_c++;
        rhs_c++;
        n--;
    }
    return 0;
}

void *
memcpy(void *dest, const void *src, size_t n)
{
    char *dest_c      = dest;
    const char *src_c = src;

    while(n) {
        *dest_c = *src_c;

        dest_c++;
        src_c++;
        n--;
    }

    return dest;
}

/* XXX implement me! */
void *
memmove(void *dest, const void *src, size_t n)
{
    (void) dest;
    (void) src;
    (void) n;

    return NULL;
}

char *
strcat(char * dst, const char * src)
{
    return strncat(dst, src, strlen(src));
}

char *
strchr(const char *s, int c)
{
    return memchr(s, c, strlen(s));
}

int
strcmp(const char *lhs, const char *rhs)
{
    int lhs_length = strlen(lhs);
    int rhs_length = strlen(rhs);

    /* XXX we want to behave the right way if the lengths don't match */
    return strncmp(lhs, rhs,
        lhs_length < rhs_length ? lhs_length : rhs_length);
}

char *
strcpy(char * src, const char * dest)
{
    return strncpy(src, dest, strlen(dest));
}

size_t
strlen(const char *s)
{
    size_t n = 0;

    while(*s) {
        n++;
        s++;
    }

    return n;
}

char *
strncat(char * lhs, const char * rhs, size_t n)
{
    return strncpy(lhs + strlen(lhs), rhs, n);
}

int
strncmp(const char *lhs, const char *rhs, size_t n)
{
    return memcmp(lhs, rhs, n);
}

char *
strncpy(char * dest, const char * src, size_t n)
{
    memcpy(dest, src, n - 1);
    dest[n - 1] = '\0';

    return dest;
}

char *
strrchr(const char *s, int c)
{
    const char *previous_location = NULL;

    while(*s) {
        if(*s == c) {
            previous_location = s;
        }
        s++;
    }

    return (char *) previous_location;
}

char *
strstr(const char *haystack, const char *needle)
{
    char *potential_find;
    size_t needle_len = strlen(needle);

    while((potential_find = strchr(haystack, *needle))) {
        if(strncmp(potential_find, needle, needle_len) == 0) {
            return potential_find;
        }
        haystack = potential_find + 1;
    }

    return NULL;
}
