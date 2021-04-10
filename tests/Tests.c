#include "acutest.h"

#include "Containers/BucketArrayTest.c"
#include "Containers/DictionaryTest.c"
#include "Containers/SparseSetTest.c"
#include "Containers/GroupedSparseSetTest.c"
#include "Core/ECSTest.c"

TEST_LIST = {
    {"TestBucketArray", TestBucketArray },
    {"TestDictionary", TestDictionary },
    {"TestSparseSet", TestSparseSet },
    {"TestGroupedSparseSet", TestGroupedSparseSet },
    {"TestECS", TestECS },
    {0}
};