#include "ECS.h"

#include "Logger.h"
#include "Utils/Hash.h"

ECS* ECSNew()
{
    ECS* newECS = malloc(sizeof(ECS));
    ECSInit(newECS);

    return newECS;
}

void ECSFree(ECS* ecs)
{
    LogAssert(ecs);

    free(ecs);
}

ComponentTypeID ECSRegisterComponent(ECS* ecs, char* componentName, size_t componentNameSize, size_t componentSize)
{
    LogAssert(ecs);
    LogAssert(componentNameSize > 0);

    ComponentTypeID componentTypeID = HashFNV1a64(componentName, componentNameSize);
    ArrayAdd(&(ecs->ComponentTypeIDs), &componentTypeID);

    for(int i = 0; i < ArrayNum(&(ecs->Scenes)); ++i)
    {
        Scene* scene = ArrayGet(&(ecs->Scenes), i);

        // Array* componentArray = ArrayNew(componentSize);
        SparseSet* componentSparseSet = SparseSetNew(componentSize, ComponentGetID, 16); //TODO: hardcoded bucketsize 16
        DictionaryAdd(&(scene->components), &componentTypeID, componentSparseSet);
    }

    return componentTypeID;
}

ComponentID ECSAddComponent(ECS* ecs, ComponentTypeID componentTypeID, void* component, Entity entity, Scene* scene) //TODO: remove scene argument
{
    LogAssert(ecs);
    LogAssert(component);
    LogAssert(componentTypeID);
    LogAssert(entity);

    ++nextComponentID;

    Component* c = component;
    c->componentID = nextComponentID;
    c->entity = entity;

    SparseSet* componentSparseSet = DictionaryGet(&(scene->components), &componentTypeID);
    SparseSetAdd(componentSparseSet, component);

    BucketArray* entities = SparseSetGetDenseData(&(scene->entities));

    for(int i = 0; i < ArrayNum(&(ecs->systems)); ++i) // TODO: this should not just add all entities to all systems. It should filter based on the components needed by the specific systems.
    {
        for(int e = 0; e < BucketArrayNum(entities); ++e)
        {
            System* system = ArrayGet(&(ecs->systems), i);

            SparseSetAdd(&(system->compatibleEntities), BucketArrayGet(entities, e));
        }
    }

    return nextComponentID; // TODO: return the specific component's ID.
}

void ECSRegisterSystem(ECS* ecs, System* system)
{
    LogAssert(ecs);
    LogAssert(system);

    // DictionaryAdd(&(ecs->systems), &(system->id), system);
    ArrayAdd(&(ecs->systems), system);
}

Entity ECSAddEntity(ECS* ecs, Scene* sceneToAddEntityTo)
{
    LogAssert(ecs);
    LogAssert(sceneToAddEntityTo);

    ++nextEntityID;

    Entity newEntity = nextEntityID;
    SparseSetAdd(&(sceneToAddEntityTo->entities), &newEntity);

    return newEntity;
}

void ECSUpdate(ECS* ecs, Scene* scene)//TODO: remove scene argument
{
    // for(int i = 0; i < ArrayNum(&(ecs->systems)); ++i)
    // {
    //     System* system = ArrayGet(&(ecs->systems), i);

    //     for(int sc = 0; sc < ArrayNum(&(ecs->ComponentTypeIDs)); ++sc)
    //     {
    //         ComponentTypeID* componentTypeID = ArrayGet(&(ecs->ComponentTypeIDs), sc);
    //         Array* components = DictionaryGet(&(scene->components), componentTypeID);
    //         // BucketArray* denseComponents = SparseSetGetDenseData(sparseComponents);

    //         for(int c = 0; c < ArrayNum(components); ++c)
    //         {
    //             int numComponents = ArrayNum(&(system->componentsToUpdate));
    //             void* component = ArrayGet(components, c);
    //             system->updateFunction(numComponents, component);
    //         }
    //     }
    // }

    for(int i = 0; i < ArrayNum(&(ecs->systems)); ++i)
    {
        System* system = ArrayGet(&(ecs->systems), i);

        for(int sc = 0; sc < ArrayNum(&(ecs->ComponentTypeIDs)); ++sc)
        {
            ComponentTypeID* componentTypeID = ArrayGet(&(ecs->ComponentTypeIDs), sc);
            SparseSet* sparseComponents = DictionaryGet(&(scene->components), componentTypeID);
            BucketArray* denseComponents = SparseSetGetDenseData(sparseComponents);

            for(int c = 0; c < BucketArrayNum(denseComponents); ++c)
            {
                int numComponents = ArrayNum(&(system->componentsToUpdate));
                void* component = BucketArrayGet(denseComponents, c);
                system->updateFunction(numComponents, component);
            }
        }
    }
}

/* ---------------------------------------------------- INTERNAL ---------------------------------------------------- */

void ECSInit(ECS* ecs)
{
    LogAssert(ecs);

    // DictionaryInit(&(ecs->systems), sizeof(char*), sizeof(System));
    ArrayInit(&(ecs->systems), sizeof(System), 1);
    ArrayInit(&(ecs->Scenes), sizeof(Scene), 1);
    ArrayInit(&(ecs->ComponentTypeIDs), sizeof(ComponentTypeID), 1);
}

/* void ECSAddEntity(Entity* e)
{
    LogAssert(e != NULL);
    ArrayAdd(&(ecs.entities), e);
}

void ECSRegisterComponent(char* componentName, size_t componentSize)
{
    LogAssert(componentName != NULL);
    LogAssert(componentSize > 0);

    DictionaryAdd(&(ecs.components), &componentName, ArrayNew(componentSize));
}

void ECSAddComponent(char* componentName, void* component, uint64_t entityId)
{
    // Entity* e = (Entity*) ArrayGet(&(ecs.entities), entityId);

    Array* components = DictionaryGet(&(ecs.components), &componentName);

    ArrayAdd(components, component);
}

void ECSSystemInitialize(System* system)
{
    ArrayInit(&system->componentsToUpdate, sizeof(char*), 1);
}

void ECSRegisterSystem(char* systemName, System* system)
{
    ArrayAdd(&(ecs.systems), system);
}

void ECSAddSystem(char* systemName, uint64_t entityId)
{

}

void ECSUpdate()
{
    for(int i = 0; i < ArrayNum(&(ecs.systems)); ++i)
    {
        System* systemToUpdate = (System*) ArrayGet(&(ecs.systems), i);

        int numComponentsToUpdate = ArrayNum(&(system->componentsToUpdate));
        Array componentArraysToUpdate[numComponentsToUpdate];

        for

            OUR_GUID_ENTRY("evnaerov", )

            int numUpdates =

            for(int j = 0; j < numComponents; ++j)
            {
                for(int k = 0; k < numComponents)
                {
                    char* componentName = *(char**) ArrayGet(&(system->componentsToUpdate), k);
                    Array* components = (Array*) DictionaryGet(&(ecs.components), componentName);
                }
            }

        system->updateFunction(numComponents, components);
    }
} */