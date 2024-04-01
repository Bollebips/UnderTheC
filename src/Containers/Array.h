#pragma once

#ifndef ARRAY_I
#define ARRAY_I

#include "Math/Math.h"
#include <math.h>
#include "Logger.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * @brief A dynamic array, that dynamically expands its memory footprint when necessary.
 */
typedef struct Array
{
    uint64_t num;           // The number of occupied elements in the array.
    uint64_t capacity;      // The maximum number of occupied elements before the array has to allocate more memory.
    size_t elementSize;     // The memory footprint of 1 element.
    void* elements;         // A pointer to the elements allocated in memory.
}Array;

void ArrayInit(Array* array, size_t elementSize, const uint64_t initialCapacity);
void ArrayDeinit(Array* array);

Array* ArrayNew(size_t elementSize);
void* ArrayAdd(Array* array, const void* newElement);
void ArrayPopBack(Array* array, void* poppedElement);
void* ArrayGet(const Array* array, const uint64_t index);
void ArrayResize(Array* array, const uint64_t newCapacity);
void ArrayFill(Array* array, void* value);
void ArrayClear(Array* array);
bool ArrayContains(Array* array, void* element);
void ArrayFree(Array* array);

uint64_t ArrayNum(const Array* array);
uint64_t ArrayCapacity(const Array* array);

#endif