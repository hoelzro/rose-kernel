#ifndef ROSE_ATOMIC_H
#define ROSE_ATOMIC_H

#include <rose/stdint.h>

int atomic_compare_and_set8(uint8_t *location, uint8_t old_value, uint8_t new_value);
int atomic_compare_and_set16(uint16_t *location, uint16_t old_value, uint16_t new_value);
int atomic_compare_and_set32(uint32_t *location, uint32_t old_value, uint32_t new_value);
int atomic_compare_and_set64(uint64_t *location, uint64_t old_value, uint64_t new_value);

#endif
