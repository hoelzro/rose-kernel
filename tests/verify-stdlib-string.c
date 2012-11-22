#include <assert.h>
#include <rose/string.h>

static void
verify_memset(void)
{
    char memory[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    int i;

    memset(memory, 1, 8);

    for(i = 0; i < 8; i++) {
        assert(memory[i] == 1);
    }

    memset(memory, 2, 4);

    for(i = 0; i < 4; i++) {
        assert(memory[i] == 2);
    }
    for(i = 4; i < 8; i++) {
        assert(memory[i] == 1);
    }

    memset(memory + 4, 3, 4);
    for(i = 0; i < 4; i++) {
        assert(memory[i] == 2);
    }
    for(i = 4; i < 8; i++) {
        assert(memory[i] == 3);
    }
}

static void
verify_memchr(void)
{
    char memory[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
    char *p;

    p = memchr(memory, 3, 8);
    assert(p == memory + 3);
    p = memchr(memory, 8, 8);
    assert(p == NULL);
    p = memchr(memory, 4, 4);
    assert(p == NULL);
}

static void
verify_memcmp(void)
{
    const char *lhs    = "foo ";
    const char *rhs    = "food";

    assert(memcmp(lhs, rhs, 3) == 0);
    assert(memcmp(lhs, rhs, 4) == ' ' - 'd');
    assert(memcmp(lhs, rhs, 0) == 0);
}

static void
verify_memcpy(void)
{
    char dest[5] = { 0, 0, 0, 0, 1 };
    const char *src = "food";

    assert(memcpy(dest, src, 4) == dest);

    assert(memcmp(dest, src, 4) == 0);
    assert(memcmp(dest, src, 5) == 1);
    assert(memcpy(dest + 2, src, 3) == (dest + 2));
    assert(memcmp(dest, "fofoo", 5) == 0);
}

static void
verify_memmove(void)
{
    char buffer[10];
    int i;

    for(i = 0; i < 10; i++) {
        buffer[i] = 'a';
    }

    /* test no-op situation */
    assert(memmove(buffer, buffer, 10) == buffer);

    assert(memcmp(buffer, "aaaaaaaaaa", 10) == 0);

    for(i = 0; i < 5; i++) {
        buffer[i] = 'b';
    }

    /* test no overlap situation */
    assert(memmove(buffer + 5, buffer, 5) == (buffer + 5));
    assert(memcmp(buffer, "bbbbbbbbbb", 10) == 0);

    for(i = 0; i < 10; i++) {
        buffer[i] = 'a' + i;
    }

    /* test overlap: dest < src */
    assert(memmove(buffer, buffer + 3, 7) == buffer);
    assert(memcmp(buffer, "defghijhij", 10) == 0);

    /* test overlap: src < dest */
    for(i = 0; i < 10; i++) {
        buffer[i] = 'a' + i;
    }
    assert(memmove(buffer + 3, buffer, 7) == (buffer + 3));
    assert(memcmp(buffer, "abcabcdefg", 10) == 0);
}

static void
verify_strcat(void)
{
    char string[10] = { 'f', 'o', 'o', '\0' };

    assert(strcat(string, "bar") == string);
    assert(memcmp(string, "foobar", 6) == 0);
    assert(strcat(string, "baz") == string);
    assert(memcmp(string, "foobarbaz", 9) == 0);
}

static void
verify_strchr(void)
{
    char *s = "abcdefghijklmnopqrstuvwxyz";
    unsigned int i;

    for(i = 0; i < sizeof(s) - 1; i++) {
        assert(strchr(s, s[i]) == s + i);
    }
    assert(strchr(s, ' ') == NULL);
}

static void
verify_strcmp(void)
{
}

static void
verify_strcpy(void)
{
}

static void
verify_strlen(void)
{
    assert(strlen("") == 0);
    assert(strlen("abc123") == 6);
}

static void
verify_strncat(void)
{
}

static void
verify_strncmp(void)
{
}

static void
verify_strncpy(void)
{
}

static void
verify_strrchr(void)
{
}

static void
verify_strstr(void)
{
}

int
main(void)
{
    verify_memset();
    verify_memchr();
    verify_memcmp();
    verify_memcpy();
    verify_memmove();
    verify_strcat();
    verify_strchr();
    verify_strcmp();
    verify_strcpy();
    verify_strlen();
    verify_strncat();
    verify_strncmp();
    verify_strncpy();
    verify_strrchr();
    verify_strstr();

    return 0;
}
