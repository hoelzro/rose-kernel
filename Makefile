QEMU=qemu-system-i386

all:
	make -C src kernel.bin

# XXX we should eventually throw GRUB in the mix
boot: all
	$(QEMU) -kernel src/kernel.bin

clean:
	make -C src clean
