// src/app/GLCanvas.h
#pragma once

#include <memory>
#include <string>

#include <wx/glcanvas.h>
#include <wx/timer.h>

class Renderer; // from src/render/Renderer.h

/**
 * GLCanvas
 * - Derives from wxGLCanvas and owns a wxGLContext.
 * - Bridges wxWidgets events (paint/resize/mouse) to the rendering backend (Renderer).
 * - Does NOT expose any wxWidgets types to the render module.
 */
class GLCanvas final : public wxGLCanvas
{
public:
    explicit GLCanvas(wxWindow* parent, wxWindowID id);
    ~GLCanvas() override;

    // State setters used by SidePanel (UI → Render)
    void SetRotation(float deg);
    void SetScale(float s);
    void SetObjectVisible(bool v);

    // Manual redraw (optional external trigger)
    void RequestRedraw();

private:
    // Event handlers
    void OnPaint(wxPaintEvent& evt);
    void OnSize(wxSizeEvent& evt);
    void OnLeftDown(wxMouseEvent& evt);
    void OnTimer(wxTimerEvent& evt);
    void OnEraseBackground(wxEraseEvent& evt); // no-op to avoid flicker

    // Helpers
    void CreateContextIfNeeded();
    void EnsureCurrent();
    void InitializeRendererIfNeeded();
    float GetDPIScale() const;
    void ResizeRendererToClient();

private:
    std::unique_ptr<wxGLContext> m_context;
    std::unique_ptr<Renderer>    m_renderer;
    wxTimer                      m_timer;
    bool                         m_initialized {false}; // renderer initialization guard
};