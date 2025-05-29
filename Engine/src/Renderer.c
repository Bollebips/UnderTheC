#include "Renderer.h"

#include <stdlib.h>
#include <time.h>
#include <math.h>

#include <Logger.h>
#include <Utils/FileIO.h>

static GLuint CreateComputeShader(const char*);
static void GlfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

static bool MoveUp, MoveDown,
            MoveLeft, MoveRight,
            MoveForward, MoveBackward,
            LookUp, LookDown,
            LookLeft, LookRight = false;

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

    int version = gladLoadGL(glfwGetProcAddress);
    LogInfo("GL %d.%d", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

    LogInfo("Graphics device: %s", glGetString(GL_RENDERER));

    glfwSetKeyCallback(renderer->Window, GlfwKeyCallback);

    glViewport(0, 0, width, height);

    GLuint computeShaderHandle = CreateComputeShader("resources/voxelShader.glsl");

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, computeShaderHandle);
    glLinkProgram(shaderProgram);
    renderer->ShaderProgram = shaderProgram;
    glUseProgram(renderer->ShaderProgram);

    renderer->Camera = (Camera)
    {
        .Transform = GLMS_MAT4_IDENTITY,
        .Speed = 1.0f,
        .AngularSpeed = 1.0f,
        .NearPlaneDistance = 0.01f,
        .FarPlaneDistance = 50.0f,
        .VerticalFov = 65.0f
    };
    glm_translate(renderer->Camera.Transform.raw, (vec3){0, 1, -1});

    renderer->Texture = CreateTexture(width, height);
    glCreateFramebuffers(1, &renderer->Framebuffer);
    AttachTextureToFramebuffer(renderer);

    return EXIT_SUCCESS;
}

void Render(Renderer* renderer)
{
    struct timespec prevTime, currentTime;
    timespec_get(&prevTime, TIME_UTC);

    GLint cameraTransformAttribute = glGetUniformLocation(renderer->ShaderProgram, "cameraTransform");

    const GLuint workGroupSizeX = 16;
    const GLuint workGroupSizeY = 16;

    float averageDeltaTime = 0;
    u64 deltaTimeSampleCount = 0;

    //Disable v-sync
    /* glfwSwapInterval(0); */

    while (!glfwWindowShouldClose(renderer->Window))
    {
        timespec_get(&currentTime, TIME_UTC);
        float deltaTime = (currentTime.tv_sec - prevTime.tv_sec) +
                           (currentTime.tv_nsec - prevTime.tv_nsec) * 1e-9;
        timespec_get(&prevTime, TIME_UTC);

        averageDeltaTime = ((averageDeltaTime * deltaTimeSampleCount) + deltaTime) / (deltaTimeSampleCount + 1);
        deltaTimeSampleCount++;

        CameraProcessInput(&renderer->Camera, deltaTime, MoveForward, MoveBackward, MoveLeft, MoveRight, MoveUp, MoveDown, LookDown, LookUp, LookLeft, LookRight);

        int width, height;

        glfwGetFramebufferSize(renderer->Window, &width, &height);

        if(width != renderer->Texture.Width || height != renderer->Texture.Height)
        {
            glDeleteTextures(1, &renderer->Texture.Handle);
            renderer->Texture = CreateTexture(width, height);
            AttachTextureToFramebuffer(renderer);
        }

        glClearColor(0, 0.5f, 0.75f, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(renderer->ShaderProgram);

        glUniformMatrix4fv(cameraTransformAttribute, 1, GL_FALSE, renderer->Camera.Transform.raw[0]);

        GLuint numGroupsX = (width + workGroupSizeX - 1) / workGroupSizeX;
        GLuint numGroupsY = (height + workGroupSizeY - 1) / workGroupSizeY;

        glBindImageTexture(0, renderer->Texture.Handle, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
        glDispatchCompute(numGroupsX, numGroupsY, 1);
        glMemoryBarrier(GL_ALL_BARRIER_BITS);

        BlitFramebufferToSwapchain(renderer->Framebuffer, &renderer->Texture);

        glfwSwapBuffers(renderer->Window);
        glfwPollEvents();
    }

    LogInfo("Average deltatime = %f", averageDeltaTime);
}

void RendererCleanup(Renderer* renderer)
{
    glfwDestroyWindow(renderer->Window);
    glfwTerminate();
}

static GLuint CreateComputeShader(const char* shaderFilePath)
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

    return voxelShader;
}

void AttachTextureToFramebuffer(Renderer* renderer)
{
    glNamedFramebufferTexture(renderer->Framebuffer, GL_COLOR_ATTACHMENT0, renderer->Texture.Handle, 0);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        LogError("Framebuffer is not complete.");
        glDeleteFramebuffers(1, &renderer->Framebuffer);
        exit(1);
    }

    renderer->ViewportDimensionsAttribute = glGetUniformLocation(renderer->ShaderProgram, "viewportDimensions");
    float viewportHeight = renderer->Camera.NearPlaneDistance * tanf(glm_rad(renderer->Camera.VerticalFov * 0.5f)) * 2.0f;
    vec4 viewportDimensions =
    {
        viewportHeight * ((float)renderer->Texture.Width / (float)renderer->Texture.Height),
        viewportHeight,
        renderer->Camera.NearPlaneDistance,
        renderer->Camera.FarPlaneDistance
    };

    glUniform4fv(renderer->ViewportDimensionsAttribute, 1, (const GLfloat*)&viewportDimensions);
}

void BlitFramebufferToSwapchain(const GLuint framebuffer, const Texture* texture)
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    glBlitFramebuffer(0, 0, texture->Width, texture->Height,
                      0, 0, texture->Width, texture->Height,
                      GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

static void GlfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(action == GLFW_PRESS)
    {
        switch(key)
        {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, true);
                break;
            case GLFW_KEY_W:
                MoveForward = true;
                break;
            case GLFW_KEY_A:
                MoveLeft = true;
                break;
            case GLFW_KEY_S:
                MoveBackward = true;
                break;
            case GLFW_KEY_D:
                MoveRight = true;
                break;
            case GLFW_KEY_E:
                MoveUp = true;
                break;
            case GLFW_KEY_Q:
                MoveDown = true;
                break;
             case GLFW_KEY_UP:
                LookUp = true;
                break;
            case GLFW_KEY_DOWN:
                LookDown = true;
                break;
             case GLFW_KEY_LEFT:
                LookLeft = true;
                break;
            case GLFW_KEY_RIGHT:
                LookRight = true;
                break;
        }
    }
    else if(action == GLFW_RELEASE)
    {
        switch(key)
        {
            case GLFW_KEY_W:
                MoveForward = false;
                break;
            case GLFW_KEY_A:
                MoveLeft = false;
                break;
            case GLFW_KEY_S:
                MoveBackward = false;
                break;
            case GLFW_KEY_D:
                MoveRight = false;
                break;
            case GLFW_KEY_E:
                MoveUp = false;
                break;
            case GLFW_KEY_Q:
                MoveDown = false;
                break;
            case GLFW_KEY_UP:
                LookUp = false;
                break;
            case GLFW_KEY_DOWN:
                LookDown = false;
                break;
            case GLFW_KEY_LEFT:
                LookLeft = false;
                break;
            case GLFW_KEY_RIGHT:
                LookRight = false;
                break;
        }
    }
}
