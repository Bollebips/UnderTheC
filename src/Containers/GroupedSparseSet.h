#ifndef GROUPED_SPARSE_SET_I
#define GROUPED_SPARSE_SET_I

#include "SparseSet.h"
#include "Array.h"

typedef struct GroupedSparseSet
{
    SparseSet sparseSet;
    Array groups;
}GroupedSparseSet;

GroupedSparseSet* GroupedSparseSetNew(const size_t elementSize, const uint64_t(*getIndexFromDataFunc)(const void*), const uint64_t bucketCapacity, const uint64_t initialNumGroups);
void GroupedSparseSetAdd(GroupedSparseSet* groupedSparseSet, const void* newElement, const int groupToAddTo);
void GroupedSparseSetRemove(GroupedSparseSet* groupedSparseSet, const uint64_t index);
void* GroupedSparseSetGet(GroupedSparseSet* groupedSparseSet, const uint64_t index);
bool GroupedSparseSetContains(GroupedSparseSet* groupedSparseSet, const uint64_t index);
int GroupedSparseSetAddGroup(GroupedSparseSet* groupedSparseSet);
void GroupedSparseSetAddElementToGroup(GroupedSparseSet* groupedSparseSet, const uint64_t elementIndex, const int groupToAddTo);
int GroupedSparseSetGetGroupFromElement(GroupedSparseSet* groupedSparseSet, const uint64_t elementToGetGroupFrom);
void GroupedSparseSetFree(GroupedSparseSet* groupedSparseSet);

void GroupedSparseSetInit(GroupedSparseSet* sparseSet, const size_t elementSize, const uint64_t(*getIndexFromDataFunc)(const void*), uint64_t bucketCapacity);
void GroupedSparseSetDeinit(GroupedSparseSet* sparseSet);

#endif