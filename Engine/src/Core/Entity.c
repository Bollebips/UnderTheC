#include "Entity.h"

uint64_t EntityGetID(const void* entity)
{
    return *(Entity*) entity;
}