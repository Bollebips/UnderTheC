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
// TODO: Add a get function for a specific index, maybe? Not sure if this is something we should support, as the sparse set is meant for fast iteration primarily.
bool SparseSetContains(SparseSet* sparseSet, const uint64_t index);
void SparseSetFree(SparseSet* sparseSet);

BucketArray* SparseSetGetDenseData(SparseSet* sparseSet);

void SparseSetInit(SparseSet* sparseSet, const size_t elementSize, const uint64_t(*getIndexFromDataFunc)(const void*), uint64_t bucketCapacity);
void SparseSetDeinit(SparseSet* sparseSet);

#endif