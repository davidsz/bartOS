#ifndef LIB_STATUS_H
#define LIB_STATUS_H

enum Status : short int {
    ALL_OK = 0,
    E_IO = -1,
    E_INVALID_ARGUMENT = -2,
    E_NO_MEMORY = -3,
    E_BAD_PATH = -4,
    E_CANT_ACCESS = -5,
    E_NOT_IMPLEMENTED = -6,
};

#endif // LIB_STATUS_H
