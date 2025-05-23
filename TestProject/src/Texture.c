#include "Texture.h"

Texture CreateTexture(int width, int height)
{
    Texture result;
    result.Width = width;
    result.Height = height;

    glCreateTextures(GL_TEXTURE_2D, 1, &result.Handle);
    glTextureStorage2D(result.Handle, 1, GL_RGBA32F, width, height);

    glTextureParameteri(result.Handle, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(result.Handle, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTextureParameteri(result.Handle, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(result.Handle, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    return result;
}
