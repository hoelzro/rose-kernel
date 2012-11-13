CFLAGS+=-m32 -Wall -Wextra -I../include

run: verify-gdt verify-idt
	./verify-gdt
	./verify-idt

verify-gdt: verify-gdt.c ../src/descriptor-tables.o
	gcc $(CFLAGS) -o $@ $^ -Wl,verify-idt.ld

verify-idt: verify-idt.c ../src/descriptor-tables.o
	gcc $(CFLAGS) -o $@ $^ -Wl,verify-idt.ld

clean:
	rm -f *.o verify-gdt verify-idt
