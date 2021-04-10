#include "Containers/GroupedSparseSet.h"

typedef struct GroupedSparseSetData
{
    uint64_t index;
    char* data;
}GroupedSparseSetData;

uint64_t DataGetIndex(const void* data)
{
    GroupedSparseSetData* groupedSparseSetData = (GroupedSparseSetData*) data;
    return groupedSparseSetData->index;
}

void DataSetIndex(void* data, uint64_t index)
{
    GroupedSparseSetData* groupedSparseSetData = (GroupedSparseSetData*) data;
    groupedSparseSetData->index = index;
}

void TestGroupedSparseSet()
{
    GroupedSparseSet* s = GroupedSparseSetNew(sizeof(GroupedSparseSetData), DataGetIndex, 3);

    TEST_CHECK(s != NULL);
    TEST_CHECK(GroupedSparseSetContains(s, 0) == false);

    GroupedSparseSetData group1[2];
    group1[0].data = "Hello";
    group1[0].index = 0;
    group1[1].data = "world";
    group1[1].index = 1;

    GroupedSparseSetData group2[4];
    group2[0].data = "This";
    group2[0].index = 2;
    group2[1].data = "is";
    group2[1].index = 3;
    group2[2].data = "a";
    group2[2].index = 4;
    group2[3].data = "test";
    group2[3].index = 5;

    GroupedSparseSetData group3[3];
    group3[0].data = "Under";
    group3[0].index = 6;
    group3[1].data = "the";
    group3[1].index = 7;
    group3[2].data = "C";
    group3[2].index = 8;

    for(int i = 0; i < 2; ++i)
    {
        GroupedSparseSetAdd(s, &(group1[i]), 0);
    }

    TEST_CHECK(GroupedSparseSetContains(s, 0) == true);
    TEST_CHECK(GroupedSparseSetContains(s, 1) == true);
    TEST_CHECK(GroupedSparseSetContains(s, 2) == true);

    TEST_CHECK(BucketArrayNum(SparseSetGetDenseData(s)) == 3);
    TEST_CHECK(ArrayNum(&(s->groups)) == 1);



    GroupedSparseSetRemove(s, 0);
    TEST_CHECK(GroupedSparseSetContains(s, 2) == true);
    TEST_CHECK(GroupedSparseSetContains(s, 0) == false);
    TEST_CHECK(BucketArrayNum(GroupedSparseSetGetDenseData(s)) == 1);
}