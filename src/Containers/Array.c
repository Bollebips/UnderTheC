#ifndef ARRAY_C
#define ARRAY_C
#include "Array.h"

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

static const int INITIAL_CAPACITY = 16;
static const float GOLDEN_RATIO = 1.61803398875f;

/**
 * @brief Creates a new array, and initializes it.
 * @param elementSize The memory footprint of 1 element.
 * @param initialCapacity The initial element capacity to reserve.
 * @return Array* A pointer to the newly created array.
 */
Array* ArrayNew(size_t elementSize)
{
    Array* newArray = (Array*) malloc(sizeof(Array));

    assert(elementSize > 0);
    assert(newArray != NULL);

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
    assert(array != NULL);
    assert(newElement != NULL);

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
    assert(array != NULL);
    assert(array->num > 0);

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
    assert(array != NULL);
    assert(index < array->num);

    return (void*) array->elements + ((index) * array->elementSize);
}

/**
 * @brief Resize the array. If the new size is smaller than the previous size, the excess elements will be discarded. If the new size is bigger than the previous size, old pointers to elements might become corrupt.
 * @param array The array to resize.
 * @param numElements The new number of elements for the array to occupy.
 */
void ArrayResize(Array* array, const uint64_t numElements)
{
    assert(array != NULL);

    array->elements = realloc(array->elements, array->elementSize * (size_t) numElements);
    array->capacity = numElements;
    array->num = fmin(array->num, numElements);
}

/**
 * @brief Free the array.
 * @param array The array to free.
 */
void ArrayFree(Array* array)
{
    assert(array != NULL);

    free(array->elements);
    free(array);
}

/**
 * @brief Get the number of elements present in the array.
 * @param array The array to get the number of elements from.
 * @return uint64_t The number of elements in the array.
 */
uint64_t ArrayGetNum(const Array* array)
{
    assert(array != NULL);
    return array->num;
}

/**
 * @brief Get the number of elements the array can store, before having to increase in size.
 * @param array The array to get the capacity from.
 * @return uint64_t The capacity of the array.
 */
uint64_t ArrayGetCapacity(const Array* array)
{
    assert(array != NULL);
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
    assert(array != NULL);
    assert(elementSize > 0);

    array->num = 0;
    array->capacity = initialCapacity;
    array->elementSize = elementSize;
    array->elements = calloc(initialCapacity, elementSize);
}

#endif