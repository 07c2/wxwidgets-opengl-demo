// src/render/Mesh.h
#pragma once

#include <initializer_list>
#include <vector>
#include <cstddef>

/**
 * Mesh
 * Minimal RAII wrapper for a non-indexed triangle mesh (VBO + optional VAO).
 *
 * Goals:
 * - Keep the demo code clean when setting up interleaved vertex data.
 * - Be tolerant to platforms lacking core VAO (GL 2.1 without extension):
 *   if VAO functions are unavailable, Mesh will rebind attributes on each draw.
 *
 * Usage example:
 *   Mesh::Attrib aPos  { /*index* /0, /*size* /2, /*type* /GL_FLOAT, GL_FALSE, /*stride* /sizeof(Vertex), /*offset* /offsetof(Vertex, pos) };
 *   Mesh::Attrib aColor{ 1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, color) };
 *   Mesh m;
 *   m.Create(vertexData, sizeof(vertexData), {aPos, aColor});
 *   m.Draw(GL_TRIANGLES, 3);
 */
class Mesh
{
public:
    struct Attrib {
        unsigned index;   // attribute location (layout index)
        int      size;    // components (1..4)
        unsigned type;    // e.g., GL_FLOAT
        unsigned char normalized; // GL_TRUE/GL_FALSE
        int      stride;  // byte stride of a single vertex
        std::size_t offset; // byte offset within the vertex
    };

    Mesh() = default;
    ~Mesh();

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    /**
     * Create the mesh from interleaved vertex data.
     * @param data   pointer to vertex bytes
     * @param size   size in bytes
     * @param attribs attribute descriptions describing the interleaved layout
     * @param usage  GL usage hint (e.g., GL_STATIC_DRAW)
     * @return true on success
     */
    bool Create(const void* data,
                std::size_t size,
                std::initializer_list<Attrib> attribs,
                unsigned usage /* e.g., GL_STATIC_DRAW */);

    /**
     * Update the vertex buffer contents (size can grow).
     */
    bool UpdateBuffer(const void* data, std::size_t size, unsigned usage);

    /**
     * Draw the mesh as non-indexed primitives.
     * @param mode  GL primitive (e.g., GL_TRIANGLES)
     * @param count number of vertices to draw
     */
    void Draw(unsigned mode, int count) const;

    /**
     * Release GL resources.
     */
    void Reset();

    // Accessors
    unsigned vbo() const { return m_vbo; }
    unsigned vao() const { return m_vao; } // might be 0 if not supported

private:
    void moveSwap(Mesh& rhs) noexcept;
    bool setupVAO();                    // tries to create VAO if available
    void enableAttributes() const;      // fallback path (no VAO)
    void disableAttributes() const;     // fallback path (no VAO)

private:
    unsigned m_vbo {0};
    unsigned m_vao {0};                 // 0 => not used/available
    std::vector<Attrib> m_attribs;      // cached for fallback attribute binding
};