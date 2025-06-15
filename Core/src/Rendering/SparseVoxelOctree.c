#include <math.h>
#include <stddef.h>

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

void PopulateVoxelChunk(SparseVoxelOctree* result, u8 denseData[])
{
    LogAssert(result != NULL);

    int voxelArrayLength = pow(VOXEL_CHUNK_SIZE, 3);

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
