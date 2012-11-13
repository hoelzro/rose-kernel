#include <rose/descriptor-tables.h>
#include <rose/stdint.h>
#include <rose/string.h>

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

struct task_gate {
    uint16_t reserved0;
    uint16_t tss;
    uint8_t  reserved1;
    uint8_t  id_bits : 5; /* 0x05 */
    uint8_t  dpl     : 2;
    uint8_t  present : 1;
    uint16_t reserved2;
} __attribute__((packed));

struct interrupt_gate {
    uint16_t offset_lower;
    uint16_t segment_selector; /* XXX segment selector type? */
    uint8_t  reserved0;
    uint8_t  id_bits   : 3; /* 0x06 */
    uint8_t  gate_size : 1;
    uint8_t  reserved1 : 1;
    uint8_t  dpl       : 2;
    uint8_t  present   : 1;
    uint16_t offset_upper;
} __attribute__((packed));

struct trap_gate {
    uint16_t offset_lower;
    uint16_t segment_selector; /* XXX segment selector type? */
    uint8_t  reserved0;
    uint8_t  id_bits   : 3; /* 0x07 */
    uint8_t  gate_size : 1;
    uint8_t  reserved1 : 1;
    uint8_t  dpl     : 2;
    uint8_t  present : 1;
    uint16_t offset_upper;
} __attribute__((packed));

struct idt_common {
    uint32_t reserved0;
    uint8_t  reserved1;
    uint8_t  id_bits : 5;
    uint8_t  dpl     : 2;
    uint8_t  present : 1;
    uint16_t reserved2;
} __attribute__((packed));

union idt_entry {
    struct task_gate      task_gate;
    struct interrupt_gate interrupt_gate;
    struct trap_gate      trap_gate;

    struct idt_common common;
} __attribute__((packed));

struct idt_pointer {
    uint16_t limit;
    union idt_entry *first;
} __attribute__((packed));

struct gdt_entry gdt[5] __attribute__((aligned (8)));
struct gdt_pointer gdt_ptr;

union idt_entry idt[256] __attribute__((aligned (8)));
struct idt_pointer idt_ptr;

/* defined in gdt-helper.s */
extern void
gdt_set(struct gdt_pointer *gdt);

extern void
idt_set(struct idt_pointer *idt);

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

static void
setup_interrupt_gate(union idt_entry *entry, void *handler)
{
    uint32_t handler_as_int = (uint32_t) handler;

    entry->interrupt_gate.offset_lower     = handler_as_int & 0xFFFF;
    entry->interrupt_gate.offset_upper     = (handler_as_int >> 16) & 0xFFFF;
    entry->interrupt_gate.segment_selector = 0x08; /* kernelspace code segment */
    entry->interrupt_gate.id_bits          = 0x06;
    entry->interrupt_gate.gate_size        = 1; /* 32-bit */
    entry->interrupt_gate.dpl              = 0; /* change to 3 when we start using userspace */
    entry->interrupt_gate.present          = 1;
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
        gdt[i].default_operation_size = 1; /* 32-bit */
        gdt[i].granularity            = 1; /* XXX for now */
        //gdt[i].granularity            = 0; /* byte-level (the example uses kbyte level?) */
        gdt[i].avl                    = 0;
        gdt[i].descriptor_type        = 1;
    }

    gdt[1].type = gdt[3].type = 0x0A;
    gdt[2].type = gdt[4].type = 0x02;

    gdt[1].dpl = gdt[2].dpl = 0;
    gdt[3].dpl = gdt[4].dpl = 3;

    gdt_ptr.limit = sizeof(gdt) - 1;
    gdt_ptr.first = gdt;

    gdt_set(&gdt_ptr);
}

extern void isr0(void);
extern void isr1(void);
extern void isr2(void);
extern void isr3(void);
extern void isr4(void);
extern void isr5(void);
extern void isr6(void);
extern void isr7(void);
extern void isr8(void);
extern void isr9(void);
extern void isr10(void);
extern void isr11(void);
extern void isr12(void);
extern void isr13(void);
extern void isr14(void);
extern void isr15(void);
extern void isr16(void);
extern void isr17(void);
extern void isr18(void);
extern void isr19(void);
extern void isr20(void);
extern void isr21(void);
extern void isr22(void);
extern void isr23(void);
extern void isr24(void);
extern void isr25(void);
extern void isr26(void);
extern void isr27(void);
extern void isr28(void);
extern void isr29(void);
extern void isr30(void);
extern void isr31(void);

void
idt_init(void)
{
    /* zero-out all interrupt handlers */
    memset(idt, 0, sizeof(idt));

#define install_isr(n)\
    setup_interrupt_gate(idt + n, isr##n)

    install_isr(0);
    install_isr(1);
    install_isr(2);
    install_isr(3);
    install_isr(4);
    install_isr(5);
    install_isr(6);
    install_isr(7);
    install_isr(8);
    install_isr(9);
    install_isr(10);
    install_isr(11);
    install_isr(12);
    install_isr(13);
    install_isr(14);
    install_isr(15);
    install_isr(16);
    install_isr(17);
    install_isr(18);
    install_isr(19);
    install_isr(20);
    install_isr(21);
    install_isr(22);
    install_isr(23);
    install_isr(24);
    install_isr(25);
    install_isr(26);
    install_isr(27);
    install_isr(28);
    install_isr(29);
    install_isr(30);
    install_isr(31);

    idt_ptr.limit = sizeof(idt) - 1;
    idt_ptr.first = idt;

    idt_set(&idt_ptr);
}
