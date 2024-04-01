#ifndef SCENE_I
#define SCENE_I

#include "Containers/Dictionary.h"
#include "Containers/SparseSet.h"
#include "Entity.h"
#include "Component.h"
#include "System.h"

static Entity nextEntityID;

typedef struct Scene
{
    Dictionary components;  // Dictionary<ComponentTypeID, SparseSet<ComponentID>>
    SparseSet entities;
} Scene;

Entity SceneAddEntity(Scene* scene);
void SceneRegisterComponent(Scene* scene, char* componentName, size_t componentNameSize, size_t componentSize);
ComponentInstanceID SceneAddComponent(Scene* scene, ComponentTypeID componentTypeID, void* component, Entity entity);

//export
Scene* SceneNew();
void SceneFree(Scene* scene);

//no export
void SceneInit(Scene* scene);
void SceneDeinit(Scene* scene);


#endif