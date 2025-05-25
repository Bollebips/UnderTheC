#include <Logger.h>

#include "OpenGlExercise.h"
#include "Renderer.h"

int main()
{
    /* return Exercise(); */

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
