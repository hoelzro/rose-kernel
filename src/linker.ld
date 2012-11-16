ENTRY (start)
SECTIONS
{
    . = 0x00100000;

    .text ALIGN (0x1000) :
    {
        code = .;
        *(.text)
    }

    .rodata ALIGN (0x1000) :
    {
        *(.rodata*)
    }

    .data ALIGN (0x1000) :
    {
        data = .;
        *(.data)
    }

    .bss :
    {
        bss  = .;
        sbss = .;
        *(COMMON)
        *(.bss)
        ebss = .;
    }

    .     = . + 0x4000;
    stack = .;

    end = .;
}
