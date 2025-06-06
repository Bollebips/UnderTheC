#include "Game.h"
#include "Renderer.h"
#include "Logger.h"

#include <stdio.h>

int RunGame()
{
    Renderer renderer;

    bool rendererInitSuccess = RendererInit(&renderer);

    if(!rendererInitSuccess)
    {
        RendererCleanup(&renderer);
        return EXIT_FAILURE;
    }

    Render(&renderer);

    RendererCleanup(&renderer);

    LogInfo("Success!");
    return EXIT_SUCCESS;
}
