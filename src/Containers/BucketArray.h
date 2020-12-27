#ifndef BUCKETARRAY_I
#define BUCKETARRAY_I

#include "../../include/Containers/BucketArray.h"
#include "Array.h"

/**
 * @brief A dynamic array, that dynamically expands its memory footprint when necessary. This expanding happens in buckets, in order to prevent pointers to elements becoming corrupt.
 */
struct BucketArray
{
    uint64_t num;               // The number of elements present in the bucketArray.
    uint64_t bucketCapacity;    // The maximum number of elements per bucket.
    size_t elementSize;         // The memory footprint of 1 element.
    Array bucketPtrs;           // A collection of pointers to the different buckets.
};

void BucketArrayInit(BucketArray* bucketArray, const size_t elementSize, const uint64_t bucketCapacity);
void BucketArrayDeinit(BucketArray* bucketArray);

#endif