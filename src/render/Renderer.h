// src/render/Renderer.h
#pragma once

#include <memory>
#include <string>

#include "RenderState.h"

// Forward declarations to keep rendering core decoupled at interface level.
class Scene;
class UIOverlay;

/**
 * Renderer
 * Core rendering orchestrator (no dependency on wxWidgets).
 *
 * Lifecycle:
 *   - Initialize()    : load GL functions, set GL state, create subcomponents
 *   - Resize(w,h,dpi) : update viewport and subcomponents
 *   - Render()        : draw scene + overlay
 *
 * UI -> Render state:
 *   SetRotation / SetScale / SetObjectVisible
 *
 * Overlay:
 *   LoadOverlayIcon() : load PNG into GL texture for the overlay button
 *   HitTestOverlay()  : pixel-space hit test for the overlay button
 */
class Renderer final
{
public:
    Renderer();
    ~Renderer();

    // Must be called with a current OpenGL context.
    bool Initialize();

    // Dimensions are in device pixels (after DPI scaling).
    void Resize(int width_px, int height_px, float dpi_scale);

    // Draw frame (scene first, then overlay).
    void Render();

    // UI-controlled parameters
    void SetRotation(float deg);
    void SetScale(float s);
    void SetObjectVisible(bool v);

    // Overlay interaction
    bool LoadOverlayIcon(const std::string& png_path);
    bool HitTestOverlay(int x_px, int y_px, float dpi_scale) const;

private:
    // Helpers
    void ApplyDefaultGLState();

private:
    // Backing state shared with Scene
    RenderState m_state;

    // Backbuffer/viewport info
    int   m_width  {1};
    int   m_height {1};
    float m_dpi    {1.0f};

    // Subsystems
    std::unique_ptr<Scene>     m_scene;
    std::unique_ptr<UIOverlay> m_overlay;

    bool m_initialized {false};
};