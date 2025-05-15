#include "Renderer.h"

#include <stdlib.h>

#include <Logger.h>

void RendererGlfwFrameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

int RendererInit(Renderer* renderer)
{
    if(glfwInit() == false)
    {
        LogError("Failed to initialize glfw.");
        return EXIT_FAILURE;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    int width = 640;
    int height = 480;
    renderer->Window = glfwCreateWindow(width, height, "Voxel renderer", NULL, NULL);
    LogAssert(renderer->Window != NULL, "GLFW window creation failed.");

    glfwMakeContextCurrent(renderer->Window);
    
    glfwSetFramebufferSizeCallback(renderer->Window, RendererGlfwFrameBufferSizeCallback);
    glViewport(0, 0, width, height);

    glewExperimental = GL_TRUE;
    GLenum glewInitResult = glewInit();
    LogAssert(glewInitResult != GLEW_OK, "Glew did not initialize correctly.");

    return EXIT_SUCCESS;
}

void Render(Renderer* renderer)
{
    while (!glfwWindowShouldClose(renderer->Window))
    { 
        glfwSwapBuffers(renderer->Window);
        glfwPollEvents();
    }
}

void RendererCleanup(Renderer* renderer)
{
    glfwDestroyWindow(renderer->Window);
    glfwTerminate();
}
