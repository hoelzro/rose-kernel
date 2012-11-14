#include <rose/stdarg.h>

void *
__va_arg(va_list *args, int size)
{
    void *return_value;

    return_value = *args;

    *args += size;

    return return_value;
}
