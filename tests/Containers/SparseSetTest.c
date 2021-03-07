#include "Containers/SparseSet.h"

typedef struct SparseSetData
{
    uint64_t index;
    char* data;
}SparseSetData;

uint64_t DataGetIndex(const void* data)
{
    SparseSetData* sparseSetData = (SparseSetData*) data;
    return sparseSetData->index;
}

void DataSetIndex(void* data, uint64_t index)
{
    SparseSetData* sparseSetData = (SparseSetData*) data;
    sparseSetData->index = index;
}

void TestSparseSet()
{
    SparseSet* s = SparseSetNew(sizeof(SparseSetData), DataGetIndex, 3);

    TEST_CHECK(s != NULL);

    TEST_CHECK(SparseSetContains(s, 0) == false);
    SparseSetData sData = { 0, "Hello" };
    SparseSetAdd(s, &sData);
    TEST_CHECK(SparseSetContains(s, 0) == true);

    sData.index = 2;
    sData.data = "World";
    SparseSetAdd(s, &sData);
    TEST_CHECK(SparseSetContains(s, 2) == true);
    TEST_CHECK(BucketArrayNum(SparseSetGetDenseData(s)) == 2);

    SparseSetRemove(s, 0);
    TEST_CHECK(SparseSetContains(s, 2) == true);
    TEST_CHECK(SparseSetContains(s, 0) == false);
    TEST_CHECK(BucketArrayNum(SparseSetGetDenseData(s)) == 1);

    // TEST_CHECK(SparseSetGet()
}