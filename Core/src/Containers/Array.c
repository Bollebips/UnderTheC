#include "Array.h"
#include "Defines.h"
#include "Logger.h"

#include <math.h>
#include <stdlib.h>

/**
 * @brief Initialize an existing array. Only used internally. When calling
 * ArrayNew, the array will already be initialized.
 * @param array The array to be initialized.
 * @param elementSize The memory footprint of 1 element.
 * @param initialCapacity The initial element capacity to reserve.
 */
// Array ArrayNew(size_t elementSize)
//{
//     LogAssert(elementSize > 0);
//
//     Array array = {.length = 0, .capacity = INITIAL_CAPACITY, .elementSize =
//     elementSize, .elements = calloc(array.capacity, elementSize)};
//
//     return array;
// }

/**
 * @brief Deinitialize the array. This does not free the array pointer. Use this
 * function instead of free if the array is stack allocated or allocated locally
 * as a struct member.
 * @param array The array to deinitialize.
 */
// void ArrayFree(Array* array)
//{
//     LogAssert(array != NULL);
//     free(array->elements);
// }

// void* ArrayAdd(Array* array, const void* newElement)
//{
//     LogAssert(array != NULL);
//     LogAssert(newElement != NULL);
//
//     if (array->length >= array->capacity)
//     {
//         if (array->capacity == 0)
//             array->capacity = INITIAL_CAPACITY;
//         else
//             array->capacity *= 2;
//
//         // ArrayResize(array, array->capacity);
//         array->elements = reallocarray(array->elements, array->elementSize,
//         array->capacity);
//         // array->elements = realloc(array->elements, array->elementSize *
//         (size_t)
//         // newCapacity);
//     }
//
//     void* locationToSet = array->elements + ((size_t)array->length *
//     array->elementSize); void* result = memcpy(locationToSet, newElement,
//     array->elementSize);
//
//     array->length++;
//
//     return result;
// }

/**
 * @brief Remove the last element of the array.
 * @param  array: The array to remove from.
 * @param  poppedElement: Pointer to retreiving data, the get a copy of the
 * element that was removed. This can be left to NULL if no returned value is
 * requested.
 */
// void ArrayPopBack(Array* array, void* poppedElement)
//{
//     LogAssert(array != NULL);
//     LogAssert(array->length > 0);
//
//     void* lastElement = ArrayGet(array, array->length - 1);
//
//     if (poppedElement != NULL)
//     {
//         memcpy(poppedElement, lastElement, array->elementSize);
//     }
//
//     memset(lastElement, 0, array->elementSize);
//
//     array->length--;
// }
//
///**
// * @brief Retrieve an element at a specific index.
// * @param array The array to retrieve the element from.
// * @param index The index at which to find the element
// * @return void* A pointer to the requested element.
// */
// void* ArrayGet(const Array* array, const uint64_t index)
//{
//    LogAssert(array != NULL);
//    LogAssert(index < array->length);
//
//    return (void*)array->elements + ((index)*array->elementSize);
//}
//
///**
// * @brief Resize the array. If the new size is smaller than the previous size,
// * the excess elements will be discarded. If the new size is bigger than the
// * previous size, old pointers to elements might become corrupt.
// * @param array The array to resize.
// * @param newCapacity The new number of elements the array can occupy before
// * having to allocate more memory.
// */
// void ArrayResize(Array* array, const uint newCapacity)
//{
//    LogAssert(array != NULL);
//
//    if (newCapacity == array->capacity) return;
//
//    array->elements = realloc(array->elements, array->elementSize *
//    (size_t)newCapacity); array->capacity = newCapacity; array->length =
//    fmin(array->length, newCapacity);
//}
//
///**
// * @brief Fill the whole array with values until its capacity is reached. This
// * will override values already present in the array.
// * @param array The array to fill.
// * @param value The value to fill the array with.
// */
// void ArrayFill(Array* array, void* value)
//{
//    LogAssert(array != NULL);
//    LogAssert(value != NULL);
//
//    for (int i = 0; i < array->capacity; ++i)
//    {
//        memcpy(array->elements + (i * array->elementSize), value,
//        array->elementSize);
//    }
//
//    array->length = array->capacity;
//}
//
///**
// * @brief Remove all elements from the array. Doesn't change the array's
// * capacity.
// * @param array The array to be cleared.
// */
// void ArrayClear(Array* array)
//{
//    LogAssert(array != NULL);
//
//    memset(array->elements, 0, array->elementSize * array->capacity);
//    array->length = 0;
//}
//
// bool ArrayContains(Array* array, void* element)
//{
//    for (int i = 0; i < ArrayNum(array); ++i)
//    {
//        if (memcmp(ArrayGet(array, i), element, array->elementSize) == 0)
//        {
//            return true;
//        }
//    }
//
//    return false;
//}
//
///**
// * @brief Get the number of elements present in the array.
// * @param array The array to get the number of elements from.
// * @return uint64_t The number of elements in the array.
// */
// uint64_t ArrayNum(const Array* array)
//{
//    LogAssert(array != NULL);
//    return array->length;
//}
//
///**
// * @brief Get the number of elements the array can store, before having to
// * increase in size.
// * @param array The array to get the capacity from.
// * @return uint64_t The capacity of the array.
// */
// uint64_t ArrayCapacity(const Array* array)
//{
//    LogAssert(array != NULL);
//    return array->capacity;
//}
