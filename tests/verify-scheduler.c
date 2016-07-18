#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <rose/scheduler.h>

void
panic(const char *message)
{
    assert(0);
}


void *
memory_allocate_page(void)
{
    return aligned_alloc(getpagesize(), getpagesize());
}

void
memory_free_page(void *page)
{
    free(page);
}

static int values[10];
static int value_index = 0;

static
void even_proc(void)
{
    int i;

    for(i = 2; i <= 10; i += 2) {
        values[value_index++] = i;
        scheduler_yield();
    }
}

static
void odd_proc(void)
{
    int i;

    for(i = 1; i <= 9; i += 2) {
        values[value_index++] = i;
        scheduler_yield();
    }
}


int
main(void)
{
    int i;

    scheduler_init();
    scheduler_add_process(odd_proc);
    scheduler_add_process(even_proc);

    scheduler_run();

    for(i = 0; i < 10; i++) {
        assert(values[i] == i + 1);
    }
    return 0;
}
