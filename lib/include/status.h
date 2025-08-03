#ifndef LIB_STATUS_H
#define LIB_STATUS_H

enum Status : short int {
    ALL_OK = 0,
    EIO = -1,
    EINVARG = -2,
    ENOMEM = -3
};

#endif // LIB_STATUS_H
