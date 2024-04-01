#ifndef ECS_I
#define ECS_I

#include "Containers/Dictionary.h"
#include "Scene.h"
#include "System.h"
#include "Entity.h"

#include <stdint.h>

static const uint8_t MAX_COMPONENT_TYPES = 64;
static const uint8_t MAX_SYSTEM_TYPES = 64;

ComponentInstanceID nextComponentID;

struct ECS ecs;

typedef struct ECS
{
    Array systems;
    Array Scenes;
    Array ComponentTypeIDs;
}ECS;

//export
ECS* ECSNew();
void ECSFree(ECS* ecs);

// void ECSAddEntity(Entity* e);

ComponentTypeID ECSRegisterComponent(ECS* ecs, char* componentName, size_t componentNameSize, size_t componentSize);
ComponentInstanceID ECSAddComponent(ECS* ecs, ComponentTypeID componentTypeID, void* component, Entity entity, Scene* scene);
ComponentTypeID ECSGetComponentTypeID(ECS* ecs, char* componentName);

void ECSRegisterSystem(ECS* ecs, System* system);
// void ECSAddSystem(char* systemName, uint64_t entityId); // SHOULD GO AWAY

Entity ECSAddEntity(ECS* ecs, Scene* sceneToAddEntityTo);

void ECSUpdate(ECS* ecs, Scene* scene);

//no export
void ECSInit(ECS* ecs);
void ECSDeinit(ECS* ecs);

#endif