#pragma once
#include <stdint.h>
#include <stdlib.h>

/**
 * @brief A dynamic array, that dynamically expands its memory footprint when necessary.
*/
typedef struct Array Array;

Array* ArrayNew(size_t elementSize, const uint64_t initialCapacity);
void* ArrayAdd(Array* array, const void* newElement);
void ArrayPopBack(Array* array, void* poppedElement);
void* ArrayGet(const Array* array, const uint64_t index);
void ArrayResize(Array* array, const uint64_t numElements);
void ArrayFree(Array* array);

uint64_t ArrayGetNum(const Array* array);
uint64_t ArrayGetCapacity(const Array* array);