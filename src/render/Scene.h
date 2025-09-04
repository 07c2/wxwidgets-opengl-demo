// src/render/Scene.h
#pragma once

#include "RenderState.h"

#include <cstddef>

class Shader;

/**
 * Scene
 * A minimal 2D demo scene rendering a single colored triangle in NDC.
 * - Applies rotation (degrees) and uniform scale from RenderState.
 * - Skips drawing when object_visible == false.
 *
 * No dependency on wxWidgets. Uses raw OpenGL via the loader.
 */
class Scene
{
public:
    Scene();
    ~Scene();

    // Initialize GL resources (VBO, shader). Requires a current GL context.
    bool Initialize();

    // Viewport/DPI are kept for possible future use (e.g., pixel-space UI).
    void Resize(int width_px, int height_px, float dpi_scale);

    // Render the scene according to the provided state.
    void Render(const RenderState& state);

private:
    bool BuildGeometry();
    bool BuildShader();

private:
    unsigned int m_vbo {0};     // GL buffer for vertex data
    Shader*      m_shader {nullptr}; // owned; created during Initialize()

    int   m_width  {1};
    int   m_height {1};
    float m_dpi    {1.0f};

    bool  m_ready  {false};
};