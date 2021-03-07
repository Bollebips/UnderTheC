#include "System.h"

#include "Logger.h"
#include "Utils/Hash.h"

System* SystemNew(const char* systemName, const size_t systemNameLength, const ComponentTypeID componentsToUpdate[], const uint8_t numComponentsToUpdate, uint64_t updateOrder, void (*updateFunction)(int, void* []))
{
    LogAssert(systemName);
    LogAssert(systemNameLength > 0);
    LogAssert(numComponentsToUpdate > 0);
    LogAssert(updateFunction);

    System* newSystem = malloc(sizeof(System));
    SystemInit(newSystem, systemName, systemNameLength, componentsToUpdate, numComponentsToUpdate, updateOrder, updateFunction);

    return newSystem;
}

void SystemFree(System* system)
{
    LogAssert(system);

    SystemDeinit(system);
    free(system);
}

/* ---------------------------------------------------- INTERNAL ---------------------------------------------------- */

void SystemInit(System* system, const char* systemName, const size_t systemNameLength, const ComponentTypeID componentsToUpdate[], const uint8_t numComponentsToUpdate, uint64_t updateOrder, void (*updateFunction)(int, void* []))
{
    LogAssert(system);
    LogAssert(numComponentsToUpdate > 0);

    system->id = HashFNV1a64(systemName, systemNameLength);
    system->updateOrder = updateOrder;
    system->updateFunction = updateFunction;

    ArrayInit(&(system->componentsToUpdate), sizeof(ComponentTypeID), numComponentsToUpdate);

    for(int i = 0; i < numComponentsToUpdate; ++i)
    {
        ArrayAdd(&(system->componentsToUpdate), &componentsToUpdate[i]);
    }
}

void SystemDeinit(System* system)
{
    LogAssert(system);

    ArrayDeinit(&(system->componentsToUpdate));
    SparseSetDeinit(&(system->compatibleEntities));
}