// src/render/Scene.cpp
#include "Scene.h"

#include <cmath>
#include <new>

#include "glad/glad.h"
#include "Shader.h"

// Vertex: 2D position + RGB color
struct VertexPC {
    float x, y;
    float r, g, b;
};

Scene::Scene() = default;

Scene::~Scene()
{
    if (m_vbo) {
        glDeleteBuffers(1, &m_vbo);
        m_vbo = 0;
    }
    delete m_shader;
    m_shader = nullptr;
}

bool Scene::Initialize()
{
    if (m_ready)
        return true;

    if (!BuildGeometry())
        return false;
    if (!BuildShader())
        return false;

    m_ready = true;
    return true;
}

void Scene::Resize(int width_px, int height_px, float dpi_scale)
{
    m_width  = (width_px  > 0) ? width_px  : 1;
    m_height = (height_px > 0) ? height_px : 1;
    m_dpi    = (dpi_scale > 0.f) ? dpi_scale : 1.f;

    (void)m_width; (void)m_height; (void)m_dpi; // reserved for future use
}

void Scene::Render(const RenderState& state)
{
    if (!m_ready || !m_shader)
        return;

    if (!state.object_visible)
        return;

    // Activate program
    m_shader->Use();
    const unsigned int prog = m_shader->Program();
    if (!prog)
        return;

    // Build a 2D rotation+scale matrix (column-major for GLSL)
    const float s = (state.scale > 0.f) ? state.scale : 1.f;
    const float rad = state.rotation_deg * 3.14159265358979323846f / 180.0f;
    const float c = std::cos(rad) * s;
    const float sn = std::sin(rad) * s;

    const float mvp[16] = {
        c,   sn,  0.f, 0.f,
       -sn,  c,   0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        0.f, 0.f, 0.f, 1.f
    };

    const int locMvp = glGetUniformLocation(prog, "uMVP");
    if (locMvp >= 0 && glad_glUniformMatrix4fv) {
        glUniformMatrix4fv(locMvp, 1, GL_FALSE, mvp);
    }

    // Bind VBO and set vertex attributes by queried locations
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    const int locPos   = glGetAttribLocation(prog, "aPos");
    const int locColor = glGetAttribLocation(prog, "aColor");
    if (locPos < 0 || locColor < 0) {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        return;
    }

    const GLsizei stride = static_cast<GLsizei>(sizeof(VertexPC));
    const GLvoid* posPtr = reinterpret_cast<const GLvoid*>(offsetof(VertexPC, x));
    const GLvoid* colPtr = reinterpret_cast<const GLvoid*>(offsetof(VertexPC, r));

    if (locPos >= 0 && glad_glEnableVertexAttribArray && glad_glVertexAttribPointer) {
        glEnableVertexAttribArray(static_cast<GLuint>(locPos));
        glVertexAttribPointer(static_cast<GLuint>(locPos), 2, GL_FLOAT, GL_FALSE, stride, posPtr);
    }
    if (locColor >= 0 && glad_glEnableVertexAttribArray && glad_glVertexAttribPointer) {
        glEnableVertexAttribArray(static_cast<GLuint>(locColor));
        glVertexAttribPointer(static_cast<GLuint>(locColor), 3, GL_FLOAT, GL_FALSE, stride, colPtr);
    }

    if (glad_glDrawArrays) {
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    if (locColor >= 0 && glad_glDisableVertexAttribArray) {
        glDisableVertexAttribArray(static_cast<GLuint>(locColor));
    }
    if (locPos >= 0 && glad_glDisableVertexAttribArray) {
        glDisableVertexAttribArray(static_cast<GLuint>(locPos));
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

bool Scene::BuildGeometry()
{
    // A simple isosceles triangle centered at the origin (NDC space)
    const VertexPC verts[3] = {
        { -0.5f, -0.5f,  0.95f, 0.4f, 0.3f },
        {  0.5f, -0.5f,  0.3f,  0.8f, 0.4f },
        {  0.0f,  0.6f,  0.2f,  0.5f, 0.95f }
    };

    glGenBuffers(1, &m_vbo);
    if (!m_vbo)
        return false;

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return (glGetError() == 0);
}

bool Scene::BuildShader()
{
    static const char* kVS =
        "#version 120\n"
        "attribute vec2 aPos;\n"
        "attribute vec3 aColor;\n"
        "uniform mat4 uMVP;\n"
        "varying vec3 vColor;\n"
        "void main() {\n"
        "  vColor = aColor;\n"
        "  gl_Position = uMVP * vec4(aPos, 0.0, 1.0);\n"
        "}\n";

    static const char* kFS =
        "#version 120\n"
        "varying vec3 vColor;\n"
        "void main() {\n"
        "  gl_FragColor = vec4(vColor, 1.0);\n"
        "}\n";

    m_shader = new (std::nothrow) Shader();
    if (!m_shader)
        return false;

    if (!m_shader->CompileFromSource(kVS, kFS, nullptr)) {
        delete m_shader;
        m_shader = nullptr;
        return false;
    }
    return true;
}