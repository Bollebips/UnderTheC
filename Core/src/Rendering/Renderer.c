#include "Renderer.h"
#include "Rendering/SparseVoxelOctree.h"

#include <math.h>
#include <stdlib.h>
#include <time.h>

#include <Logger.h>
#include <Utils/FileIO.h>

bool RendererInit(Renderer* renderer)
{
    if (glfwInit() == false)
    {
        LogError("Failed to initialize glfw.");
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    int width = 1280;
    int height = 720;
    GLFWwindow* window = glfwCreateWindow(width, height, "Voxel renderer", NULL, NULL);
    LogAssert(window != NULL, "GLFW window creation failed.");
    renderer->Window = window;
    glfwSetWindowUserPointer(window, renderer);

    glfwMakeContextCurrent(window);

    int version = gladLoadGL(glfwGetProcAddress);
    LogInfo("GL %d.%d", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

    LogInfo("Graphics device: %s", glGetString(GL_RENDERER));

    glfwSetKeyCallback(window, GlfwKeyCallback);
    glfwSetWindowSizeCallback(window, GlfwWindowSizeCallback);

    glViewport(0, 0, width, height);

    GLuint computeShaderHandle = CreateComputeShader("resources/voxelShader_v2.glsl");

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, computeShaderHandle);
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);
    renderer->ShaderProgram = shaderProgram;

    renderer->Camera = (Camera){.Transform = GLMS_MAT4_IDENTITY,
                                .LinearSpeed = 1.0f,
                                .AngularSpeed = 1.0f,
                                .NearPlaneDistance = 0.01f,
                                .FarPlaneDistance = 50.0f,
                                .VerticalFov = 65.0f};

    vec3 startLocation = {1.0f, 1.5f, 1.5f};
    vec3 lookTarget = {0.0f, 0.0f, 0.0f};
    vec3 right, up, forward;
    glm_vec3_sub(lookTarget, startLocation, forward);
    glm_vec3_normalize(forward);

    glm_vec3_crossn(forward, GLM_YUP, right);
    glm_vec3_crossn(right, forward, up);

    glm_vec3_copy(right, renderer->Camera.Transform.raw[0]);
    glm_vec3_copy(up, renderer->Camera.Transform.raw[1]);
    glm_vec3_copy(forward, renderer->Camera.Transform.raw[2]);
    glm_vec3_copy(startLocation, renderer->Camera.Transform.raw[3]);

    glCreateFramebuffers(1, &renderer->Framebuffer);
    renderer->Texture = CreateTexture(width, height);
    AttachTextureToFramebuffer(renderer);

    u8 voxelArray[64] = {0};
    // voxelArray[0] = 0xFF;
    // voxelArray[1] = 0xFF;
    // voxelArray[2] = 0xFF;
    // voxelArray[3] = 0x88;
    // voxelArray[4] = 0xF0;
    // voxelArray[5] = 0xA5;
    // voxelArray[6] = 0xA5;
    // voxelArray[7] = 0xA5;
    voxelArray[0] = 0xFF;
    voxelArray[1] = 0xFF;
    voxelArray[2] = 0xCD;
    voxelArray[3] = 0xCD;
    voxelArray[4] = 0xDD;
    voxelArray[5] = 0x22;
    voxelArray[6] = 0x00;
    voxelArray[7] = 0x22;

    LogAssert(sizeof(voxelArray) / sizeof(voxelArray[0]) == pow(VOXEL_CHUNK_SIZE, 3), "Size was %d but should be %f", sizeof(voxelArray) / sizeof(voxelArray[0]), pow(VOXEL_CHUNK_SIZE, 3));

    PopulateVoxelChunk(&renderer->VoxelChunk.Octree, voxelArray);

    for (int i = 0; i < renderer->VoxelChunk.Octree.Length; i++)
    {
        LogInfo("Octree[%d] = %d", i, renderer->VoxelChunk.Octree.Items[i]);
    }

    return true;
}

void Render(Renderer* renderer)
{
    struct timespec prevTime, currentTime;
    timespec_get(&prevTime, TIME_UTC);

    GLint cameraTransformAttribute = glGetUniformLocation(renderer->ShaderProgram, "cameraTransform");
    GLint octreeAttribute = glGetUniformLocation(renderer->ShaderProgram, "octree");

    const GLuint workGroupSizeX = 16;
    const GLuint workGroupSizeY = 16;

    float averageDeltaTime = 0;
    u64 deltaTimeSampleCount = 0;

    // Disable v-sync
    glfwSwapInterval(1);

    while (!glfwWindowShouldClose(renderer->Window))
    {
        timespec_get(&currentTime, TIME_UTC);
        float deltaTime = (currentTime.tv_sec - prevTime.tv_sec) + (currentTime.tv_nsec - prevTime.tv_nsec) * 1e-9;
        timespec_get(&prevTime, TIME_UTC);

        //LogInfo("%f", 1.0f / deltaTime);

        averageDeltaTime = ((averageDeltaTime * deltaTimeSampleCount) + deltaTime) / (deltaTimeSampleCount + 1);
        deltaTimeSampleCount++;

        CameraProcessInput(&renderer->Camera, deltaTime);

        glClearColor(0.5f, 0.7f, 1.0f, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(renderer->ShaderProgram);

        glUniformMatrix4fv(cameraTransformAttribute, 1, GL_FALSE, renderer->Camera.Transform.raw[0]);
        glUniform1uiv(octreeAttribute, renderer->VoxelChunk.Octree.Length, renderer->VoxelChunk.Octree.Items);

        GLuint numGroupsX = (renderer->Texture.Width + workGroupSizeX - 1) / workGroupSizeX;
        GLuint numGroupsY = (renderer->Texture.Height + workGroupSizeY - 1) / workGroupSizeY;

        glBindImageTexture(0, renderer->Texture.Handle, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
        glDispatchCompute(numGroupsX, numGroupsY, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        BlitFramebufferToSwapchain(renderer->Framebuffer, &renderer->Texture);

        glfwSwapBuffers(renderer->Window);
        glfwPollEvents();
    }

    LogInfo("Average deltatime = %f ms", averageDeltaTime * 1000);
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
    free((char*)shaderSource);
    glCompileShader(voxelShader);

    glGetShaderiv(voxelShader, GL_COMPILE_STATUS, &shaderCompileStatus);
    if (shaderCompileStatus == false)
    {
        glGetShaderInfoLog(voxelShader, 512, NULL, statusLog);
        LogError("ComputeShader compilation failed: %s", statusLog);
    }

    return voxelShader;
}

void AttachTextureToFramebuffer(Renderer* renderer)
{
    glNamedFramebufferTexture(renderer->Framebuffer, GL_COLOR_ATTACHMENT0, renderer->Texture.Handle, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        LogError("Framebuffer is not complete.");
        glDeleteFramebuffers(1, &renderer->Framebuffer);
        exit(1);
    }

    renderer->ViewportDimensionsAttribute = glGetUniformLocation(renderer->ShaderProgram, "viewportDimensions");
    float viewportHeight = renderer->Camera.NearPlaneDistance * tanf(glm_rad(renderer->Camera.VerticalFov * 0.5f)) * 2.0f;
    float aspectRatio = ((float)renderer->Texture.Width / (float)renderer->Texture.Height);
    vec4 viewportDimensions = {viewportHeight * aspectRatio, viewportHeight, renderer->Camera.NearPlaneDistance, renderer->Camera.FarPlaneDistance};

    glUniform4fv(renderer->ViewportDimensionsAttribute, 1, (const GLfloat*)&viewportDimensions);
}

void BlitFramebufferToSwapchain(const GLuint framebuffer, const Texture* texture)
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    glBlitFramebuffer(0, 0, texture->Width, texture->Height, 0, 0, texture->Width, texture->Height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void GlfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Renderer* renderer = glfwGetWindowUserPointer(window);
    CameraRecieveInput(&renderer->Camera, key, action);

    if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

void GlfwWindowSizeCallback(GLFWwindow* window, int width, int height)
{
    Renderer* renderer = glfwGetWindowUserPointer(window);
    glDeleteTextures(1, &renderer->Texture.Handle);
    renderer->Texture = CreateTexture(width, height);
    AttachTextureToFramebuffer(renderer);
}
