// src/render/Renderer.cpp
#include "Renderer.h"

#include <iostream>
#include <algorithm>
#include <cmath>

#include "glad/glad.h"

#include "Scene.h"
#include "UIOverlay.h"

namespace {
// Simple clamp helper for C++14
template <typename T>
T clamp(T v, T lo, T hi) {
    return (v < lo) ? lo : (v > hi) ? hi : v;
}
} // namespace

Renderer::Renderer()  = default;
Renderer::~Renderer() = default;

bool Renderer::Initialize()
{
    if (m_initialized)
        return true;

    // Load OpenGL entry points (requires a current context).
    if (!gladLoadGL()) {
        return false;
    }
    const char* ver = reinterpret_cast<const char*>(glGetString(0x1F02)); // GL_VERSION
    const char* shv = reinterpret_cast<const char*>(glGetString(0x8B8C)); // GLSL
    std::cout << "OpenGL: " << (ver ? ver : "?")
          << " | GLSL: " << (shv ? shv : "?") << std::endl;

    ApplyDefaultGLState();

    // Create subsystems
    m_scene.reset(new Scene());
    m_overlay.reset(new UIOverlay());

    if (!m_scene->Initialize()) {
        return false;
    }
    if (!m_overlay->Initialize()) {
        return false;
    }

    // Propagate initial sizes if Resize was called earlier with defaults.
    m_scene->Resize(m_width, m_height, m_dpi);
    m_overlay->Resize(m_width, m_height, m_dpi);

    m_initialized = true;
    return true;
}

void Renderer::ApplyDefaultGLState()
{
    // 2D: no depth, enable alpha blending for textured UI
    glDisable(0x0B71 /* GL_DEPTH_TEST */);
    glDisable(0x0B44 /* GL_CULL_FACE */);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // A neutral dark background
    glClearColor(0.10f, 0.12f, 0.15f, 1.0f);
}

void Renderer::Resize(int width_px, int height_px, float dpi_scale)
{
    m_width  = std::max(1, width_px);
    m_height = std::max(1, height_px);
    m_dpi    = (dpi_scale > 0.0f) ? dpi_scale : 1.0f;

    glViewport(0, 0, m_width, m_height);

    if (m_scene)   m_scene->Resize(m_width, m_height, m_dpi);
    if (m_overlay) m_overlay->Resize(m_width, m_height, m_dpi);
}

void Renderer::Render()
{
    glClear(GL_COLOR_BUFFER_BIT);

    if (m_scene)   m_scene->Render(m_state);
    if (m_overlay) m_overlay->Render();
}

void Renderer::SetRotation(float deg)
{
    // Normalize angle for numeric stability in animations/shaders
    if (std::isfinite(deg)) {
        // Keep within [0, 360)
        float d = std::fmod(deg, 360.0f);
        if (d < 0.0f) d += 360.0f;
        m_state.rotation_deg = d;
    }
}

void Renderer::SetScale(float s)
{
    if (std::isfinite(s)) {
        m_state.scale = clamp(s, 0.1f, 8.0f);
    }
}

void Renderer::SetObjectVisible(bool v)
{
    m_state.object_visible = v;
}

bool Renderer::LoadOverlayIcon(const std::string& png_path)
{
    if (!m_overlay)
        m_overlay.reset(new UIOverlay());
    if (!m_initialized) {
        // The icon upload requires GL; caller ensures Initialize() first.
        // We still allow storing the path and lazy-loading inside UIOverlay if desired,
        // but here we just forward and return the immediate result.
    }
    return m_overlay->LoadIcon(png_path);
}

bool Renderer::HitTestOverlay(int x_px, int y_px, float /*dpi_scale*/) const
{
    return m_overlay ? m_overlay->HitTest(x_px, y_px) : false;
}