// src/render/Texture.cpp
#include "Texture.h"

#include <cassert>
#include <utility>

#include "glad/glad.h"

// Limit stb_image to PNG to keep binary small; remove if you need more formats.
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include <stb_image.h>

Texture::~Texture()
{
    Reset();
}

Texture::Texture(Texture&& other) noexcept
{
    swap(other);
}

Texture& Texture::operator=(Texture&& other) noexcept
{
    if (this != &other) {
        Reset();
        swap(other);
    }
    return *this;
}

void Texture::swap(Texture& rhs) noexcept
{
    std::swap(m_id, rhs.m_id);
    std::swap(m_w,  rhs.m_w);
    std::swap(m_h,  rhs.m_h);
}

void Texture::Reset()
{
    if (m_id) {
        glDeleteTextures(1, &m_id);
        m_id = 0;
    }
    m_w = m_h = 0;
}

bool Texture::LoadFromFile(const std::string& path, bool flipY)
{
    // Release any previous texture.
    Reset();

    // Decode with stb_image (force RGBA to simplify GL upload and alignment).
    stbi_set_flip_vertically_on_load(flipY ? 1 : 0);

    int w = 0, h = 0, comp = 0;
    unsigned char* pixels = stbi_load(path.c_str(), &w, &h, &comp, 4);
    if (!pixels || w <= 0 || h <= 0) {
        if (pixels) stbi_image_free(pixels);
        return false;
    }

    // Create and upload GL texture
    GLuint tex = 0;
    glGenTextures(1, &tex);
    if (!tex) {
        stbi_image_free(pixels);
        return false;
    }

    glBindTexture(GL_TEXTURE_2D, tex);
    // No mipmaps (icon-sized); use linear filtering and clamp-to-edge.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,    GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,    GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D,
                 0,                // level
                 GL_RGBA,          // internal format
                 w, h,
                 0,                // border
                 GL_RGBA,          // data format
                 GL_UNSIGNED_BYTE, // data type
                 pixels);

    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(pixels);

    m_id = tex;
    m_w  = w;
    m_h  = h;
    return true;
}

void Texture::Bind(unsigned target) const
{
    glBindTexture(target, m_id);
}