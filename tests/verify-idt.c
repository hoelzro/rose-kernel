#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <rose/interrupts.h>

struct idt_pointer {
    uint16_t limit;
    void *base;
} __attribute__((packed));

void
gdt_set(void *p)
{
    (void)p;
}

void
console_printf(const char *fmt, ...)
{
    (void)fmt;
}

static int idt_set_called = 0;

void
_idt_set(struct idt_pointer *idt)
{
    uint64_t *entries = (uint64_t *) idt->base;

    idt_set_called = 1;
    assert(idt->limit == 2047);

    assert(entries[0]  == 0x00008E0000080000);
    assert(entries[1]  == 0x00008E0000081111);
    assert(entries[2]  == 0x00008E0000082222);
    assert(entries[3]  == 0x00008E0000083333);
    assert(entries[4]  == 0x00008E0000084444);
    assert(entries[5]  == 0x00008E0000085555);
    assert(entries[6]  == 0x00008E0000086666);
    assert(entries[7]  == 0x00008E0000087777);
    assert(entries[8]  == 0x00008E0000088888);
    assert(entries[9]  == 0x00008E0000089999);
    assert(entries[10] == 0x00008E000008AAAA);
    assert(entries[11] == 0x00008E000008BBBB);
    assert(entries[12] == 0x00008E000008CCCC);
    assert(entries[13] == 0x00008E000008DDDD);
    assert(entries[14] == 0x00008E000008EEEE);
    assert(entries[15] == 0x00008E000008FFFF);
    assert(entries[16] == 0x11118E0000080000);
    assert(entries[17] == 0x11118E0000081111);
    assert(entries[18] == 0x11118E0000082222);
    assert(entries[19] == 0x11118E0000083333);
    assert(entries[20] == 0x11118E0000084444);
    assert(entries[21] == 0x11118E0000085555);
    assert(entries[22] == 0x11118E0000086666);
    assert(entries[23] == 0x11118E0000087777);
    assert(entries[24] == 0x11118E0000088888);
    assert(entries[25] == 0x11118E0000089999);
    assert(entries[26] == 0x11118E000008AAAA);
    assert(entries[27] == 0x11118E000008BBBB);
    assert(entries[28] == 0x11118E000008CCCC);
    assert(entries[29] == 0x11118E000008DDDD);
    assert(entries[30] == 0x11118E000008EEEE);
    assert(entries[31] == 0x11118E000008FFFF);

}

int
main(void)
{
    interrupts_init();
    assert(idt_set_called);
    return 0;
}
