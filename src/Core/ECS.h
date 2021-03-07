#ifndef ECS_I
#define ECS_I

#include "../include/Core/ECS.h"

#include "Containers/Dictionary.h"
#include "Scene.h"

static const uint8_t MAX_COMPONENT_TYPES = 64;
static const uint8_t MAX_SYSTEM_TYPES = 64;

ComponentID nextComponentID;

struct ECS ecs;

struct ECS
{
    Array systems;
    Array Scenes;
    Array ComponentTypeIDs;
};

void ECSInit(ECS* ecs);
void ECSDeinit(ECS* ecs);

#endif