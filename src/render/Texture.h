// src/render/Texture.h
#pragma once

#include <string>

/**
 * Texture
 * Minimal RAII wrapper over an OpenGL 2D texture for PNG icons.
 * - LoadFromFile(path, flipY): decodes image (via stb_image) and uploads RGBA8
 * - Bind(target): binds texture to given target (e.g. GL_TEXTURE_2D)
 *
 * Notes:
 * - Requires a current GL context when loading or destroying.
 * - Copy is disabled; move is supported.
 */
class Texture
{
public:
    Texture() = default;
    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;

    // Decode PNG and upload to GL as RGBA8. Returns true on success.
    bool LoadFromFile(const std::string& path, bool flipY);

    // Bind to a GL target (pass GL_TEXTURE_2D).
    void Bind(unsigned target) const;

    // Release the GL texture, if any.
    void Reset();

    // Accessors
    unsigned id() const { return m_id; }
    int width()  const { return m_w; }
    int height() const { return m_h; }
    bool valid() const { return m_id != 0; }

private:
    void swap(Texture& rhs) noexcept;

private:
    unsigned m_id {0};
    int      m_w  {0};
    int      m_h  {0};
};