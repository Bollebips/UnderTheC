#ifndef ECS_H
#define ECS_H

#include "System.h"
#include "Scene.h"
#include "Entity.h"

#include <stdint.h>

typedef struct ECS ECS;

ECS* ECSNew();
void ECSFree(ECS* ecs);

//-------------------------------------------

// void ECSAddEntity(Entity* e);

ComponentTypeID ECSRegisterComponent(ECS* ecs, char* componentName, size_t componentNameSize, size_t componentSize);
ComponentInstanceID ECSAddComponent(ECS* ecs, ComponentTypeID componentTypeID, void* component, Entity entity, Scene* scene);
ComponentTypeID ECSGetComponentTypeID(ECS* ecs, char* componentName);

void ECSRegisterSystem(ECS* ecs, System* system);
// void ECSAddSystem(char* systemName, uint64_t entityId); // SHOULD GO AWAY

Entity ECSAddEntity(ECS* ecs, Scene* sceneToAddEntityTo);

void ECSUpdate();

#endif