#include "Containers/Dictionary.h"
#include "Math/Math.h"
#include <inttypes.h>

void TestDictionaryAddGet()
{
    Dictionary* dict = DictionaryNew(sizeof(char*), sizeof(int));
    TEST_CHECK(dict != NULL);

    TEST_CHECK(dict->keySize == sizeof(char*));
    TEST_CHECK(dict->valueSize == sizeof(int));
    TEST_CHECK(dict->num == 0);
    TEST_CHECK(dict->collisionElements.num == 0);

    int value = 5;
    char* key = "hello";
    DictionaryAdd(dict, key, &value);
    TEST_CHECK(dict->num == 1);
    TEST_CHECK(dict->collisionElements.num == 0);

    int* returnValue = (int*) DictionaryGet(dict, key);
    TEST_CHECK(*returnValue == value);

    void* returnPtr = DictionaryGet(dict, "randomKey");
    TEST_CHECK(returnPtr == NULL);

    int value2 = 123;
    char* key2 = "world";
    DictionaryAdd(dict, key2, &value2);
    TEST_CHECK(dict->num == 2);

    returnValue = (int*) DictionaryGet(dict, key2);
    TEST_CHECK(*returnValue == value2);
}

void TestDictionaryResize()
{
    Dictionary* dict = DictionaryNew(sizeof(int), sizeof(int));

    int dictionaryInitialCapacity = 22;

    int value;
    for(int i = 0; i < dictionaryInitialCapacity; ++i)
    {
        value = rand();
        DictionaryAdd(dict, &i, &value);
    }

    TEST_CHECK(dict->num == dictionaryInitialCapacity);
    TEST_CHECK_(dict->elements.num == dictionaryInitialCapacity, "%"PRIu64" > dictionaryInitialCapacity", dict->elements.num);

    value = rand();
    DictionaryAdd(dict, &dictionaryInitialCapacity, &value);

    TEST_CHECK(dict->num == dictionaryInitialCapacity + 1);
    int newCapacity = dictionaryInitialCapacity * GOLDEN_RATIO;
    TEST_CHECK(dict->elements.num == newCapacity);
}

void TestDictionary()
{
    TestDictionaryAddGet();
    TestDictionaryResize();

}