#include "Array.h"
#include "BucketArray.h"
#include "Dictionary.h"
#include "Windows.h"
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>
#include <stdio.h>

typedef struct test
{
    bool b1;
    int64_t i1;
    float f1;
    int64_t i2;
} test;

void print_array(Array* array)
{
    for(int i = 0; i < ArrayGetNum(array); ++i)
    {
        test result = *(test*) ArrayGet(array, i);
        printf("%" PRId64 ", %" PRId64 ", %f, %d\n", result.i1, result.i2, result.f1, result.b1);
    }
}

void PrintBucketArray(BucketArray* bucketArray)
{
    printf("Number of buckets = %d\n", (int) BucketArrayGetNumBuckets(bucketArray));
    for(int i = 0; i < BucketArrayGetNum(bucketArray); ++i)
    {
        test result = *(test*) BucketArrayGet(bucketArray, i);
        printf("%" PRId64 ", %" PRId64 ", %f, %d\n", result.i1, result.i2, result.f1, result.b1);
    }
}

void PrintTest(test* test)
{
    printf("%" PRId64 ", %" PRId64 ", %f, %d\n", test->i1, test->i2, test->f1, test->b1);
}

    // void print_dict(dictionary* array)
    // {
    //     for (int i = 0; i < array->num; ++i)
    //     {
    //         test result = *(test*)array_get(array, i);
    //         printf("%d, %d, %f, %d\n", result.i1, result.i2, result.f1, result.b1);
    //     }
    // }

void* testfunc(size_t size)
{
    return malloc(size);
}

typedef struct test_key
{
    uint8_t u8;
    uint8_t u8Other;
} test_key;

void array_test()
{
    test testers[4];
    testers[0].i1 = 9;
    testers[0].i2 = 1;
    testers[0].f1 = 9.6549f;
    testers[0].b1 = false;

    testers[1].i1 = 13;
    testers[1].i2 = 642355325;
    testers[1].f1 = 0;
    testers[1].b1 = false;

    testers[2].i1 = 0;
    testers[2].i2 = -13461143;
    testers[2].f1 = 3.1415f;
    testers[2].b1 = true;

    testers[3].i1 = 987;
    testers[3].i2 = 20202;
    testers[3].f1 = 0.000001f;
    testers[3].b1 = true;

    Array* testArray = ArrayNew(sizeof(test), 7);

    ArrayAdd(testArray, &testers[0]);
    print_array(testArray);

    ArrayAdd(testArray, &testers[1]);
    print_array(testArray);

    ArrayAdd(testArray, &testers[2]);
    print_array(testArray);

    ArrayAdd(testArray, &testers[3]);
    print_array(testArray);

    ArrayAdd(testArray, &testers[3]);
    print_array(testArray);

    ArrayAdd(testArray, &testers[3]);
    print_array(testArray);

    ArrayAdd(testArray, &testers[3]);
    print_array(testArray);

    ArrayAdd(testArray, &testers[3]);
    print_array(testArray);

    ArrayAdd(testArray, &testers[3]);
    print_array(testArray);

    ArrayAdd(testArray, &testers[3]);
    print_array(testArray);

    ArrayAdd(testArray, &testers[3]);
    print_array(testArray);

    ArrayAdd(testArray, &testers[3]);
    print_array(testArray);

    ArrayAdd(testArray, &testers[3]);
    print_array(testArray);

    ArrayAdd(testArray, &testers[3]);
    print_array(testArray);

    test new_tester;
    new_tester.i1 = 123;
    new_tester.i2 = 123;
    new_tester.f1 = 123;
    new_tester.b1 = true;

    test* element13 = ArrayGet(testArray, 13);
    *element13 = new_tester;
    print_array(testArray);

    ArrayGet(testArray, 13);

    // testArray = NULL;

    ArrayGet(testArray, 13);

    // printf("%p\n", testArray.elements);
    ArrayResize(testArray, 5);
    printf("\n\n");
    printf("%"PRIu64"\n", ArrayGetNum(testArray));
    print_array(testArray);
    test* lastElement = (test*) ArrayGet(testArray, ArrayGetNum(testArray) - 1);

    lastElement->i1 = 1;
    lastElement->i2 = 2;
    lastElement->f1 = 707.705f;
    lastElement->b1 = false;

    printf("\n\n");
    print_array(testArray);

    test newElement;
    ArrayPopBack(testArray, &newElement);

    printf("\n\n");
    printf("%" PRId64 ", %" PRId64 ", %f, %d\n", newElement.i1, newElement.i2, newElement.f1, newElement.b1);

    printf("\n\n");
    printf("%"PRIu64"\n", ArrayGetNum(testArray));
    print_array(testArray);

    ArrayPopBack(testArray, &newElement);

    printf("\n\n");
    printf("%" PRId64 ", %" PRId64 ", %f, %d\n", newElement.i1, newElement.i2, newElement.f1, newElement.b1);
    printf("%"PRIu64"\n", ArrayGetNum(testArray));

    ArrayFree(testArray);
    testArray = NULL;
}

void dict_test()
{
    Dictionary* newDict = DictionaryNew(sizeof(test_key), sizeof(test), 5);

    test tester;
    tester.i1 = 9;
    tester.i2 = 1;
    tester.f1 = 9.6549f;
    tester.b1 = true;

    test_key testKey;
    testKey.u8 = 5;
    testKey.u8Other = 5;

    DictionaryAdd(newDict, &testKey, &tester);

    test* newTest;
    newTest = (test*) DictionaryGet(newDict, &testKey);

    PrintTest(newTest);

    newTest->i1 = 123456;
    newTest->f1 = 0.0001f;
    newTest->b1 = false;
    PrintTest(newTest);

    newTest = (test*) DictionaryGet(newDict, &testKey);
    PrintTest(newTest);

    test otherTester;
    otherTester.i1 = 1;
    otherTester.i2 = 2;
    otherTester.f1 = 3.0f;
    otherTester.b1 = false;

    test_key otherTestKey;
    otherTestKey.u8 = 69;
    otherTestKey.u8Other = 69;

    DictionaryAdd(newDict, &otherTestKey, &otherTester);

    newTest = (test*) DictionaryGet(newDict, &otherTestKey);
    PrintTest(newTest);

    otherTester.i1 = -4;
    otherTester.i2 = -5;
    otherTester.f1 = -6.0f;
    otherTester.b1 = true;

    otherTestKey.u8 = 40;
    otherTestKey.u8Other = 69;

    DictionaryAdd(newDict, &otherTestKey, &otherTester);

    newTest = (test*) DictionaryGet(newDict, &otherTestKey);
    PrintTest(newTest);

    otherTester.i1 = -7;
    otherTester.i2 = -8;
    otherTester.f1 = -9.0f;
    otherTester.b1 = false;

    otherTestKey.u8 = 72;
    otherTestKey.u8Other = 69;

    DictionaryAdd(newDict, &otherTestKey, &otherTester);

    newTest = (test*) DictionaryGet(newDict, &otherTestKey);
    PrintTest(newTest);

    DictionaryRemove(newDict, &testKey);

    otherTestKey.u8 = 40;
    otherTestKey.u8Other = 69;
    DictionaryRemove(newDict, &otherTestKey);

    otherTestKey.u8 = 69;
    otherTestKey.u8Other = 69;
    newTest = (test*) DictionaryGet(newDict, &otherTestKey);
    PrintTest(newTest);

    otherTestKey.u8 = 72;
    otherTestKey.u8Other = 69;
    newTest = (test*) DictionaryGet(newDict, &otherTestKey);
    PrintTest(newTest);

    DictionaryFree(newDict);
    newDict = NULL;
}

void BucketArrayTest()
{
    BucketArray* bucketArray = BucketArrayNew(sizeof(test), 5);

    test tester;
    tester.i1 = 9;
    tester.i2 = 1;
    tester.f1 = 9.6549f;
    tester.b1 = false;

    BucketArrayAdd(bucketArray, &tester);
    tester.i2 = 2;
    BucketArrayAdd(bucketArray, &tester);
    tester.i2 = 3;
    BucketArrayAdd(bucketArray, &tester);
    tester.i2 = 4;
    BucketArrayAdd(bucketArray, &tester);
    tester.i2 = 5;
    BucketArrayAdd(bucketArray, &tester);
    tester.i2 = 6;
    BucketArrayAdd(bucketArray, &tester);
    tester.i2 = 7;
    BucketArrayAdd(bucketArray, &tester);
    tester.i2 = 8;
    BucketArrayAdd(bucketArray, &tester);
    tester.i2 = 9;
    BucketArrayAdd(bucketArray, &tester);
    tester.i2 = 10;
    BucketArrayAdd(bucketArray, &tester);
    tester.i2 = 11;
    BucketArrayAdd(bucketArray, &tester);

    PrintBucketArray(bucketArray);

    BucketArrayPopBack(bucketArray, NULL);
    PrintBucketArray(bucketArray);

    test newtester = *(test*) BucketArrayGet(bucketArray, 5);

    printf("%" PRId64 ", %" PRId64 ", %f, %d\n", newtester.i1, newtester.i2, newtester.f1, newtester.b1);

    BucketArrayFree(bucketArray);
    bucketArray = NULL;
}

int main()
{
    // array_test();
    // BucketArrayTest();
    dict_test();

    // system("pause");

    return 0;
}