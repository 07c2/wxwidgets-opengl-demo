// src/render/UIOverlay.cpp
#include "UIOverlay.h"

#include <cmath>
#include <new>

#include "glad/glad.h"
#include "Shader.h"
#include "Texture.h"

UIOverlay::UIOverlay() = default;

UIOverlay::~UIOverlay()
{
    if (m_vbo) {
        glDeleteBuffers(1, &m_vbo);
        m_vbo = 0;
    }
    delete m_shader; m_shader = nullptr;
    delete m_icon;   m_icon   = nullptr;
}

bool UIOverlay::Initialize()
{
    if (m_ready)
        return true;

    if (!BuildGeometry())
        return false;

    if (!BuildShader())
        return false;

    UpdateLayout();
    UpdateOrtho();

    m_ready = true;
    return true;
}

void UIOverlay::Resize(int width_px, int height_px, float dpi_scale)
{
    m_width  = (width_px  > 0) ? width_px  : 1;
    m_height = (height_px > 0) ? height_px : 1;
    m_dpi    = (dpi_scale > 0.f) ? dpi_scale : 1.f;

    UpdateLayout();
    UpdateOrtho();
}

void UIOverlay::Render()
{
    if (!m_ready || !m_shader || !m_icon)
        return;

    m_shader->Use();
    const unsigned int prog = m_shader->Program();

    // Set uniforms: ortho, rect position & size in pixels, texture unit 0
    const int locOrtho = glGetUniformLocation(prog, "uOrtho");
    if (locOrtho >= 0) {
        glUniformMatrix4fv(locOrtho, 1, GL_FALSE, m_ortho);
    }
    const int locPos = glGetUniformLocation(prog, "uPosPx");
    const int locSize = glGetUniformLocation(prog, "uSizePx");
    if (locPos >= 0) {
        glUniform4f(locPos,
            static_cast<float>(m_btnPx.x),
            static_cast<float>(m_btnPx.y),
            0.f, 0.f);
    }
    if (locSize >= 0) {
        glUniform4f(locSize,
            static_cast<float>(m_btnPx.w),
            static_cast<float>(m_btnPx.h),
            0.f, 0.f);
    }

    const int locTex = glGetUniformLocation(prog, "uTex");
    if (locTex >= 0) {
        glUniform1i(locTex, 0);
    }

    glActiveTexture(GL_TEXTURE0);
    m_icon->Bind(GL_TEXTURE_2D);

    // Vertex attributes
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    const int locPosA = glGetAttribLocation(prog, "aPos01");
    const int locUvA  = glGetAttribLocation(prog, "aUV");
    const GLsizei stride = static_cast<GLsizei>(sizeof(float) * 4);
    const void*   posPtr = reinterpret_cast<const void*>(0);
    const void*   uvPtr  = reinterpret_cast<const void*>(sizeof(float) * 2);

    if (locPosA >= 0) {
        glEnableVertexAttribArray(static_cast<GLuint>(locPosA));
        glVertexAttribPointer(static_cast<GLuint>(locPosA), 2, GL_FLOAT, GL_FALSE, stride, posPtr);
    }
    if (locUvA >= 0) {
        glEnableVertexAttribArray(static_cast<GLuint>(locUvA));
        glVertexAttribPointer(static_cast<GLuint>(locUvA), 2, GL_FLOAT, GL_FALSE, stride, uvPtr);
    }

    // Ensure blending for alpha PNGs
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    if (locUvA  >= 0) glDisableVertexAttribArray(static_cast<GLuint>(locUvA));
    if (locPosA >= 0) glDisableVertexAttribArray(static_cast<GLuint>(locPosA));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

bool UIOverlay::LoadIcon(const std::string& png_path)
{
    if (!m_icon) {
        m_icon = new (std::nothrow) Texture();
        if (!m_icon) return false;
    }
    // Flip vertically so that (0,0) UV is top-left in image files.
    return m_icon->LoadFromFile(png_path, /*flipY=*/true);
}

bool UIOverlay::HitTest(int x_px, int y_px) const
{
    const int x0 = m_btnPx.x;
    const int y0 = m_btnPx.y;
    const int x1 = x0 + m_btnPx.w;
    const int y1 = y0 + m_btnPx.h;
    return (x_px >= x0 && x_px < x1 && y_px >= y0 && y_px < y1);
}

bool UIOverlay::BuildGeometry()
{
    // Interleaved: [pos01.x, pos01.y, uv.x, uv.y] for 4 vertices
    // Unit quad in 0..1 (to be scaled/translated in shader by uPosPx/uSizePx)
    const float quad[4 * 4] = {
        // x  y   u  v
         0.f, 0.f, 0.f, 0.f, // top-left
         1.f, 0.f, 1.f, 0.f, // top-right
         1.f, 1.f, 1.f, 1.f, // bottom-right
         0.f, 1.f, 0.f, 1.f, // bottom-left
    };

    glGenBuffers(1, &m_vbo);
    if (!m_vbo) return false;

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return (glGetError() == 0);
}

bool UIOverlay::BuildShader()
{
    // #version 120 for broad compatibility (OpenGL 2.1-era)
    static const char* kVS =
        "#version 120\n"
        "attribute vec2 aPos01;\n"
        "attribute vec2 aUV;\n"
        "uniform mat4 uOrtho;\n"
        "uniform vec4 uPosPx;  // x,y in pixels (top-left)\n"
        "uniform vec4 uSizePx; // w,h in pixels\n"
        "varying vec2 vUV;\n"
        "void main() {\n"
        "  vec2 posPx = uPosPx.xy + aPos01 * uSizePx.xy;\n"
        "  vec4 posClip = uOrtho * vec4(posPx.xy, 0.0, 1.0);\n"
        "  gl_Position = posClip;\n"
        "  vUV = aUV;\n"
        "}\n";

    static const char* kFS =
        "#version 120\n"
        "uniform sampler2D uTex;\n"
        "varying vec2 vUV;\n"
        "void main() {\n"
        "  vec4 tex = texture2D(uTex, vUV);\n"
        "  gl_FragColor = tex;\n"
        "}\n";

    m_shader = new (std::nothrow) Shader();
    if (!m_shader) return false;

    if (!m_shader->CompileFromSource(kVS, kFS, nullptr)) {
        delete m_shader; m_shader = nullptr;
        return false;
    }
    return true;
}

void UIOverlay::UpdateLayout()
{
    const int sizePx = static_cast<int>(std::floor(m_btnSizeDip * m_dpi + 0.5f));
    const int padPx  = static_cast<int>(std::floor(m_padDip     * m_dpi + 0.5f));

    const int w = sizePx;
    const int h = sizePx;

    const int x = m_width  - padPx - w;
    const int y = m_height - padPx - h;

    m_btnPx = { x < 0 ? 0 : x, y < 0 ? 0 : y, w, h };
}

void UIOverlay::UpdateOrtho()
{
    // Column-major orthographic transform mapping pixel coords to clip space:
    // x: [0..w] -> [-1..+1], y: [0..h] -> [+1..-1] (top-left origin in pixels)
    const float w = static_cast<float>(m_width  > 0 ? m_width  : 1);
    const float h = static_cast<float>(m_height > 0 ? m_height : 1);

    m_ortho[0]  =  2.f / w;  m_ortho[4]  =  0.f;      m_ortho[8]  = 0.f; m_ortho[12] = -1.f;
    m_ortho[1]  =  0.f;      m_ortho[5]  = -2.f / h;  m_ortho[9]  = 0.f; m_ortho[13] =  1.f;
    m_ortho[2]  =  0.f;      m_ortho[6]  =  0.f;      m_ortho[10] = 1.f; m_ortho[14] =  0.f;
    m_ortho[3]  =  0.f;      m_ortho[7]  =  0.f;      m_ortho[11] = 0.f; m_ortho[15] =  1.f;
}