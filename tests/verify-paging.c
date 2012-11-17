#include <assert.h>
#include <rose/memory.h>
#include <rose/stdint.h>

#include <stdio.h>

#define PAGE_SIZE          4096
#define PAGE_ENTRY_PRESENT 1
#define PAGE_ENTRY_RW      2

static int is_cr3_called = 0;
static char kmem[20 * PAGE_SIZE] __attribute__((aligned (0x100000)));
static void *start = kmem;
static void *end   = kmem + 0x8800;

static void
check_entry(uint32_t **directory, void *p)
{
    uint32_t directory_index;
    uint32_t table_index;
    uint32_t *table;
    uint32_t table_entry;
    void     *page;

    directory_index = (((uint32_t)p) >> 22);
    table_index     = (((uint32_t)p) >> 12) & 0x3FF;
    table           = directory[directory_index];

    assert(((uint32_t) table) & PAGE_ENTRY_PRESENT);
    assert(((uint32_t) table) & PAGE_ENTRY_RW);

    table = (uint32_t *) (((uint32_t) table) & ~0x3FF);

    table_entry = table[table_index];
    page        = (void *) (table_entry & ~0x3FF);

    assert(((uint32_t) table_entry) & PAGE_ENTRY_PRESENT);
    assert(((uint32_t) table_entry) & PAGE_ENTRY_RW);
    assert(page == p);
}

void
_cr3_set(uint32_t **directory)
{
    void *p;

    is_cr3_called = 1;

    for(p = kmem; p < end; p += PAGE_SIZE) {
        check_entry(directory, p);
    }
    /* XXX check other directory/table entries for !present? */
}

void
_gdt_set(void *p)
{
    (void)p;
}

int
main(void)
{
    memory_init_paging(start, end);
    assert(is_cr3_called);
    return 0;
}
