#include "TextureUnit.hpp"
#include <SDL2/SDL_image.h>
#include <iostream>

TTexture::TTexture() : textureID(0) {}

TTexture::~TTexture() {
    if (textureID != 0) glDeleteTextures(1, &textureID);
}

bool TTexture::LoadFrom(const std::string& filename) {
    SDL_Surface* surface = IMG_Load(filename.c_str());
    if (!surface) {
        std::cerr << "Failed to load texture: " << filename << std::endl;
        return false;
    }

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    GLenum format = GL_RGB;
    if (surface->format->BytesPerPixel == 4) format = GL_RGBA;

    glTexImage2D(GL_TEXTURE_2D, 0, format, surface->w, surface->h, 0, format, GL_UNSIGNED_BYTE, surface->pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    SDL_FreeSurface(surface);
    return true;
}

void TTexture::bind() {
    glBindTexture(GL_TEXTURE_2D, textureID);
}