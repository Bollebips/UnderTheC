#ifndef ARRAY_I
#define ARRAY_I

#include "../../include/Containers/Array.h"

/**
 * @brief A dynamic array, that dynamically expands its memory footprint when necessary.
 */
struct Array
{
    uint64_t num;           // The number of occupied elements in the array.
    uint64_t capacity;      // The maximum number of occupied elements before the array has to allocate more memory.
    size_t elementSize;     // The memory footprint of 1 element.
    void* elements;         // A pointer to the elements allocated in memory.
};

void ArrayInit(Array* array, size_t elementSize, const uint64_t initialCapacity);
void ArrayDeinit(Array* array);

#endif