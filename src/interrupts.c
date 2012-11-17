#include <rose/interrupts.h>
#include <rose/io.h>
#include <rose/screen.h>
#include <rose/string.h>

#define SEGMENT_KERNEL_CODE   0x08
#define IDT_INTERRUPT_GATE    0x06
#define GATE_SIZE_16BIT       0
#define GATE_SIZE_32BIT       1
#define DPL_KERNEL            0
#define DPL_USER              3

#define NMI_PORT           0x70
#define NMI_INTERRUPTS_OFF 0x80

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

struct registers {
    uint32_t ds;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t interrupt_no;
    uint32_t error_code;
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
} __attribute__((packed));

union idt_entry idt[256] __attribute__((aligned (8)));
struct idt_pointer idt_ptr;

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

static void
_setup_interrupt_gate(union idt_entry *entry, void *handler)
{
    uint32_t handler_as_int = (uint32_t) handler;

    entry->interrupt_gate.offset_lower     = handler_as_int & 0xFFFF;
    entry->interrupt_gate.offset_upper     = (handler_as_int >> 16) & 0xFFFF;
    entry->interrupt_gate.segment_selector = SEGMENT_KERNEL_CODE;
    entry->interrupt_gate.id_bits          = IDT_INTERRUPT_GATE;
    entry->interrupt_gate.gate_size        = GATE_SIZE_32BIT;
    entry->interrupt_gate.dpl              = DPL_KERNEL; /* change to DPL_USER when we start using userspace */
    entry->interrupt_gate.present          = 1;
}

void
_general_isr(struct registers regs)
{
    screen_write_string_at("Handling interrupt: 0x", 0, 1);
    screen_write_integer_at(regs.interrupt_no, 16, 22, 1);
}

#ifdef ROSE_TESTING
extern void _idt_set(struct idt_pointer *ptr);
#else
static void
_idt_set(struct idt_pointer *ptr)
{
    asm("MOV EAX, %0;"
        "LIDT [EAX];"
       :
       : "m"(ptr)
       : "eax"
       );
}
#endif

void
interrupts_init(void)
{
    /* zero-out all interrupt handlers */
    memset(idt, 0, sizeof(idt));

#define install_isr(n)\
    _setup_interrupt_gate(idt + n, isr##n)

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

    _idt_set(&idt_ptr);
}

void
interrupts_disable(void)
{
    asm volatile ("CLI"); /* disable maskable interrupts */
    io_outb(NMI_PORT, io_inb(NMI_PORT) | NMI_INTERRUPTS_OFF);
}

void
interrupts_enable(void)
{
    asm volatile ("STI"); /* enable maskable interrupts */
    io_outb(NMI_PORT, io_inb(NMI_PORT) & ~NMI_INTERRUPTS_OFF);
}
