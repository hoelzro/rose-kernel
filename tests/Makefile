ASFLAGS=-f elf
CFLAGS+=-m32 -masm=intel -Wall -Wextra -I../include -g -DROSE_TESTING=1

TESTS=$(shell ls verify-*.c | sed -e 's/.c$$//')

run: $(TESTS)
	# XXX verify that tests are properly built!
	@for test in $^; do \
	    echo ./$$test ; \
	    ./$$test ; \
	done

%.o: %.s
	nasm $(ASFLAGS) -o $@ $<

verify-gdt: verify-gdt.c ../src/memory.o
	gcc $(CFLAGS) -o $@ $^ -Wl,--allow-multiple-definition -Wl,verify-idt.ld

verify-idt: verify-idt.c ../src/interrupts.o
	gcc $(CFLAGS) -o $@ $^ -Wl,--allow-multiple-definition -Wl,verify-idt.ld

verify-stdarg: verify-stdarg.c ../src/stdarg.o
	gcc $(CFLAGS) -o $@ $^

verify-stdlib-string: verify-stdlib-string.c ../src/string.o
	gcc $(CFLAGS) -o $@ $^

verify-paging: verify-paging.c ../src/memory.o
	gcc $(CFLAGS) -o $@ $^ -Wl,--allow-multiple-definition

verify-stream: verify-stream.c ../src/stream.o ../src/stdarg.o
	gcc $(CFLAGS) -o $@ $^

verify-atomic: verify-atomic.c ../src/atomic.o
	gcc $(CFLAGS) -o $@ $^ -lpthread

verify-scheduler: verify-scheduler.c ../src/scheduler.o ../src/scheduler-helper.o
	gcc $(CFLAGS) -o $@ $^ -lpthread

clean:
	rm -f *.o verify-gdt verify-idt verify-stdarg verify-stdlib-string verify-paging verify-stream verify-atomic
