#ifndef RENDERER_H
#define RENDERER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Texture.h"

typedef struct Renderer
{
    GLFWwindow* Window;
    GLuint ShaderProgram;
} Renderer;

int RendererInit(Renderer* renderer);
void Render(Renderer* renderer);
void RendererCleanup(Renderer* renderer);

#endif // RENDERER_H
