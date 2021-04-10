#include "GroupedSparseSet.h"

#include "Logger.h"

#include <string.h>

static void GroupedSparseSetSwapElements(GroupedSparseSet* groupedSparseSet, const uint64_t element1Index, const uint64_t element2Index);

GroupedSparseSet* GroupedSparseSetNew(const size_t elementSize, const uint64_t(*getIndexFromDataFunc)(const void*), const uint64_t bucketCapacity, const uint64_t initialNumGroups)
{
    LogAssert(elementSize > 0);
    LogAssert(getIndexFromDataFunc);
    LogAssert(bucketCapacity);
    LogAssert(initialNumGroups > 0);

    GroupedSparseSet* newGroupedSparseSet = malloc(sizeof(GroupedSparseSet));

    SparseSetInit(&(newGroupedSparseSet->sparseSet), elementSize, getIndexFromDataFunc, bucketCapacity);
    ArrayInit(&(newGroupedSparseSet->groups), sizeof(uint64_t), initialNumGroups);

    return newGroupedSparseSet;
}

void GroupedSparseSetAdd(GroupedSparseSet* groupedSparseSet, const void* newElement, const int groupToAddTo)
{
    LogAssert(groupedSparseSet);
    LogAssert(newElement);
    LogAssert(groupToAddTo < ArrayNum(&(groupedSparseSet->groups)));

    SparseSetAdd(&(groupedSparseSet->sparseSet), newElement);

    BucketArray* denseData = SparseSetGetDenseData(&(groupedSparseSet->sparseSet));
    uint64_t lastIndex = BucketArrayNum(denseData) - 1;

    if(groupToAddTo > -1)
    {
        GroupedSparseSetAddElementToGroup(groupedSparseSet, lastIndex, groupToAddTo);
    }
}

void GroupedSparseSetRemove(GroupedSparseSet* groupedSparseSet, const uint64_t index)
{
    LogAssert(groupedSparseSet);
    LogAssert(index < BucketArrayNum(SparseSetGetDenseData(&(groupedSparseSet->sparseSet))));

    //Check if the element is in a group

    uint64_t prevGroupLastIndex = 0;
    uint64_t currentGroupLastIndex;
    for(int i = 0; i < ArrayNum(&(groupedSparseSet->groups)); ++i)
    {
        currentGroupLastIndex = *(uint64_t*) ArrayGet(&(groupedSparseSet->groups), i);
        if(index >= prevGroupLastIndex && index < currentGroupLastIndex)
        {

        }
        prevGroupLastIndex = currentGroupLastIndex;
    }
}

void* GroupedSparseSetGet(GroupedSparseSet* groupedSparseSet, const uint64_t index)
{
    return NULL;
}

bool GroupedSparseSetContains(GroupedSparseSet* groupedSparseSet, const uint64_t index)
{
    return 0;
}

int GroupedSparseSetAddGroup(GroupedSparseSet* groupedSparseSet)
{
    LogAssert(groupedSparseSet);

    uint64_t groupID = ArrayNum(&(groupedSparseSet->groups));
    ArrayAdd(&(groupedSparseSet->groups), &groupID);

    return groupID;
}

void GroupedSparseSetAddElementToGroup(GroupedSparseSet* groupedSparseSet, const uint64_t elementIndex, const int groupToAddTo)
{
    LogAssert(groupedSparseSet);
    LogAssert(elementIndex < BucketArrayNum(SparseSetGetDenseData(&(groupedSparseSet->sparseSet))));
    LogAssert(groupToAddTo >= 0 && groupToAddTo < ArrayNum(&(groupedSparseSet->groups)));

    int elementPrevGroup = GroupedSparseSetGetGroupFromElement(groupedSparseSet, elementIndex);

    for(int i = groupToAddTo; i < ArrayNum(&(groupedSparseSet->groups)); ++i)
    {
        uint64_t* groupIndex = (uint64_t*) ArrayGet(&(groupedSparseSet->groups), i);

        if(i >= elementPrevGroup || *groupIndex >= elementIndex) // The value of these 2 checks should always be the same!!
        {
            return;
        }

        GroupedSparseSetSwapElements(groupedSparseSet, *groupIndex, elementIndex);
        *groupIndex += 1;
    }
}

int GroupedSparseSetGetGroupFromElement(GroupedSparseSet* groupedSparseSet, const uint64_t elementToGetGroupFrom)
{
    LogAssert(groupedSparseSet);
    LogAssert(elementToGetGroupFrom < BucketArrayNum(SparseSetGetDenseData(&(groupedSparseSet->sparseSet))));

    uint64_t prevGroupLastIndex = 0;
    uint64_t currentGroupLastIndex;

    for(int i = 0; i < ArrayNum(&(groupedSparseSet->groups)); ++i)
    {
        currentGroupLastIndex = *(uint64_t*) ArrayGet(&(groupedSparseSet->groups), i);
        if(elementToGetGroupFrom >= prevGroupLastIndex && elementToGetGroupFrom < currentGroupLastIndex)
        {
            return i;
        }
    }

    return -1;
}

void GroupedSparseSetFree(GroupedSparseSet* groupedSparseSet)
{

}

void GroupedSparseSetInit(GroupedSparseSet* sparseSet, const size_t elementSize, const uint64_t(*getIndexFromDataFunc)(const void*), uint64_t bucketCapacity)
{

}

void GroupedSparseSetDeinit(GroupedSparseSet* sparseSet)
{

}

/* ----------------------------------------------------- PRIVATE ---------------------------------------------------- */

void GroupedSparseSetSwapElements(GroupedSparseSet* groupedSparseSet, const uint64_t element1Index, const uint64_t element2Index)
{
    LogAssert(groupedSparseSet);
    LogAssert(element1Index != element2Index);

    BucketArray* denseData = SparseSetGetDenseData(&(groupedSparseSet->sparseSet));

    LogAssert(element1Index < BucketArrayNum(denseData));
    LogAssert(element2Index < BucketArrayNum(denseData));

    size_t elementSize = denseData->elementSize;

    void* tempElement = malloc(elementSize);
    void* element1 = BucketArrayGet(denseData, element1Index);
    void* element2 = BucketArrayGet(denseData, element2Index);

    memcpy(tempElement, element1, elementSize);
    memcpy(element1, element2, elementSize);
    memcpy(element2, tempElement, elementSize);
}