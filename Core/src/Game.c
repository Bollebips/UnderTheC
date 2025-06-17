#include "Containers/Array.h"
#include "Game.h"
#include "Logger.h"
#include "Rendering/Renderer.h"
#include "Rendering/SparseVoxelOctree.h"
#include <math.h>

int RunGame()
{
    Renderer renderer = {0};

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
