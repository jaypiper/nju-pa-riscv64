#ifndef CSR_H
#define CSR_H

#define M_MODE 0b11
#define S_MODE 0b01
#define U_MODE 0b00

#include <encoding.h>
#define set_val(reg, mask, val) ((reg & ~(word_t)mask) | ((val * (mask & ~(mask << 1))) & (word_t)mask))
#define set_partial_val(reg, mask, val) ((reg & ~mask) | (val & mask))
#define get_val(reg, mask) ((reg & (word_t)mask) / ((mask) & ~((mask << 1))))
#endif
