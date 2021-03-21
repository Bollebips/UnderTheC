#ifndef SPARSE_SET_I
#define SPARSE_SET_I

#include "BucketArray.h"

#include <stdbool.h>

typedef struct SparseSet
{
    BucketArray sparseData;
    BucketArray denseData;
    uint64_t(*getIndexFromDataFunc)(const void*);
}SparseSet;

SparseSet* SparseSetNew(const size_t elementSize, const uint64_t(*getIndexFromDataFunc)(const void*), const uint64_t bucketCapacity);
void SparseSetAdd(SparseSet* sparseSet, const void* newElement);
void SparseSetRemove(SparseSet* sparseSet, const uint64_t index);
void* SparseSetGet(SparseSet* sparseSet, const uint64_t index);
bool SparseSetContains(SparseSet* sparseSet, const uint64_t index);
void SparseSetFree(SparseSet* sparseSet);

BucketArray* SparseSetGetDenseData(SparseSet* sparseSet);

void SparseSetInit(SparseSet* sparseSet, const size_t elementSize, const uint64_t(*getIndexFromDataFunc)(const void*), uint64_t bucketCapacity);
void SparseSetDeinit(SparseSet* sparseSet);

#endif