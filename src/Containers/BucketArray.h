#ifndef BUCKETARRAY_I
#define BUCKETARRAY_I

#include "../../include/Containers/BucketArray.h"
#include "Array.h"

/**
 * @brief A dynamic array, that dynamically expands its memory footprint when necessary. This expanding happens in buckets, in order to prevent pointers to elements becoming corrupt.
 */
struct BucketArray
{
    uint64_t num;               // The maximum number of occupied elements before the array has to allocate more memory.
    uint64_t bucketCapacity;    // The maximum number of elements per bucket.
    size_t elementSize;         // The memory footprint of 1 element.
    Array bucketPtrs;           // A collection of pointers to the different buckets.
};

void BucketArrayInit(BucketArray* bucketArray, const size_t elementSize, const uint64_t bucketCapacity);

#endif