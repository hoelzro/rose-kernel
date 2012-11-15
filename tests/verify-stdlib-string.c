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


int
main(void)
{
    verify_memset();
    return 0;
}
