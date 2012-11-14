#include <assert.h>
#include <rose/stdarg.h>

int first;
int second;
int third;

void
variadic_setter(int a, ...)
{
    va_list args;

    va_start(args, a);

    first  = a;
    second = va_arg(args, int);
    third  = va_arg(args, int);

    va_end(args);
}

int
main(void)
{
    variadic_setter(10, 20, 30);

    assert(first  == 10);
    assert(second == 20);
    assert(third  == 30);

    variadic_setter(40, 50, 60);

    assert(first  == 40);
    assert(second == 50);
    assert(third  == 60);

    return 0;
}
