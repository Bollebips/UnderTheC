#include "acutest.h"

#include "Containers/BucketArrayTest.c"
#include "Containers/DictionaryTest.c"
#include "Containers/SparseSetTest.c"
#include "Core/ECSTest.c"

TEST_LIST = {
    {"TestBucketArray", TestBucketArray },
    {"TestDictionary", TestDictionary },
    {"TestSparseSet", TestSparseSet },
    {"TestECS", TestECS },
    {0}
};