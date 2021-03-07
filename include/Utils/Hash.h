#ifndef HASH_H
#define HASH_H

#include <stdint.h>

uint64_t HashFNV1a64(const void* data, const size_t dataSize);

#endif