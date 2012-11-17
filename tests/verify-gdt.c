#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <rose/memory.h>

void
idt_set(void *p)
{
    (void)p;
}

void
screen_write_string_at(const char *s, int x, int y)
{
    (void)s;
    (void)x;
    (void)y;
}

struct gdt_pointer {
    uint16_t limit;
    void *base;
} __attribute__((packed));

static int gdt_set_called = 0;

void
_gdt_set(struct gdt_pointer *gdt)
{
    uint64_t *entries = (uint64_t *) gdt->base;

    gdt_set_called = 1;

    assert(gdt->limit == 39);
    assert(entries[0] == 0x0000000000000000); // null segment
    assert(entries[1] == 0x00CF9A000000FFFF); // kernelspace code segment
    assert(entries[2] == 0x00CF92000000FFFF); // kernelspace data segment
    assert(entries[3] == 0x00CFFA000000FFFF); // userspace code segment
    assert(entries[4] == 0x00CFF2000000FFFF); // userspace data segment
}

int
main(void)
{
    memory_init_gdt();
    assert(gdt_set_called);
    return 0;
}
