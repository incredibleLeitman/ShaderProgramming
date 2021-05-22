#pragma once
#ifndef TEXTURE_LOADER_H
#define TEXTURE_LOADER_H

#include <array>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

unsigned int loadTexture (char const *path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    //std::cout << "path: " << path << " textureID: " << textureID << std::endl;

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
    }
    stbi_image_free(data);

    return textureID;
}

std::array<int, 3> loadTextureBatch (char const* path)
{
    std::array<int, 3> batch;
    for (int i = 0; i < 3; ++i)
    {
        std::string base = path;
        if (i == 1) base += "_normal";
        else if (i == 2) base += "_disp";

        batch[i] = loadTexture((base + ".jpg").c_str());
    }
    return batch;
}
#endif // TEXTURE_LOADER_H