#ifndef ROSE_STDARG_H
#define ROSE_STDARG_H

typedef void *va_list;

void *__va_arg(va_list *args, int size);

#ifdef __x86_64__
# error "64-bit builds are not yet supported"
#else

#define va_start(args, last)\
    ((args) = (void *) (&(last) + 1))

#endif

#define va_end(args)

#define va_arg(args, type)\
    (*((type *) __va_arg(&args, sizeof(type))))


#endif
