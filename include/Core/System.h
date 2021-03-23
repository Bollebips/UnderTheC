#ifndef SYSTEM_H
#define SYSTEM_H

#include "Component.h"

#include <stddef.h>

typedef struct System System;

System* SystemNew(const char* systemName, const size_t systemNameLength, const ComponentTypeID componentsToUpdate[], const uint8_t numComponentsToUpdate, uint64_t updateOrder, void (*updateFunction)(int, void* []));
void SystemFree(System* system);

#endif