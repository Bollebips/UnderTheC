#include "Renderer.h"

#include <stdlib.h>

#include <Logger.h>
#include <Utils/FileIO.h>

GLuint CreateComputeShader(const char*);

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

    GLuint computeShaderHandle = CreateComputeShader("../voxelShader.glsl");

    renderer->ShaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, comupteShaderHandle);
    glLinkProgram(shaderProgram);

    Texture texture = CreateTexture(width, height);
    glBindImageTexture(0, texture.Handle, 0, GL_FALSE, GL_WRITE_ONLY, GL_RGBA32F);

    return EXIT_SUCCESS;
}

void Render(Renderer* renderer)
{
    while (!glfwWindowShouldClose(renderer->Window))
    {
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(renderer->ShaderProgram);
        glDispatchCompute(renderer->Window.Width, renderer->Window.Height, 1);
        //make ALL barriers wait until this compute shader is done.
        glMemoryBarrier(GL_ALL_BARRIER_BITS);

        glfwSwapBuffers(renderer->Window);
        glfwPollEvents();
    }
}

void RendererCleanup(Renderer* renderer)
{
    glfwDestroyWindow(renderer->Window);
    glfwTerminate();
}

GLuint CreateComputeShader(const char* shaderFilePath)
{
    char statusLog[512];
    GLuint shaderCompileStatus;

    const char* shaderSource = GetStringFromFile(shaderFilePath);
    GLuint voxelShader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(voxelShader, 1, &shaderSource, NULL);
    free((char*) shaderSource);
    glCompileShader(voxelShader);

    glGetShaderiv(voxelShader, GL_COMPILE_STATUS, &shaderCompileStatus);
    if(shaderCompileStatus == false)
    {
        glGetShaderInfoLog(voxelShader, 512, NULL, statusLog);
        LogError("ComputeShader compilation failed: %s", statusLog);
    }
}
