// src/render/UIOverlay.h
#pragma once

#include <string>

class Shader;
class Texture;

/**
 * UIOverlay
 * A simple screen-space textured button rendered directly with OpenGL.
 * - Layout is in device pixels, DPI-aware (anchored at bottom-right).
 * - HitTest() uses the same pixel coordinate system as the canvas mouse events
 *   (origin at top-left, Y increases downward).
 *
 * No dependency on wxWidgets; the owner (Renderer) forwards input and sizing.
 */
class UIOverlay
{
public:
    UIOverlay();
    ~UIOverlay();

    // GL initialization (shaders, geometry). Requires a current GL context.
    bool Initialize();

    // Update viewport (device pixels) and DPI scale.
    void Resize(int width_px, int height_px, float dpi_scale);

    // Draw the overlay (if icon and shader are ready).
    void Render();

    // Load PNG icon into an OpenGL texture.
    bool LoadIcon(const std::string& png_path);

    // Return true if (x_px, y_px) hits the button (pixel coords, top-left origin).
    bool HitTest(int x_px, int y_px) const;

private:
    bool BuildGeometry();     // unit quad VBO (0..1) with UVs
    bool BuildShader();       // textured quad shader
    void UpdateLayout();      // compute button rect in pixels
    void UpdateOrtho();       // compute NDC matrix from pixel coords

private:
    // Viewport & DPI
    int   m_width  {1};
    int   m_height {1};
    float m_dpi    {1.0f};

    // Button metrics in Device-Independent Pixels (DIP)
    int   m_btnSizeDip {64};  // base size
    int   m_padDip     {12};  // margin from edges

    // Derived pixel-space rect for the button (top-left origin)
    struct Rect { int x, y, w, h; } m_btnPx {0,0,0,0};

    // GL resources
    unsigned int m_vbo {0};   // unit quad: pos(2) + uv(2), 4 vertices (TRIANGLE_FAN)
    Shader*  m_shader {nullptr};  // owned
    Texture* m_icon   {nullptr};  // owned

    // Cached orthographic transform (pixel -> clip space), column-major
    float m_ortho[16] = {
         2.f,  0.f, 0.f, 0.f,
         0.f, -2.f, 0.f, 0.f,
         0.f,  0.f, 1.f, 0.f,
        -1.f,  1.f, 0.f, 1.f
    };

    bool m_ready {false};
};