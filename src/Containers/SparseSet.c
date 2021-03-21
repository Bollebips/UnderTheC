#include "SparseSet.h"

#include "Logger.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

/**
 * @brief Creates a new Sparse set, and initializes it.
 * @param elementSize The memory footprint of 1 element.
 * @param getIndexFromDataFunc A function pointer to retreive an identifier or index from a given element.
 * @param setIndexInDataFunc
 * @param bucketCapacity
 * @return SparseSet*
 */
SparseSet* SparseSetNew(const size_t elementSize, const uint64_t(*getIndexFromDataFunc)(const void*), const uint64_t bucketCapacity)
{
    LogAssert(getIndexFromDataFunc != NULL);
    LogAssert(bucketCapacity > 0);

    SparseSet* newSparseSet = malloc(sizeof(SparseSet));
    LogAssert(newSparseSet != NULL);

    SparseSetInit(newSparseSet, elementSize, getIndexFromDataFunc, bucketCapacity);

    return newSparseSet;
}

void SparseSetAdd(SparseSet* sparseSet, const void* newElement)
{
    LogAssert(sparseSet != NULL);
    LogAssert(newElement != NULL);

    uint64_t index = sparseSet->getIndexFromDataFunc(newElement);

    if(index >= BucketArrayNum(&(sparseSet->sparseData)))
    {
        BucketArrayResize(&(sparseSet->sparseData), index);
        uint64_t firstBucketIndexToSet = ceil(BucketArrayNum(&(sparseSet->sparseData)) / BucketArrayBucketCapacity(&(sparseSet->sparseData)));

        for(int i = firstBucketIndexToSet; i < BucketArrayNumBuckets(&(sparseSet->sparseData)); i++)
        {
            void* bucket = BucketArrayGetBucket(&(sparseSet->sparseData), i);
            memset(bucket, 0, BucketArrayBucketCapacity(&(sparseSet->sparseData)) * sizeof(uint64_t));
        }
    }

    if(SparseSetContains(sparseSet, index))
    {
        return;
    }

    uint64_t elementIndexInDenseData = BucketArrayNum(&(sparseSet->denseData));

    BucketArrayAdd(&(sparseSet->denseData), newElement);

    uint64_t* elementInSparseData = (uint64_t*) BucketArrayGet(&(sparseSet->sparseData), index);
    *elementInSparseData = elementIndexInDenseData;
}

void SparseSetRemove(SparseSet* sparseSet, const uint64_t index)
{
    LogAssert(sparseSet != NULL);

    uint64_t oldDenseIndex = *(uint64_t*) BucketArrayGet(&(sparseSet->sparseData), index);
    LogAssert(oldDenseIndex < BucketArrayNum(&(sparseSet->denseData)));

    if(!SparseSetContains(sparseSet, oldDenseIndex))
    {
        return;
    }

    if(oldDenseIndex != BucketArrayNum(&(sparseSet->denseData)) - 1)
    {
        void* oldDenseElement = BucketArrayGet(&(sparseSet->denseData), oldDenseIndex);
        void* lastDenseElement = BucketArrayGet(&(sparseSet->denseData), BucketArrayNum(&(sparseSet->denseData)) - 1);
        memcpy(oldDenseElement, lastDenseElement, sparseSet->denseData.elementSize);

        uint64_t* lastDenseElementNewSparseIndex = (uint64_t*) BucketArrayGet(&(sparseSet->sparseData), sparseSet->getIndexFromDataFunc(lastDenseElement));
        *lastDenseElementNewSparseIndex = oldDenseIndex;
    }

    BucketArrayPopBack(&(sparseSet->denseData), NULL);
}

void* SparseSetGet(SparseSet* sparseSet, const uint64_t index)
{
    LogAssert(sparseSet);

    uint64_t denseIndex = *(uint64_t*) BucketArrayGet(&(sparseSet->sparseData), index);
    return BucketArrayGet(&(sparseSet->denseData), denseIndex);
}

bool SparseSetContains(SparseSet* sparseSet, const uint64_t index)
{
    LogAssert(sparseSet != NULL);

    if(index >= BucketArrayNum(&(sparseSet->sparseData)))
    {
        return false;
    }

    uint64_t indexInDenseData = *(uint64_t*) BucketArrayGet(&(sparseSet->sparseData), index);

    if(indexInDenseData >= BucketArrayNum(&(sparseSet->denseData)))
    {
        return false;
    }

    void* elementInDenseData = BucketArrayGet(&(sparseSet->denseData), indexInDenseData);

    uint64_t indexInSparseData = sparseSet->getIndexFromDataFunc(elementInDenseData);

    return index == indexInSparseData;
}

void SparseSetFree(SparseSet* sparseSet)
{
    LogAssert(sparseSet != NULL);

    SparseSetDeinit(sparseSet);
    free(sparseSet);
}

BucketArray* SparseSetGetDenseData(SparseSet* sparseSet)
{
    LogAssert(sparseSet != NULL);
    return &(sparseSet->denseData);
}

void SparseSetInit(SparseSet* sparseSet, const size_t elementSize, const uint64_t(*getIndexFromDataFunc)(const void*), uint64_t bucketCapacity)
{
    LogAssert(sparseSet != NULL);
    LogAssert(bucketCapacity > 0);

    BucketArrayInit(&(sparseSet->denseData), elementSize, bucketCapacity);
    BucketArrayInit(&(sparseSet->sparseData), sizeof(uint64_t), bucketCapacity);

    uint64_t emptyIndex = 0;
    BucketArrayFill(&(sparseSet->sparseData), &emptyIndex);

    sparseSet->getIndexFromDataFunc = getIndexFromDataFunc;
}

void SparseSetDeinit(SparseSet* sparseSet)
{
    LogAssert(sparseSet != NULL);

    BucketArrayDeinit(&(sparseSet->denseData));
    BucketArrayDeinit(&(sparseSet->sparseData));
}

