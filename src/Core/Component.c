#include "Component.h"

uint64_t ComponentGetID(const void* componentID)
{
    return *(ComponentID*) componentID;
}