/* XXX license? */

ENTRY (start)
SECTIONS
{
    . = 0x00100000;

    .establish_higher_half ALIGN(0x1000) :
    {
        pre_higher_half = .;
        *(.establish_higher_half)
    }

    . += 0xC0000000;

    .text ALIGN (0x1000) :
      AT(ADDR(.text) - 0xC0000000)
    {
        code = .;
        *(.text)
    }

    .rodata ALIGN (0x1000) :
      AT(ADDR(.rodata) - 0xC0000000)
    {
        *(.rodata*)
    }

    .data ALIGN (0x1000) :
      AT(ADDR(.data) - 0xC0000000)
    {
        data = .;
        *(.data)
    }

    .bss :
      AT(ADDR(.bss) - 0xC0000000)
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
