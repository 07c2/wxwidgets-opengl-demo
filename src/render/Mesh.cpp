// src/render/Mesh.cpp
#include "Mesh.h"

#include <utility>
#include "glad/glad.h"

Mesh::~Mesh()
{
    Reset();
}

Mesh::Mesh(Mesh&& other) noexcept
{
    moveSwap(other);
}

Mesh& Mesh::operator=(Mesh&& other) noexcept
{
    if (this != &other) {
        Reset();
        moveSwap(other);
    }
    return *this;
}

void Mesh::moveSwap(Mesh& rhs) noexcept
{
    std::swap(m_vbo, rhs.m_vbo);
    std::swap(m_vao, rhs.m_vao);
    m_attribs.swap(rhs.m_attribs);
}

bool Mesh::Create(const void* data,
                  std::size_t size,
                  std::initializer_list<Attrib> attribs,
                  unsigned usage)
{
    Reset();

    // Create VBO and upload data
    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    if (!vbo) return false;

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(size), data, usage);

    m_vbo = vbo;
    m_attribs.assign(attribs.begin(), attribs.end());

    // Try to set up a VAO; if unavailable, we'll bind attributes on each draw.
    if (!setupVAO()) {
        // Fallback path: nothing else to do here; attributes will be set at draw time.
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return true;
}

bool Mesh::UpdateBuffer(const void* data, std::size_t size, unsigned usage)
{
    if (!m_vbo) return false;
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(size), data, usage);

    if (m_vao) {
        // VAO already knows attribute layout; nothing else to do.
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return true;
}

bool Mesh::setupVAO()
{
    // VAO is not always available on very old GL. We check function pointers.
    if (!glGenVertexArrays || !glBindVertexArray || !glDeleteVertexArrays)
        return false;

    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    if (!vao) return false;

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    // Define attribute arrays once into the VAO state object
    for (const auto& a : m_attribs) {
        glEnableVertexAttribArray(a.index);
        glVertexAttribPointer(a.index, a.size, a.type,
                              a.normalized, a.stride,
                              reinterpret_cast<const void*>(a.offset));
    }

    // Unbind to keep state clean
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    m_vao = vao;
    return true;
}

void Mesh::enableAttributes() const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    for (const auto& a : m_attribs) {
        glEnableVertexAttribArray(a.index);
        glVertexAttribPointer(a.index, a.size, a.type,
                              a.normalized, a.stride,
                              reinterpret_cast<const void*>(a.offset));
    }
}

void Mesh::disableAttributes() const
{
    for (const auto& a : m_attribs) {
        glDisableVertexAttribArray(a.index);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::Draw(unsigned mode, int count) const
{
    if (!m_vbo || count <= 0) return;

    if (m_vao) {
        glBindVertexArray(m_vao);
        glDrawArrays(mode, 0, count);
        glBindVertexArray(0);
    } else {
        // Fallback: bind attributes every draw
        enableAttributes();
        glDrawArrays(mode, 0, count);
        disableAttributes();
    }
}

void Mesh::Reset()
{
    if (m_vao) {
        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }
    if (m_vbo) {
        glDeleteBuffers(1, &m_vbo);
        m_vbo = 0;
    }
    m_attribs.clear();
}