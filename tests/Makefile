CFLAGS+=-m32 -Wall -Wextra -I../include

run: verify-gdt verify-idt verify-stdarg
	@for test in $^; do \
	    echo ./$$test ; \
	    ./$$test ; \
	done

verify-gdt: verify-gdt.c ../src/descriptor-tables.o
	gcc $(CFLAGS) -o $@ $^ -Wl,verify-idt.ld

verify-idt: verify-idt.c ../src/descriptor-tables.o
	gcc $(CFLAGS) -o $@ $^ -Wl,verify-idt.ld

verify-stdarg: verify-stdarg.c ../src/stdarg.o
	gcc $(CFLAGS) -o $@ $^

clean:
	rm -f *.o verify-gdt verify-idt
