#include "Game.h"
#include "Renderer.h"
#include "Logger.h"

#include <stdio.h>

int RunGame()
{
    Renderer renderer;

    int rendererInitResult = RendererInit(&renderer);

    if(rendererInitResult != EXIT_SUCCESS)
    {
        RendererCleanup(&renderer);
        return rendererInitResult;
    }

    Render(&renderer);

    RendererCleanup(&renderer);

    LogInfo("Success!");
    return EXIT_SUCCESS;
}
