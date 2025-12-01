#include <math.h>
#include <stddef.h>

#include "Containers/Array.h"
#include "Logger.h"
#include "SparseVoxelOctree.h"

// int CalculateOctreeSize(int denseDataSize)
//{
//     int levels = 1;
//
//     for (u8 branchCount = denseDataSize; branchCount > 1; levels++)
//     {
//         LogAssert(branchCount % 8 == 0, "Dense voxel brick count should be a
//         power of 8"); branchCount /= 8;
//     }
//
//     return pow(8, levels);
// }

void PopulateVoxelLayer(SparseVoxelOctree* result, u8 denseData[])
{
    LogAssert(result != NULL);

    int denseDataSize = (int)pow(VOXEL_CHUNK_SIZE, 3);
    LogAssert(denseDataSize % 8 == 0);

    Array(u16) parents;
    Array(u16) children;

    for (int p = 0; p < denseDataSize / 8; ++p)
    {
        u16 parent;
        u8 occupancyMask;
        u8 childCount;

        for (int i = 0; i < 8; ++i)
        {
            u8 child = denseData[(p * 8) + i];

            if (child > 0)
            {
                occupancyMask |= 1 << i;
                ArrayAdd(&children, child);
            }
        }

        if (occupancyMask > 0)
        {
            parent = childCount << 8;
            parent |= occupancyMask;
            ArrayAdd(&parents, parent);
        }
    }

    u8 totalChildCount = 0;

    for (int i = 0; i < parents.Length; ++i)
    {
        u8 childPointer = parents.Length + totalChildCount;
        u16 parent = parents.Items[i];
        totalChildCount += parent >> 8;
        parent |= childPointer << 8;
        ArrayAdd(result, parent);
    }

    for (int i = 0; i < children.Length; ++i)
    {
        ArrayAdd(result, children.Items[i]);
    }
}

void PopulateVoxelChunk(SparseVoxelOctree* result, u8 denseData[])
{
    LogAssert(result != NULL);

    int voxelArrayLength = (int)pow(VOXEL_CHUNK_SIZE, 3);

    int numLevels = 0;

    for (int branchCount = voxelArrayLength; branchCount > 1; numLevels++)
    {
        LogAssert(branchCount % 8 == 0, "Dense voxel brick count should be a power of 8 (was %d instead)", branchCount);
        branchCount /= 8;
    }

    LogInfo("%d", numLevels);

    for (int i = 0; i < voxelArrayLength; ++i)
    {
        u8 voxelBrick = denseData[i];
        if (voxelBrick != 0)
        {
            if (result->Length == 0) ArrayAdd(result, 0); // TODO: Better check

            result->Items[0] |= 1 << i;
            ArrayAdd(result, voxelBrick);
        }
    }
    /*

result->VoxelData = alloca(pow(8, levels));

result->VoxelData = denseData;

LogInfo("%d", *result->VoxelData);
*/
    // u8[pow(8, levels)]{};
}

u16 PopulateVoxelBrick(u16 children[8])
{
    u16 resultingParent = 0;

    // the size in bytes of this voxel tree data, including itself and all its children
    u8 size = 1;
    u8 occupancyMask = 0;

    for (int i = 0; i < 8; ++i)
    {
        u16 child = children[i];
        u8 childSize = (child >> 8);
        if (childSize == 0)
            childSize = 1;

        size += childSize;

        u8 childOccupancyMask = child & 0xFF;
        if (childOccupancyMask > 0)
        {
            occupancyMask |= 1 << i;
        }
    }
}
