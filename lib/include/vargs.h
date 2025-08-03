#ifndef LIB_VARGS_H
#define LIB_VARGS_H

typedef char *VA_LIST;

#define VA_START(p, arg) (p = (VA_LIST)&arg + sizeof(arg))

#define VA_ARG(p, type) (*(type *)((p += sizeof(type)) - sizeof(type)))

#define VA_END(p) (p = 0)

#endif // LIB_VARGS_H
