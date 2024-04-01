#include "Hash.h"

#include "Logger.h"

static const uint64_t HASH_PRIME_64 = 1099511628211U; // 2^40 + 2^8 + 0xb3
static const uint64_t HASH_OFFSET_64 = 14695981039346656037U;

/**
 * @brief Returns a 64 bit hash, computed from the given data.
 * @param data The data to be hashed.
 * @param dataSize The memory footprint of the data.
 * @return uint64_t A semi-unique hash, computed from the given data.
 */
uint64_t HashFNV1a64(const void* data, const size_t dataSize)
{
    LogAssert(data != NULL);
    LogAssert(dataSize > 0);

    uint8_t* dataBytes = (uint8_t*) data; // The data gets converted to an array of uint8.

    uint64_t hash = HASH_OFFSET_64;

    for(int i = 0; i < dataSize; ++i)
    {
        hash = hash ^ dataBytes[i];
        hash *= HASH_PRIME_64;
    }

    return hash;
}