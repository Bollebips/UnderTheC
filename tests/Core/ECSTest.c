#include "Core/ECS.h"
#include "Core/System.h"
#include "Core/Component.h"

#include "Utils/Hash.h"

typedef struct TestComponent
{
    Component component;
    int testInt;
    float testFloat;
    bool testBool;

}TestComponent;

ComponentTypeID testComponentTypeID;

void UpdateTestSystem(int numComponents, void* componentData[])
{
    TestComponent* testComponent = (TestComponent*) componentData;
    TEST_CHECK(testComponent->testInt == 1234);
}

void TestECS()
{
    ECS* ecs = ECSNew();

    Scene* newScene = SceneNew();
    SceneAddEntity(newScene);

    //TODO: This should be done automatically when creating a new scene. The scene creation will probably have to be done through the ECS system for this to work.
    ArrayAdd(&(ecs->Scenes), newScene);


    testComponentTypeID = ECSRegisterComponent(ecs, "TestComponent", 13, sizeof(TestComponent));

    Entity newEntity = ECSAddEntity(ecs, newScene);

    TestComponent newTestComponent;
    newTestComponent.testInt = 1234;
    newTestComponent.testFloat = 3.14f;
    newTestComponent.testBool = true;
    ECSAddComponent(ecs, testComponentTypeID, &newTestComponent, newEntity, newScene);

    ComponentTypeID componentsToUpdate[3] = { 0, 1, 2 };
    System* testSystem = SystemNew("testSystem", 10, componentsToUpdate, 3, 69, &UpdateTestSystem);

    ECSRegisterSystem(ecs, testSystem);

    TEST_CHECK(ecs->systems.num == 1);

    // System* retrievedSystem = DictionaryGet(&(ecs->systems), &(testSystem->id));
    // TEST_CHECK(retrievedSystem != NULL);
    // TEST_CHECK(*(uint64_t*) ArrayGet(&(retrievedSystem->componentsToUpdate), 2) == 2);

    ECSUpdate(ecs, newScene);

    ECSFree(ecs);
}