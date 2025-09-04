// src/render/Quad.h
#pragma once

#include "Mesh.h"

/**
 * Quad
 * A tiny helper that provides a reusable unit quad mesh in 0..1 space
 * with interleaved attributes. Intended for screen-space/UI rendering,
 * but can be used anywhere a unit quad is handy.
 *
 * - CreatePosUV(posIndex, uvIndex):
 *     Builds a 4-vertex quad with attributes:
 *       aPos01 (2 floats) at 'posIndex'  — position in 0..1 range
 *       aUV    (2 floats) at 'uvIndex'   — texture coordinates (0..1)
 *   The caller's shader must use the same attribute locations.
 *
 * - Draw():
 *     Issues a non-indexed draw call as GL_TRIANGLE_FAN with 4 vertices.
 *
 * Notes:
 * - Uses Mesh to manage VBO/VAO (if available). Works on GL 2.1-era drivers.
 */
class Quad
{
public:
    Quad() = default;
    ~Quad() = default;

    Quad(const Quad&) = delete;
    Quad& operator=(const Quad&) = delete;

    Quad(Quad&&) noexcept = default;
    Quad& operator=(Quad&&) noexcept = default;

    // Build a unit quad with position(0..1) + UV(0..1)
    bool CreatePosUV(unsigned posIndex, unsigned uvIndex);

    // Draw as a triangle fan (4 vertices)
    void Draw() const;

    // Release GL resources
    void Reset();

private:
    Mesh m_mesh;
    int  m_count {0};
};