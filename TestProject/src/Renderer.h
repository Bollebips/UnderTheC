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
} Renderer;

int RendererInit(Renderer* renderer);
void Render(Renderer* renderer);
void RendererCleanup(Renderer* renderer);
GLuint CreateFramebufferWithTexture(const Texture* texture);
void BlitFramebufferToSwapchain(const GLuint framebuffer, const Texture* texture);

static bool TryAttachTextureToFramebuffer(const GLuint framebufferHandle, const Texture* texture);

#endif // RENDERER_H
