#include "Containers/Array.h"
#include "Game.h"
#include "Logger.h"
#include "Rendering/Renderer.h"
#include "Rendering/SparseVoxelOctree.h"
#include <math.h>

int RunGame()
{
    VoxelChunk chunk = {0};
    const u8 voxelArrayLength = 64;
    u8 voxelArray[voxelArrayLength] = {};
    voxelArray[0] = 0x50;
    voxelArray[1] = 0xA0;
    voxelArray[2] = 0x50;
    voxelArray[3] = 0xAF;
    voxelArray[4] = 0x8F;
    voxelArray[5] = 0x8F;
    voxelArray[6] = 0xFF;
    voxelArray[7] = 0xFF;

    LogAssert(sizeof(voxelArray) == pow(VOXEL_CHUNK_SIZE, 3));

    PopulateVoxelChunk(&chunk.Octree, voxelArray);

    LogInfo("%d", chunk.Octree.Items[0]);

    Renderer renderer;

    bool rendererInitSuccess = RendererInit(&renderer);

    if (!rendererInitSuccess)
    {
        RendererCleanup(&renderer);
        return EXIT_FAILURE;
    }

    Render(&renderer);

    RendererCleanup(&renderer);

    LogInfo("Success!");
    return EXIT_SUCCESS;
}
