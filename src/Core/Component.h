#ifndef COMPONENT_I
#define COMPONENT_I

#include "Entity.h"

#include <stdint.h>

typedef uint64_t ComponentTypeID;
typedef uint64_t ComponentInstanceID;

typedef struct Component
{
    ComponentTypeID componentTypeID;
    ComponentInstanceID componentInstanceID;
    Entity entity;
}Component;

uint64_t ComponentGetID(const void* componentID);

#endif