#ifndef BUCKETARRAY_I
#define BUCKETARRAY_I

#include "Array.h"

/**
 * @brief A dynamic array, that dynamically expands its memory footprint when necessary. This expanding happens in buckets, in order to prevent pointers to elements becoming corrupt.
 */
typedef struct BucketArray
{
    uint64_t num;               // The number of elements present in the bucketArray.
    uint64_t bucketCapacity;    // The maximum number of elements per bucket.
    size_t elementSize;         // The memory footprint of 1 element.
    Array bucketPtrs;           // A collection of pointers to the different buckets.
}BucketArray;

void BucketArrayInit(BucketArray* bucketArray, const size_t elementSize, const uint64_t bucketCapacity);
void BucketArrayDeinit(BucketArray* bucketArray);

void* BucketArrayGetBucket(BucketArray* bucketArray, const uint64_t bucketIndex);

BucketArray* BucketArrayNew(const size_t elementSize, const uint64_t bucketCapacity);
void* BucketArrayAdd(BucketArray* bucketArray, const void* newElement);
void BucketArrayPopBack(BucketArray* bucketArray, void* poppedElement);
void* BucketArrayGet(const BucketArray* bucketArray, const uint64_t index);
void BucketArrayResize(BucketArray* bucketArray, const uint64_t newCapacity);
void BucketArrayFill(BucketArray* array, void* value);
void BucketArrayClear(BucketArray* bucketArray);
void BucketArrayFree(BucketArray* bucketArray);

uint64_t BucketArrayNum(BucketArray* bucketArray);
uint64_t BucketArrayNumBuckets(BucketArray* bucketArray);
uint64_t BucketArrayCapacity(BucketArray* bucketArray);
uint64_t BucketArrayBucketCapacity(BucketArray* bucketArray);

#endif