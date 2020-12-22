#ifndef BUCKETARRAY_H
#define BUCKETARRAY_H

#include <stdint.h>

/**
 * @brief A dynamic array, that dynamically expands its memory footprint when necessary. This expanding happens in buckets, in order to prevent pointers to elements becoming corrupt.
 */
typedef struct BucketArray BucketArray;

BucketArray* BucketArrayNew(const size_t elementSize, const uint64_t bucketCapacity);
void* BucketArrayAdd(BucketArray* bucketArray, const void* newElement);
void BucketArrayPopBack(BucketArray* bucketArray, void* poppedElement);
void* BucketArrayGet(const BucketArray* bucketArray, const uint64_t index);
void BucketArrayFree(BucketArray* bucketArray);

uint64_t BucketArrayNum(BucketArray* bucketArray);
uint64_t BucketArrayNumBuckets(BucketArray* bucketArray);

#endif