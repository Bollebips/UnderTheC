#include "Component.h"

uint64_t ComponentGetID(const void* component)
{
    // return *(ComponentID*) componentID;
    Component* c = (Component*) component;
    return c->entity;
}