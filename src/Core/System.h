#ifndef SYSTEM_I
#define SYSTEM_I

#include "../include/core/System.h"

#include "Containers/Array.h"
#include "Containers/SparseSet.h"

#include <stdint.h>

typedef uint64_t SystemTypeID;

typedef struct System
{
    SystemTypeID id;
    Array componentsToUpdate;
    uint64_t updateOrder;
    void (*updateFunction)(int, void* []);
    SparseSet compatibleEntities;
} System;

void SystemInit(System* system, const char* systemName, const size_t systemNameLength, const ComponentTypeID componentsToUpdate[], const uint8_t numComponentsToUpdate, uint64_t updateOrder, void (*updateFunction)(int, void* []));
void SystemDeinit(System* system);

#endif