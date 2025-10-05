#ifndef LIBC_STDARG_H
#define LIBC_STDARG_H

typedef char *va_list;

#define va_start(p, arg) (p = (va_list)&arg + sizeof(arg))

#define va_arg(p, type) (*(type *)((p += sizeof(type)) - sizeof(type)))

#define va_end(p) (p = 0)

#endif // LIBC_STDARG_H
