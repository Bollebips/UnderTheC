#ifndef ARRAY_H
#define ARRAY_H

#include "Defines.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// typedef struct Array
//{
//     size_t length;      // The number of occupied elements in the array.
//     size_t capacity;    // The maximum number of occupied elements before the
//     array
//                         // has to allocate more memory.
//     size_t elementSize; // The memory footprint of 1 element.
//     void* elements;     // A pointer to the elements allocated in memory.
// } Array;

// VoxC_API Array ArrayNew(size_t elementSize);
// VoxC_API void ArrayFree(Array* array);

#define INITIAL_CAPACITY 16
#define CAPACITY_MULTIPLIER 2

#define Array(T)                                                                                                                                               \
    struct                                                                                                                                                     \
    {                                                                                                                                                          \
        T* Items;                                                                                                                                              \
        size_t Length;                                                                                                                                         \
        size_t Capacity;                                                                                                                                       \
    }

#define ArrayAdd(_array, _element)                                                                                                                             \
    do                                                                                                                                                         \
    {                                                                                                                                                          \
        if (_array->Length >= _array->Capacity)                                                                                                                \
        {                                                                                                                                                      \
            if (_array->Capacity == 0) _array->Capacity = INITIAL_CAPACITY;                                                                                    \
            else _array->Capacity *= CAPACITY_MULTIPLIER;                                                                                                      \
            _array->Items = realloc(_array->Items, _array->Capacity * sizeof(*_array->Items));                                                             \
        }                                                                                                                                                      \
        _array->Items[_array->Length++] = _element;                                                                                                            \
    } while (0)

// VoxC_API void* ArrayAdd(Array* array, const void* newElement);
// VoxC_API void ArrayPopBack(Array* array, void* poppedElement);
// VoxC_API void* ArrayGet(const Array* array, const uint64_t index);
// VoxC_API void ArrayResize(Array* array, const uint newCapacity);
// VoxC_API void ArrayFill(Array* array, void* value);
// VoxC_API void ArrayClear(Array* array);
// VoxC_API bool ArrayContains(Array* array, void* element);
//
// VoxC_API size_t ArrayNum(const Array* array);
// VoxC_API size_t ArrayCapacity(const Array* array);

#endif // ARRAY_H
