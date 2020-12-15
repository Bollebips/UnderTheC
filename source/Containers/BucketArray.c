#ifndef BUCKETARRAY_C
#define BUCKETARRAY_C
#include "BucketArray.h"

#include "Array.c"

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

static void BucketArrayInit(BucketArray* bucketArray, const size_t elementSize, const uint64_t bucketCapacity);
static void BucketArrayAddBucket(BucketArray* bucketArray);
static void BucketArrayPopBackBucket(BucketArray* bucketArray);

/**
 * @brief Creates a new array, and initializes it.
 * @param elementSize The memory footprint of 1 element.
 * @param bucketCapacity The number of elements a bucket can hold.
 * @return Array* A pointer to the newly created bucketArray.
 */
BucketArray* BucketArrayNew(const size_t elementSize, const uint64_t bucketCapacity)
{
    BucketArray* newBucketArray = malloc(sizeof(BucketArray));

    assert(newBucketArray != NULL);
    assert(elementSize > 0);
    assert(bucketCapacity > 0);

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
    assert(bucketArray != NULL);
    assert(newElement != NULL);

    if(bucketArray->num >= ArrayGetNum(&(bucketArray->bucketPtrs)) * bucketArray->bucketCapacity)
    {
        BucketArrayAddBucket(bucketArray);
    }

    void* currentBucket = *(void**) ArrayGet(&(bucketArray->bucketPtrs), ArrayGetNum(&(bucketArray->bucketPtrs)) - 1);

    uint64_t currentBucketNum = bucketArray->num % bucketArray->bucketCapacity;
    void* locationToSet = currentBucket + (currentBucketNum * bucketArray->elementSize);

    void* result = memcpy(locationToSet, newElement, bucketArray->elementSize);

    assert(result != NULL);

    bucketArray->num++;

    return result;
}

/**
 * @brief Remove the last element of the array.
 * @param  bucketArray: The bucketArray to remove from.
 * @param  poppedElement: Pointer to retreiving data, the get a copy of the element that was removed. This can be left to NULL if no returned value is requested.
 */
void BucketArrayPopBack(BucketArray* bucketArray, void* poppedElement)
{
    assert(bucketArray != NULL);
    assert(bucketArray->num > 0);

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
    if(currentBucketNum == 0 && ArrayGetNum(&(bucketArray->bucketPtrs)) > 1)
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
    assert(bucketArray != NULL);
    assert(index < bucketArray->num);

    uint64_t bucketIndex = floor((float) index / (float) bucketArray->bucketCapacity);
    uint64_t indexInBucket = index % bucketArray->bucketCapacity;

    void* bucket = *(void**) ArrayGet(&(bucketArray->bucketPtrs), bucketIndex);

    void* result = bucket + ((size_t) indexInBucket * bucketArray->elementSize);

    return result;
}

// TODO: BucketArrayResize(BucketArray* bucketArray, const uint64_t numElements)

/**
 * @brief Free the bucketArray.
 * @param bucketArray The bucketArray to free.
 */
void BucketArrayFree(BucketArray* bucketArray)
{
    assert(bucketArray != NULL);

    for(int i = 0; i < ArrayGetNum(&(bucketArray->bucketPtrs)); i++)
    {
        free(ArrayGet(&(bucketArray->bucketPtrs), i));
    }

    ArrayFree(&(bucketArray->bucketPtrs));

    free(bucketArray);
}

/**
 * @brief Get the number of elements present in the array.
 * @param bucketArray The bucketArray to get the number of elements from.
 * @return uint64_t The number of elements in the array.
 */
uint64_t BucketArrayGetNum(BucketArray* bucketArray)
{
    return bucketArray->num;
}

/**
 * @brief Get the number of buckets present in the array.
 * @param bucketArray The bucketArray to get the number of buckets from.
 * @return uint64_t The number of buckets in the array.
 */
uint64_t BucketArrayGetNumBuckets(BucketArray* bucketArray)
{
    return ArrayGetNum(&(bucketArray->bucketPtrs));
}

/* ------------------------------------------------------------------------------------------------------------------ */

/**
 * @brief Initialize an existing bucketArray. Only used internally. When calling BucketArrayNew, the array will already be initialized.
 * @param bucketArray The bucketArray to be initialized.
 * @param elementSize The memory footprint of 1 element.
 * @param bucketCapacity The number of elements a bucket can hold.
 */
static void BucketArrayInit(BucketArray* bucketArray, const size_t elementSize, const uint64_t bucketCapacity)
{
    assert(bucketArray != NULL);
    assert(elementSize > 0);

    bucketArray->elementSize = elementSize;
    bucketArray->num = 0;
    bucketArray->bucketCapacity = bucketCapacity;

    ArrayInit(&(bucketArray->bucketPtrs), sizeof(void*), 1);

    BucketArrayAddBucket(bucketArray);
}

/**
 * @brief Add a new bucket to the bucketArray.
 * @param bucketArray The bucketArray to add the bucket to.
 */
static void BucketArrayAddBucket(BucketArray* bucketArray)
{
    void* newBucket = calloc(bucketArray->bucketCapacity, bucketArray->elementSize);
    ArrayAdd(&(bucketArray->bucketPtrs), &newBucket);
}

/**
 * @brief Remove the last bucket from the bucketArray.
 * @param bucketArray The bucketArray to remove the bucket from.
 */
static void BucketArrayPopBackBucket(BucketArray* bucketArray)
{
    assert(bucketArray != NULL);
    assert(bucketArray->num > 0);

    void* lastElement;
    ArrayPopBack(&(bucketArray->bucketPtrs), &lastElement);

    free(lastElement);
}

#endif