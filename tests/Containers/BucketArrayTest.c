#include "Containers/BucketArray.h"

void TestBucketArrayAdd()
{
    BucketArray* bucketArray = BucketArrayNew(sizeof(char*), 1);
    TEST_CHECK(bucketArray != NULL);

    char* testString = "Hello world";

    BucketArrayAdd(bucketArray, &testString);
    TEST_CHECK(bucketArray->num == 1);
    TEST_CHECK(bucketArray->bucketPtrs.num == 1);

    char* testString2 = "Hello sailor";

    char* returnValue = *(char**) BucketArrayAdd(bucketArray, &testString2);
    TEST_CHECK(bucketArray->num == 2);
    TEST_CHECK(bucketArray->bucketPtrs.num == 2);
    TEST_CHECK(strcmp(returnValue, testString2) == 0);

    returnValue = *(char**) BucketArrayGet(bucketArray, BucketArrayNum(bucketArray) - 2);
    TEST_CHECK_(strcmp(returnValue, testString) == 0, "%s, %s = %d", returnValue, "Hello world", strcmp(returnValue, "Hello world"));
}

void TestBucketArrayResize()
{
    BucketArray* bucketArray = BucketArrayNew(sizeof(int), 5);

    int newNr = 0;

    for(int i = 0; i < 5; ++i)
    {
        newNr = rand();
        BucketArrayAdd(bucketArray, &newNr);
    }

    TEST_CHECK(bucketArray->num == 5);
    TEST_CHECK(bucketArray->bucketPtrs.num == 1);

    BucketArrayResize(bucketArray, 3);
    TEST_CHECK(bucketArray->num == 3);
    TEST_CHECK(bucketArray->bucketPtrs.num == 1);

    BucketArrayResize(bucketArray, 5);
    TEST_CHECK(bucketArray->num == 3);
    TEST_CHECK(bucketArray->bucketPtrs.num == 1);

    BucketArrayResize(bucketArray, 6);
    TEST_CHECK(bucketArray->num == 3);
    TEST_CHECK(bucketArray->bucketPtrs.num == 2);

    BucketArrayResize(bucketArray, 3);
    TEST_CHECK(bucketArray->num == 3);
    TEST_CHECK(bucketArray->bucketPtrs.num == 1);

    BucketArrayResize(bucketArray, 0);
    TEST_CHECK(bucketArray->num == 0);
    TEST_CHECK(bucketArray->bucketPtrs.num == 0);

    BucketArrayAdd(bucketArray, &newNr);
    TEST_CHECK(bucketArray->num == 1);
    TEST_CHECK(bucketArray->bucketPtrs.num == 1);
}

void TestBucketArrayClear()
{
    BucketArray* bucketArray = BucketArrayNew(sizeof(int), 5);

    int newNr = 0;

    for(int i = 0; i < 5; ++i)
    {
        newNr = rand();
        BucketArrayAdd(bucketArray, &newNr);
    }

    TEST_CHECK(bucketArray->num == 5);
    TEST_CHECK(bucketArray->bucketPtrs.num == 1);

    BucketArrayClear(bucketArray);
    TEST_CHECK(bucketArray->num == 0);
    TEST_CHECK(bucketArray->bucketPtrs.num == 1);

}

void TestBucketArray()
{
    TestBucketArrayAdd();
    TestBucketArrayResize();
    TestBucketArrayClear();
}