ASFLAGS=-f elf
CFLAGS+=-m32 -march=i686 -mtune=generic -Wall -Wextra -Werror -nostdlib \
        -fno-builtin -nostartfiles -nodefaultlibs -Wno-error=unused-function \
	-I../include

kernel.bin: loader.o kernel.o screen.o
	ld -T linker.ld -o $@ -m elf_i386 $^

%.o: %.s
	nasm $(ASFLAGS) -o $@ $<

clean:
	rm -f *.o kernel.bin
