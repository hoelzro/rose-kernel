#include <rose/console.h>
#include <rose/screen.h>
#include <rose/stream.h>
#include <rose/stdint.h>

#define COLS 80
#define ROWS 25

struct console_stream {
    struct stream stream;

    int row;
    int column;
};

static int
_console_write_char(struct console_stream *stream, char c)
{
    switch(c) {
        case '\n':
            stream->row++;
        case '\r':
            stream->column = 0;
            break;
        default:
            screen_write_char_at(c, stream->column, stream->row);
            stream->column++;

            if(stream->column > COLS) {
                stream->column = 0;
                stream->row++;
            }
    }
    while(stream->row >= ROWS) {
        screen_scroll_up();
        stream->row--;
    }

    return 1;
}

struct console_stream stream = {
    .stream = {
        .write_char = (int (*)(struct stream *, char)) _console_write_char,
    },
    .row    = 0,
    .column = 0,
};

static struct stream *
_get_stream(void)
{
    return (struct stream *) &stream;
}

int
console_write_char(char c)
{
    return stream_write_char(_get_stream(), c);
}

int
console_write_string(const char *s)
{
    return stream_write_string(_get_stream(), s);
}

int
console_write_integer(int radix, int n)
{
    return stream_write_integer(_get_stream(), radix, n);
}

int
console_printf(const char *fmt, ...)
{
    va_list args;
    int chars_written;

    va_start(args, fmt);

    chars_written = stream_vprintf(_get_stream(), fmt, args);
    va_end(args);

    return chars_written;
}

struct registers {
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
};

void
_console_dump_registers(struct registers *regs)
{
    uint32_t eflags;
    uint32_t cr0;
    uint32_t cr2;
    uint32_t cr3;
    uint32_t cr4;

    asm("PUSHFD;"
        "MOV %0, [ESP];"
        "MOV %1, CR0;"
        "MOV %2, CR2;"
        "MOV %3, CR3;"
        "MOV %4, CR4;"
        "ADD ESP, 4;"
       : "=r"(eflags), "=r"(cr0), "=r"(cr2), "=r"(cr3), "=r"(cr4)
       );

    console_printf("EAX:    0x%x\n", regs->eax);
    console_printf("EBX:    0x%x\n", regs->ebx);
    console_printf("ECX:    0x%x\n", regs->ecx);
    console_printf("EDX:    0x%x\n", regs->edx);
    console_printf("ESP:    0x%x\n", regs->esp);
    console_printf("EBP:    0x%x\n", regs->ebp);
    console_printf("ESI:    0x%x\n", regs->esi);
    console_printf("EDI:    0x%x\n", regs->edi);
    console_printf("EFLAGS: 0x%x\n", eflags);
    console_printf("CR0:    0x%x\n", cr0);
    console_printf("CR2:    0x%x\n", cr2);
    console_printf("CR3:    0x%x\n", cr3);
    console_printf("CR4:    0x%x\n", cr4);
}
