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
}

static void
verify_memcpy(void)
{
}

static void
verify_memmove(void)
{
}

static void
verify_strcat(void)
{
}

static void
verify_strchr(void)
{
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
