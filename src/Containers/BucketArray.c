#include "BucketArray.h"

#include "Logger.h"

#include <assert.h>
#include <string.h>
#include <math.h>

static void* BucketArrayAddBucket(BucketArray* bucketArray);
static void BucketArrayPopBackBucket(BucketArray* bucketArray);

/**
 * @brief Creates a new array, and initializes it.
 * @param elementSize The memory footprint of 1 element.
 * @param bucketCapacity The number of elements a bucket can hold.
 * @return Array* A pointer to the newly created bucketArray.
 */
BucketArray* BucketArrayNew(const size_t elementSize, const uint64_t bucketCapacity)
{
    LogAssert(elementSize > 0);
    LogAssert(bucketCapacity > 0);

    BucketArray* newBucketArray = malloc(sizeof(BucketArray));

    LogAssert(newBucketArray != NULL);
    LogAssert(elementSize > 0);
    LogAssert(bucketCapacity > 0);

    BucketArrayInit(newBucketArray, elementSize, bucketCapacity);

    return newBucketArray;
}

/**
 * @brief Add a new element to the back of the array. If the current bucket is full, a new bucket will be allocated.
 * @param bucketArray The bucketArray to add the element to.
 * @param newElement A pointer to the data to add.
 * @return void* A pointer to the new element in the array.
 */
void* BucketArrayAdd(BucketArray* bucketArray, const void* newElement)
{
    LogAssert(bucketArray != NULL);
    LogAssert(newElement != NULL);

    if(bucketArray->num >= ArrayNum(&(bucketArray->bucketPtrs)) * bucketArray->bucketCapacity)
    {
        BucketArrayAddBucket(bucketArray);
    }

    void* currentBucket = *(void**) ArrayGet(&(bucketArray->bucketPtrs), ArrayNum(&(bucketArray->bucketPtrs)) - 1);

    uint64_t currentBucketNum = bucketArray->num % bucketArray->bucketCapacity;
    void* locationToSet = currentBucket + (currentBucketNum * bucketArray->elementSize);

    void* result = memcpy(locationToSet, newElement, bucketArray->elementSize);

    LogAssert(result != NULL);

    bucketArray->num++;

    return result;
}

/**
 * @brief Remove the last element of the array.
 * @param  bucketArray: The bucketArray to remove from.
 * @param  poppedElement: Pointer to retrieving data, the get a copy of the element that was removed. This can be left to NULL if no returned value is requested.
 */
void BucketArrayPopBack(BucketArray* bucketArray, void* poppedElement)
{
    LogAssert(bucketArray != NULL);
    LogAssert(bucketArray->num > 0);

    void* lastElement = BucketArrayGet(bucketArray, bucketArray->num - 1);

    if(poppedElement != NULL)
    {
        memcpy(poppedElement, lastElement, bucketArray->elementSize);
    }

    memset(lastElement, 0, bucketArray->elementSize);

    bucketArray->num--;

    uint64_t currentBucketNum = bucketArray->num % bucketArray->bucketCapacity;

    /* TODO: Make the bucketArray preserve an extra bucket when removing elements.
        this prevents buckets from constantly being added and removed when adding and remove elements onto a full bucket.
    */
    if(currentBucketNum == 0 && ArrayNum(&(bucketArray->bucketPtrs)) > 1)
    {
        BucketArrayPopBackBucket(bucketArray);
    }
}

/**
 * @brief Retrieve an element at a specific index.
 * @param bucketArray The bucketArray to retrieve the element from.
 * @param index The index at which to find the element
 * @return void* A pointer to the requested element.
 */
void* BucketArrayGet(const BucketArray* bucketArray, const uint64_t index)
{
    LogAssert(bucketArray != NULL);
    LogAssert(index < bucketArray->num);

    uint64_t bucketIndex = floor((float) index / (float) bucketArray->bucketCapacity);
    uint64_t indexInBucket = index % bucketArray->bucketCapacity;

    void* bucket = *(void**) ArrayGet(&(bucketArray->bucketPtrs), bucketIndex);

    void* result = bucket + ((size_t) indexInBucket * bucketArray->elementSize);

    return result;
}

/**
 * @brief Resize the bucketArray. If the new size is smaller than the previous size, the excess elements will be discarded.
 * @param bucketArray The bucketArray to be resized.
 * @param newCapacity The new number of elements the bucketArray can occupy before having to allocate more memory.
 */
void BucketArrayResize(BucketArray* bucketArray, const uint64_t newCapacity)
{
    LogAssert(bucketArray != NULL);

    if(newCapacity == BucketArrayCapacity(bucketArray))
    {
        return;
    }

    uint64_t numBuckets = ceil((float) newCapacity / (float) bucketArray->bucketCapacity);

    int prevNumBucketsCurrentNumBucketsDiff = abs(bucketArray->bucketPtrs.num - numBuckets);

    if(bucketArray->bucketPtrs.num > numBuckets)
    {
        for(int i = 0; i < prevNumBucketsCurrentNumBucketsDiff; ++i)
        {
            BucketArrayPopBackBucket(bucketArray);
        }
    }
    else if(bucketArray->bucketPtrs.num < numBuckets)
    {
        for(int i = 0; i < prevNumBucketsCurrentNumBucketsDiff; ++i)
        {
            BucketArrayAddBucket(bucketArray);
        }
    }

    ArrayResize(&(bucketArray->bucketPtrs), numBuckets);

    if(numBuckets > 0)
    {
        uint64_t numElementsInLastBucket = newCapacity % bucketArray->bucketCapacity;
        void* lastBucket = *(void**) ArrayGet(&(bucketArray->bucketPtrs), ArrayNum(&(bucketArray->bucketPtrs)) - 1);
        memset(lastBucket + (numElementsInLastBucket * bucketArray->elementSize), 0, bucketArray->bucketCapacity - numElementsInLastBucket);
    }

    bucketArray->num = fmin(bucketArray->num, newCapacity);
}

void BucketArrayFill(BucketArray* bucketArray, void* value)
{
    LogAssert(bucketArray != NULL);
    LogAssert(value != NULL);

    uint64_t capacity = BucketArrayCapacity(bucketArray);

    bucketArray->num = capacity;

    for(int i = 0; i < capacity; ++i)
    {
        void* locationToSet = BucketArrayGet(bucketArray, i);
        memcpy(locationToSet, value, bucketArray->elementSize);
    }
}

/**
 * @brief Remove all elements the bucketArray. Doesn't change the capacity of the bucketArray.
 * @param bucketArray The bucketArray to be cleared.
 */
void BucketArrayClear(BucketArray* bucketArray)
{
    LogAssert(bucketArray != NULL);

    for(int i = 0; i < ArrayNum(&(bucketArray->bucketPtrs)); i++)
    {
        void* bucket = *(void**) ArrayGet(&(bucketArray->bucketPtrs), i);
        memset(bucket, 0, bucketArray->bucketCapacity);
    }

    bucketArray->num = 0;
}

/**
 * @brief Free the bucketArray.
 * @param bucketArray The bucketArray to free.
 */
void BucketArrayFree(BucketArray* bucketArray)
{
    LogAssert(bucketArray != NULL);

    BucketArrayDeinit(bucketArray);

    free(bucketArray);
}

/**
 * @brief Get the number of elements present in the array.
 * @param bucketArray The bucketArray to get the number of elements from.
 * @return uint64_t The number of elements in the array.
 */
uint64_t BucketArrayNum(BucketArray* bucketArray)
{
    LogAssert(bucketArray != NULL);
    return bucketArray->num;
}

/**
 * @brief Get the number of buckets present in the array.
 * @param bucketArray The bucketArray to get the number of buckets from.
 * @return uint64_t The number of buckets in the array.
 */
uint64_t BucketArrayNumBuckets(BucketArray* bucketArray)
{
    LogAssert(bucketArray != NULL);
    return ArrayNum(&(bucketArray->bucketPtrs));
}

/**
 * @brief Get the maximum number of elements the bucketArray can store, before having to allocate more memory.
 * @param bucketArray The bucketArray to get the capacity from.
 * @return uint64_t The capacity of the bucketArray.
 */
uint64_t BucketArrayCapacity(BucketArray* bucketArray)
{
    LogAssert(bucketArray != NULL);
    return bucketArray->bucketCapacity * ArrayNum(&(bucketArray->bucketPtrs));
}

/**
 * @brief Get the maximum number of elements a single bucket from this bucketArray can store.
 * @param bucketArray The bucketArray to get the bucket capacity from.
 * @return uint64_t The bucket capacity of the bucketArray.
 */
uint64_t BucketArrayBucketCapacity(BucketArray* bucketArray)
{
    LogAssert(bucketArray != NULL);
    return bucketArray->bucketCapacity;
}

/* ---------------------------------------------------- Internal ---------------------------------------------------- */

/**
 * @brief Initialize an existing bucketArray. Only used internally. When calling BucketArrayNew, the array will already be initialized.
 * @param bucketArray The bucketArray to be initialized.
 * @param elementSize The memory footprint of 1 element.
 * @param bucketCapacity The number of elements a bucket can hold.
 */
void BucketArrayInit(BucketArray* bucketArray, const size_t elementSize, const uint64_t bucketCapacity)
{
    LogAssert(bucketArray != NULL);
    LogAssert(elementSize > 0);
    LogAssert(bucketCapacity > 0);

    bucketArray->elementSize = elementSize;
    bucketArray->num = 0;
    bucketArray->bucketCapacity = bucketCapacity;

    ArrayInit(&(bucketArray->bucketPtrs), sizeof(void*), 1);

    BucketArrayAddBucket(bucketArray);
}

/**
 * @brief Deinitialize the bucketArray. This does not free the bucketArray pointer. Use this function instead of free if the bucketArray is stack allocated or allocated locally as a struct member.
 * @param bucketArray The bucketArray to deinitialize.
 */
void BucketArrayDeinit(BucketArray* bucketArray)
{
    LogAssert(bucketArray != NULL);

    for(int i = 0; i < ArrayNum(&(bucketArray->bucketPtrs)); i++)
    {
        void* memoryToFree = *(void**) ArrayGet(&(bucketArray->bucketPtrs), i);
        LogAssert(memoryToFree != NULL);
        free(memoryToFree);
    }

    ArrayDeinit(&(bucketArray->bucketPtrs));
}

/**
 * @brief Retrieve a specific bucket from a bucket array.
 * @param bucketArray The bucket array to retrieve the bucket from.
 * @param bucketIndex The index of the bucket.
 * @return void* A pointer to the bucket. This is a pointer to that sub-array.
 */
void* BucketArrayGetBucket(BucketArray* bucketArray, const uint64_t bucketIndex)
{
    LogAssert(bucketArray != NULL);
    LogAssert(bucketIndex < ArrayNum(&(bucketArray->bucketPtrs)));

    return *(void**) ArrayGet(&(bucketArray->bucketPtrs), bucketIndex);
}

/* ----------------------------------------------------- Private ---------------------------------------------------- */

/**
 * @brief Add a new bucket to the bucketArray.
 * @param bucketArray The bucketArray to add the bucket to.
 * @return void* A pointer to the newly added bucket.
 */
static void* BucketArrayAddBucket(BucketArray* bucketArray)
{
    LogAssert(bucketArray != NULL);

    void* newBucket = calloc(bucketArray->bucketCapacity, bucketArray->elementSize);
    ArrayAdd(&(bucketArray->bucketPtrs), &newBucket);
    return newBucket;
}

/**
 * @brief Remove the last bucket from the bucketArray.
 * @param bucketArray The bucketArray to remove the bucket from.
 */
static void BucketArrayPopBackBucket(BucketArray* bucketArray)
{
    LogAssert(bucketArray != NULL);
    LogAssert(bucketArray->num > 0);

    void* bucket;
    ArrayPopBack(&(bucketArray->bucketPtrs), &bucket);

    free(bucket);
}