// src/render/Quad.cpp
#include "Quad.h"

#include "glad/glad.h"

bool Quad::CreatePosUV(unsigned posIndex, unsigned uvIndex)
{
    // Interleaved: [pos01.x, pos01.y, uv.x, uv.y] for 4 vertices
    const float verts[] = {
        // x    y    u    v
         0.0f, 0.0f, 0.0f, 0.0f, // top-left
         1.0f, 0.0f, 1.0f, 0.0f, // top-right
         1.0f, 1.0f, 1.0f, 1.0f, // bottom-right
         0.0f, 1.0f, 0.0f, 1.0f, // bottom-left
    };

    const int stride = static_cast<int>(sizeof(float) * 4);
    const Mesh::Attrib aPos {
        posIndex, /*size*/2, GL_FLOAT, GL_FALSE, stride, /*offset*/0
    };
    const Mesh::Attrib aUV  {
        uvIndex,  /*size*/2, GL_FLOAT, GL_FALSE, stride, /*offset*/sizeof(float)*2
    };

    const bool ok = m_mesh.Create(
        verts, sizeof(verts),
        { aPos, aUV },
        GL_STATIC_DRAW
    );
    if (ok) {
        m_count = 4;
    } else {
        m_count = 0;
    }
    return ok;
}

void Quad::Draw() const
{
    if (m_count <= 0) return;
    m_mesh.Draw(GL_TRIANGLE_FAN, m_count);
}

void Quad::Reset()
{
    m_mesh.Reset();
    m_count = 0;
}