CFLAGS+=-m32 -masm=intel -Wall -Wextra -I../include -g -DROSE_TESTING=1

TESTS=$(shell ls verify-*.c | sed -e 's/.c$$//')

run: $(TESTS)
	@for test in $^; do \
	    echo ./$$test ; \
	    ./$$test ; \
	done

verify-gdt: verify-gdt.c ../src/memory.o
	gcc $(CFLAGS) -o $@ $^ -Wl,verify-idt.ld

verify-idt: verify-idt.c ../src/interrupts.o
	gcc $(CFLAGS) -o $@ $^ -Wl,verify-idt.ld

verify-stdarg: verify-stdarg.c ../src/stdarg.o
	gcc $(CFLAGS) -o $@ $^

verify-stdlib-string: verify-stdlib-string.c ../src/string.o
	gcc $(CFLAGS) -o $@ $^

clean:
	rm -f *.o verify-gdt verify-idt
