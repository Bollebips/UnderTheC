#include "Scene.h"

#include "Logger.h"
#include "Utils/Hash.h"
#include "Component.h"

Scene* SceneNew()
{
    Scene* newScene = (Scene*) malloc(sizeof(Scene));
    SceneInit(newScene);
    return newScene;
}

void SceneFree(Scene* scene)
{
    LogAssert(scene != NULL);

    SceneDeinit(scene);
    free(scene);
}

Entity SceneAddEntity(Scene* scene)
{
    SparseSetAdd(&(scene->entities), &nextEntityID);
    nextEntityID++;
    return nextEntityID;
}

void SceneRegisterComponent(Scene* scene, char* componentName, size_t componentNameSize, size_t componentSize)
{
    ComponentID componentTypeID = (ComponentID) HashFNV1a64(componentName, componentNameSize);

    if(DictionaryGet(&(scene->components), &componentTypeID) != NULL)
    {
        return;
    }

    SparseSet* newSparseSet = SparseSetNew(componentSize, &ComponentGetID, 16);
    DictionaryAdd(&(scene->components), &componentTypeID, newSparseSet);
}

// ComponentID SceneAddComponent(Scene* scene, ComponentTypeID componentTypeID, void* component, Entity entity)
// {
//     SparseSet* componentsSet = DictionaryGet(&(scene->components), &componentTypeID);

//     SparseSetAdd(componentsSet, component);
// }

/* ---------------------------------------------------- INTERNAL ---------------------------------------------------- */

void SceneInit(Scene* scene)
{
    LogAssert(scene != NULL);

    nextEntityID = 0;

    // DictionaryInit(&(scene->components), sizeof(ComponentTypeID), sizeof(SparseSet));
    DictionaryInit(&(scene->components), sizeof(ComponentTypeID), sizeof(Array));
    SparseSetInit(&(scene->entities), sizeof(Entity), EntityGetID, 16);
}

void SceneDeinit(Scene* scene)
{
    LogAssert(scene != NULL);

    DictionaryDeinit(&(scene->components));
    SparseSetDeinit(&(scene->entities));
}