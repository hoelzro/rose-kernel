QEMU=qemu-system-i386
QEMU_OPTS=-serial stdio

all:
	make -C src kernel.bin

# XXX we should eventually throw GRUB in the mix
boot: all
	$(QEMU) $(QEMU_OPTS) -kernel src/kernel.bin

test:
	make -C tests

clean:
	make -C src clean
	make -C tests clean
