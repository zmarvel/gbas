#ifndef SWAP_HELPERS_HPP
#define SWAP_HELPERS_HPP

#include <elf.h>

template <typename T>
T swap(T value);

template <>
uint16_t swap(uint16_t value) {
  return __builtin_bswap16(value);
}

template <>
uint32_t swap(uint32_t value) {
  return __builtin_bswap32(value);
}

#endif // SWAP_HELPERS_HPP
