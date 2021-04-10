#ifndef COMPONENT_I
#define COMPONENT_I

#include "../include/Core/Component.h"

#include "Entity.h"
#include "System.h"
#include "Containers/GroupedSparseSet.h"

#include <stdint.h>

struct Component
{
    ComponentTypeID componentTypeID;
    ComponentInstanceID componentInstanceID;
    Entity entity;
};

uint64_t ComponentGetID(const void* componentID);

typedef struct ComponentContainer
{
    GroupedSparseSet groupedSparseSet;
    Array groupingSystemIDs;
}ComponentContainer;

#endif