#include "Core/ECS.h"
#include "Core/System.h"
#include "Core/Component.h"

#include "Utils/Hash.h"
#include "Logger.h"

typedef struct TestComponent1
{
    Component component;
    int testInt;
    float testFloat;
    bool testBool;

}TestComponent1;

typedef struct TestComponent2
{
    Component component;
    bool testBool2;
    int testInt2;
    float testFloat2;

}TestComponent2;

ComponentTypeID testComponent1TypeID;
ComponentTypeID testComponent2TypeID;

void UpdateTestSystem1(int numComponents, void* componentData[])
{
    TestComponent1* testComponent1 = (TestComponent1*) componentData;

    TEST_CHECK(testComponent1->testInt == 1234);
    TEST_CHECK(testComponent1->testFloat == 3.14f);
    TEST_CHECK(testComponent1->testBool == true);
}

void UpdateTestSystem1And2(int numComponents, void* componentData[])
{
    TestComponent1* testComponent1 = (TestComponent1*) componentData;
    TestComponent2* testComponent2 = (TestComponent2*) (componentData + sizeof(TestComponent1));

    TEST_CHECK(testComponent1->testInt == 1234);
    TEST_CHECK(testComponent1->testFloat == 3.14f);
    TEST_CHECK(testComponent1->testBool == true);
    TEST_CHECK(testComponent2->testInt2 == -4321);
    TEST_CHECK(testComponent2->testFloat2 == 69.69f);
    TEST_CHECK(testComponent2->testBool2 == false);

}

void TestECS()
{
    ECS* ecs = ECSNew();

    Scene* newScene = SceneNew();
    // SceneAddEntity(newScene); //TODO: remove

    //TODO: This should be done automatically when creating a new scene. The scene creation will probably have to be done through the ECS system for this to work.
    newScene = ArrayAdd(&(ecs->Scenes), newScene);

    testComponent1TypeID = ECSRegisterComponent(ecs, "TestComponent1", 14, sizeof(TestComponent1));
    testComponent2TypeID = ECSRegisterComponent(ecs, "TestComponent2", 14, sizeof(TestComponent2));

    Entity newEntity = ECSAddEntity(ecs, newScene);

    TestComponent1 newTestComponent1;
    newTestComponent1.testInt = 1234;
    newTestComponent1.testFloat = 3.14f;
    newTestComponent1.testBool = true;
    ECSAddComponent(ecs, testComponent1TypeID, &newTestComponent1, newEntity, newScene);

    TestComponent2 newTestComponent2;
    newTestComponent2.testInt2 = -4321;
    newTestComponent2.testFloat2 = 69.69f;
    newTestComponent2.testBool2 = true;
    ECSAddComponent(ecs, testComponent2TypeID, &newTestComponent2, newEntity, newScene);

    ComponentTypeID componentsToUpdate1[1] = { testComponent1TypeID };
    System* testSystem1 = SystemNew("testSystem1", 11, componentsToUpdate1, 1, 69, &UpdateTestSystem1);

    ComponentTypeID componentsToUpdate2[2] = { testComponent1TypeID, testComponent2TypeID };
    System* testSystem2 = SystemNew("testSystem2", 11, componentsToUpdate2, 2, 69, &UpdateTestSystem1And2);

    ECSRegisterSystem(ecs, testSystem1);
    ECSRegisterSystem(ecs, testSystem2);

    TEST_CHECK(ecs->systems.num == 2);

    ECSUpdate(ecs, newScene);

    ECSFree(ecs);
}