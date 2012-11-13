CFLAGS+=-m32 -Wall -Wextra -I../include

run: verify-gdt
	./verify-gdt

verify-gdt: verify-gdt.c ../src/descriptor-tables.o
	gcc $(CFLAGS) -o $@ $^

clean:
	rm -f *.o verify-gdt
