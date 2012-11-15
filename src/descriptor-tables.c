#include <rose/descriptor-tables.h>
#include <rose/stdint.h>

#define OP_SIZE_16BIT         0
#define OP_SIZE_32BIT         1
#define DPL_KERNEL            0
#define DPL_USER              3
#define GRANULARITY_BYTE      0
#define GRANULARITY_KBYTE     1
#define DESC_TYPE_SYSTEM      0
#define DESC_TYPE_CODEDATA    1

#define DESC_TYPE_DATA        0
#define DESC_TYPE_CODE        (1 << 3)
#define DESC_TYPE_EXPAND_UP   0
#define DESC_TYPE_EXPAND_DOWN (1 << 2)
#define DESC_TYPE_RO          0
#define DESC_TYPE_RW          (1 << 1)
#define DESC_TYPE_XO          0
#define DESC_TYPE_RX          (1 << 1)

struct gdt_entry {
    uint16_t limit_lower;
    uint16_t address_lower;
    uint8_t  address_middle;
    uint8_t  type                   : 4;
    uint8_t  descriptor_type        : 1;
    uint8_t  dpl                    : 2;
    uint8_t  segment_present        : 1;
    uint8_t  limit_high             : 4;
    uint8_t  avl                    : 1;
    uint8_t  is_64                  : 1;
    uint8_t  default_operation_size : 1;
    uint8_t  granularity            : 1;
    uint8_t  address_high;
} __attribute__((packed));

struct gdt_pointer {
    uint16_t limit;
    struct gdt_entry *first;
} __attribute__((packed));

struct gdt_entry gdt[5] __attribute__((aligned (8)));
struct gdt_pointer gdt_ptr;

/* defined in gdt-helper.s */
extern void
gdt_set(struct gdt_pointer *gdt);

static void
gdt_entry_set_base(struct gdt_entry *entry, uint32_t base)
{
    entry->address_lower  = base & 0xFFFF;
    entry->address_middle = (base >> 16) & 0xFF;
    entry->address_high   = (base >> 24) & 0xFF;
}

static void
gdt_entry_set_limit(struct gdt_entry *entry, uint32_t limit)
{
    entry->limit_lower = limit & 0xFFFF;
    entry->limit_high  = (limit >> 16) & 0x0F;
}

/* The GDT should contain five segment descriptors:
 *
 *  0: Null Segment
 *  1: Kernelspace Code Segment
 *  2: Kernelspace Data Segment
 *  3: Userspace Code Segment
 *  4: Userspace Data Segment
 *
 */

void
gdt_init(void)
{
    int i;

    gdt_entry_set_base(gdt, 0);
    gdt_entry_set_limit(gdt, 0);

    gdt[0].segment_present        = 0;
    gdt[0].is_64                  = 0;
    gdt[0].default_operation_size = 0;
    gdt[0].granularity            = 0;
    gdt[0].avl                    = 0;
    gdt[0].type                   = 0;
    gdt[0].descriptor_type        = 0;
    gdt[0].dpl                    = 0;

    for(i = 1; i < 5; i++) {
        gdt_entry_set_base(gdt + i, 0);
        gdt_entry_set_limit(gdt + i, 0xFFFFFFFF);

        gdt[i].segment_present        = 1;
        gdt[i].is_64                  = 0;
        gdt[i].default_operation_size = OP_SIZE_32BIT;
        gdt[i].granularity            = GRANULARITY_KBYTE;
        gdt[i].avl                    = 0;
        gdt[i].descriptor_type        = DESC_TYPE_CODEDATA;
    }

    gdt[1].type = gdt[3].type = DESC_TYPE_CODE | DESC_TYPE_RX;
    gdt[2].type = gdt[4].type = DESC_TYPE_DATA | DESC_TYPE_RW;

    gdt[1].dpl = gdt[2].dpl = DPL_KERNEL;
    gdt[3].dpl = gdt[4].dpl = DPL_USER;

    gdt_ptr.limit = sizeof(gdt) - 1;
    gdt_ptr.first = gdt;

    gdt_set(&gdt_ptr);
}
