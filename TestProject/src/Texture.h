#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

typedef struct Texture
{
    GLuint Handle;
    uint32_t Width;
    uint32_t Height;
} Texture;

Texture CreateTexture(int width, int height);

#endif // TEXTURE_H
