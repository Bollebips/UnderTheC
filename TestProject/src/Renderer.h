#ifndef RENDERER_H
#define RENDERER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

typedef struct Renderer
{
    GLFWwindow* Window;
} Renderer;

int RendererInit(Renderer* renderer);
void Render(Renderer* renderer);
void RendererCleanup(Renderer* renderer);

#endif // RENDERER_H
