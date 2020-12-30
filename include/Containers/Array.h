#ifndef ARRAY_H
#define ARRAY_H

#include <stdint.h>
#include <stdlib.h>

/**
 * @brief A dynamic array, that dynamically expands its memory footprint when necessary.
*/
typedef struct Array Array;

Array* ArrayNew(size_t elementSize);
void* ArrayAdd(Array* array, const void* newElement);
void ArrayPopBack(Array* array, void* poppedElement);
void* ArrayGet(const Array* array, const uint64_t index);
void ArrayResize(Array* array, const uint64_t newCapacity);
void ArrayFill(Array* array, void* value);
void ArrayClear(Array* array);
void ArrayFree(Array* array);

uint64_t ArrayNum(const Array* array);
uint64_t ArrayCapacity(const Array* array);

#endif