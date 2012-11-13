QEMU=qemu-system-i386

all:
	make -C src kernel.bin

# XXX we should eventually throw GRUB in the mix
boot: all
	$(QEMU) -kernel src/kernel.bin

test:
	make -C tests

clean:
	make -C src clean
	make -C tests clean
