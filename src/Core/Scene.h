#ifndef SCENE_I
#define SCENE_I

#include "../../include/core/Scene.h"

#include "Containers/Dictionary.h"
#include "Containers/SparseSet.h"
#include "Entity.h"
#include "Component.h"
#include "System.h"

Entity nextEntityID;

typedef struct Scene
{
    Dictionary components;  // Dictionary<ComponentTypeID, SparseSet<ComponentID>>
    SparseSet entities;
} Scene;

Entity SceneAddEntity(Scene* scene);
void SceneRegisterComponent(Scene* scene, char* componentName, size_t componentNameSize, size_t componentSize);
ComponentInstanceID SceneAddComponent(Scene* scene, ComponentTypeID componentTypeID, void* component, Entity entity);

void SceneInit(Scene* scene);
void SceneDeinit(Scene* scene);


#endif