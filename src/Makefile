ASFLAGS=-f elf
CFLAGS+=-m32 -march=i686 -mtune=generic -nostdlib \
        -fno-builtin -nostartfiles -nodefaultlibs -I../include \
        -masm=intel
CFLAG_WARNINGS+=-Wall -Wextra -Werror -Wno-error=unused-function

ifeq ($(NOWARNINGS),)
CFLAGS+=$(CFLAG_WARNINGS)
endif

KERNEL_PIECES=$(shell ls *.c *.s | sed -e 's/[cs]$$/o/')
KERNEL_PIECES+=struct-offsets.o

kernel.bin: $(KERNEL_PIECES)
	ld -T linker.ld -o $@ -m elf_i386 $^

struct-offsets.s: ../tools/gen-struct-macros ../include/rose/registers.h
	$< > $@

%.o: %.s
	nasm $(ASFLAGS) -o $@ $<

clean:
	rm -f *.o kernel.bin

