QEMU=qemu-system-i386
QEMU_OPTS=-serial stdio

.PHONY: test tools

all: tools
	make -C src kernel.bin

# XXX we should eventually throw GRUB in the mix
boot: all
	$(QEMU) $(QEMU_OPTS) -kernel src/kernel.bin

tools:
	make -C tools

test:
	make -C tests

clean:
	make -C src clean
	make -C tests clean
	make -C tools clean
