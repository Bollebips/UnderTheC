#ifndef COMPONENT_I
#define COMPONENT_I

#include "../include/Core/Component.h"

#include "Entity.h"

#include <stdint.h>

struct Component
{
    ComponentTypeID componentTypeID;
    ComponentID componentID;
    Entity entity;
};

uint64_t ComponentGetID(const void* componentID);

#endif