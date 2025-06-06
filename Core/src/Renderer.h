#ifndef RENDERER_H
#define RENDERER_H

/* #include <GL/glew.h> */
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "Texture.h"
#include "Camera.h"

typedef struct Renderer
{
    Camera Camera;
    GLFWwindow* Window;
    Texture Texture;
    GLuint ShaderProgram;
    GLuint Framebuffer;
    GLint ViewportDimensionsAttribute;
} Renderer;

bool RendererInit(Renderer* renderer);
void Render(Renderer* renderer);
void RendererCleanup(Renderer* renderer);
const GLuint CreateFramebufferWithTexture(const Texture* texture);
void BlitFramebufferToSwapchain(const GLuint framebuffer, const Texture* texture);

static void AttachTextureToFramebuffer(Renderer* renderer);
static GLuint CreateComputeShader(const char*);
static void GlfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void GlfwWindowSizeCallback(GLFWwindow* window, int width, int height);

#endif // RENDERER_H
