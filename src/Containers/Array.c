#include "Array.h"

#include "Math/Math.h"
#include "Logger.h"

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

static const int INITIAL_CAPACITY = 16;

/**
 * @brief Creates a new array, and initializes it.
 * @param elementSize The memory footprint of 1 element.
 * @param initialCapacity The initial element capacity to reserve.
 * @return Array* A pointer to the newly created array.
 */
Array* ArrayNew(size_t elementSize)
{
    Array* newArray = (Array*) malloc(sizeof(Array));

    LogAssert(elementSize > 0);
    LogAssert(newArray != NULL);

    ArrayInit(newArray, elementSize, INITIAL_CAPACITY);

    return newArray;
}

/**
 * @brief Add a new element to the back of the array. If this causes the array to resize, old pointers to elements might become corrupt.
 * @param array The array to add the element to.
 * @param newElement A pointer to the data to add.
 * @return void* A pointer to the new element in the array.
 */
void* ArrayAdd(Array* array, const void* newElement)
{
    LogAssert(array != NULL);
    LogAssert(newElement != NULL);

    if(array->capacity <= array->num)   // INCREASE CAPACITY
    {
        if(array->capacity <= 0)
        {
            array->capacity = 1;
        }
        else
        {
            int newCapacity = (int) round((float) array->capacity * GOLDEN_RATIO);
            ArrayResize(array, newCapacity);
        }
    }

    void* locationToSet = array->elements + ((size_t) array->num * array->elementSize);
    void* result = memcpy(locationToSet, newElement, array->elementSize);

    array->num++;

    return result;
}

/**
 * @brief Remove the last element of the array.
 * @param  array: The array to remove from.
 * @param  poppedElement: Pointer to retreiving data, the get a copy of the element that was removed. This can be left to NULL if no returned value is requested.
 */
void ArrayPopBack(Array* array, void* poppedElement)
{
    LogAssert(array != NULL);
    LogAssert(array->num > 0);

    void* lastElement = ArrayGet(array, array->num - 1);

    if(poppedElement != NULL)
    {
        memcpy(poppedElement, lastElement, array->elementSize);
    }

    memset(lastElement, 0, array->elementSize);

    array->num--;
}


/**
 * @brief Retrieve an element at a specific index.
 * @param array The array to retrieve the element from.
 * @param index The index at which to find the element
 * @return void* A pointer to the requested element.
 */
void* ArrayGet(const Array* array, const uint64_t index)
{
    LogAssert(array != NULL);
    LogAssert(index < array->num);

    return (void*) array->elements + ((index) * array->elementSize);
}

/**
 * @brief Resize the array. If the new size is smaller than the previous size, the excess elements will be discarded. If the new size is bigger than the previous size, old pointers to elements might become corrupt.
 * @param array The array to resize.
 * @param numElements The new number of elements for the array to occupy.
 */
void ArrayResize(Array* array, const uint64_t numElements)
{
    LogAssert(array != NULL);

    if(numElements == array->num)
    {
        return;
    }

    array->elements = realloc(array->elements, array->elementSize * (size_t) numElements);
    array->capacity = numElements;
    array->num = fmin(array->num, numElements);
}

/**
 * @brief Remove all elements from the array. Doesn't change the array's capacity.
 * @param array The array to be cleared.
 */
void ArrayClear(Array* array)
{
    LogAssert(array != NULL);

    memset(array->elements, 0, array->elementSize * array->capacity);
    array->num = 0;
}

/**
 * @brief Free the array.
 * @param array The array to free.
 */
void ArrayFree(Array* array)
{
    LogAssert(array != NULL);

    ArrayDeinit(array);
    free(array);
}

/**
 * @brief Get the number of elements present in the array.
 * @param array The array to get the number of elements from.
 * @return uint64_t The number of elements in the array.
 */
uint64_t ArrayNum(const Array* array)
{
    LogAssert(array != NULL);
    return array->num;
}

/**
 * @brief Get the number of elements the array can store, before having to increase in size.
 * @param array The array to get the capacity from.
 * @return uint64_t The capacity of the array.
 */
uint64_t ArrayCapacity(const Array* array)
{
    LogAssert(array != NULL);
    return array->capacity;
}

/* ------------------------------------------------------------------------------------------------------------------ */

/**
 * @brief Initialize an existing array. Only used internally. When calling ArrayNew, the array will already be initialized.
 * @param array The array to be initialized.
 * @param elementSize The memory footprint of 1 element.
 * @param initialCapacity The initial element capacity to reserve.
 */
void ArrayInit(Array* array, size_t elementSize, const uint64_t initialCapacity)
{
    LogAssert(array != NULL);
    LogAssert(elementSize > 0);

    array->num = 0;
    array->capacity = initialCapacity;
    array->elementSize = elementSize;
    array->elements = calloc(initialCapacity, elementSize);
}

/**
 * @brief Deinitialize the array. This does not free the array pointer. Use this function instead of free if the array is stack allocated or allocated locally as a struct member.
 * @param array The array to deinitialize.
 */
void ArrayDeinit(Array* array)
{
    LogAssert(array != NULL);

    free(array->elements);
}