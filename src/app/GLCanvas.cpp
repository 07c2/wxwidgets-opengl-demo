// src/app/GLCanvas.cpp
#include "GLCanvas.h"

#include <wx/dcclient.h>
#include <wx/log.h>

#include "Events.h"               // custom wx event declaration
#include "render/Renderer.h"      // rendering backend API

// Attribute list for the GL canvas (legacy style works across wx versions)
namespace {
    // Request RGBA, double buffer, 24-bit depth, 8-bit stencil.
    // Terminated by 0 as required by wxGLCanvas ctor.
    const int kGLAttribs[] = {
        WX_GL_RGBA,
        WX_GL_DOUBLEBUFFER,
        WX_GL_DEPTH_SIZE,   24,
        WX_GL_STENCIL_SIZE, 8,
        0
    };
}

GLCanvas::GLCanvas(wxWindow* parent, wxWindowID id)
    : wxGLCanvas(parent, id, kGLAttribs, wxDefaultPosition, wxDefaultSize,
                 wxFULL_REPAINT_ON_RESIZE | wxBORDER_NONE, "GLCanvas"),
      m_timer(this)
{
    // Reduce background erase to avoid flicker; we'll paint everything in GL.
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);

    // Create GL context asap so that size events can safely talk to GL when needed.
    CreateContextIfNeeded();

    // Bind events (modern style)
    Bind(wxEVT_PAINT,        &GLCanvas::OnPaint,       this);
    Bind(wxEVT_SIZE,         &GLCanvas::OnSize,        this);
    Bind(wxEVT_LEFT_DOWN,    &GLCanvas::OnLeftDown,    this);
    Bind(wxEVT_ERASE_BACKGROUND, &GLCanvas::OnEraseBackground, this);
}

GLCanvas::~GLCanvas()
{
    // Stop refresh timer first.
    if (m_timer.IsRunning())
        m_timer.Stop();

    // Destroy renderer before context goes away, to ensure GL resources are released.
    m_renderer.reset();
    m_context.reset();
}

void GLCanvas::CreateContextIfNeeded()
{
    if (m_context)
        return;

#if defined(__WXGTK__) && wxCHECK_VERSION(3,1,0)
    wxGLContextAttrs compat;
    compat.PlatformDefaults().OGLVersion(2, 1).EndList();
    m_context.reset(new wxGLContext(this, nullptr, &compat));
    if (!m_context) m_context.reset(new wxGLContext(this));
#elif wxCHECK_VERSION(3,1,0)
    wxGLContextAttrs compat;
    compat.PlatformDefaults().OGLVersion(2, 1).EndList(); // macOS 也走 2.1 没问题
    m_context.reset(new wxGLContext(this, nullptr, &compat));
    if (!m_context) m_context.reset(new wxGLContext(this));
#else
    m_context.reset(new wxGLContext(this));
#endif
}

void GLCanvas::EnsureCurrent()
{
    if (!m_context)
        CreateContextIfNeeded();
    // wxGLCanvas::SetCurrent has void return in modern wx; ignore return for portability.
    SetCurrent(*m_context);
}

float GLCanvas::GetDPIScale() const
{
#if wxCHECK_VERSION(3,1,0)
    return static_cast<float>(GetContentScaleFactor());
#else
    return 1.0f;
#endif
}

void GLCanvas::InitializeRendererIfNeeded()
{
    if (m_initialized)
        return;

    if (!m_renderer)
        m_renderer.reset(new Renderer());

    // Ensure a current GL context before touching GL in the renderer.
    EnsureCurrent();

    if (!m_renderer->Initialize()) {
        wxLogError("Renderer initialization failed.");
        return;
    }

    // Load the overlay button icon from resources.
    // The top-level CMake defines APP_RESOURCE_DIR pointing to <repo>/resources.
#ifdef APP_RESOURCE_DIR
    const std::string iconPath = std::string(APP_RESOURCE_DIR) + "/icons/toggle.png";
#else
    // Fallback to a relative path when APP_RESOURCE_DIR is not defined.
    const std::string iconPath = "resources/icons/toggle.png";
#endif
    (void)m_renderer->LoadOverlayIcon(iconPath);

    m_initialized = true;
}

void GLCanvas::ResizeRendererToClient()
{
    if (!m_renderer)
        return;

    const float scale = GetDPIScale();
    const wxSize sz   = GetClientSize();
    const int w_px    = static_cast<int>(sz.GetWidth()  * scale);
    const int h_px    = static_cast<int>(sz.GetHeight() * scale);

    EnsureCurrent();
    m_renderer->Resize(w_px, h_px, scale);
}

void GLCanvas::OnPaint(wxPaintEvent& /*evt*/)
{
    // Required by wx to validate the window for painting.
    wxPaintDC dc(this);

    EnsureCurrent();
    InitializeRendererIfNeeded();
    ResizeRendererToClient(); // protect against first paint before size event

    if (m_renderer) {
        m_renderer->Render();
    }

    // Present back buffer
    SwapBuffers();
}

void GLCanvas::OnSize(wxSizeEvent& evt)
{
    // Let the default handler process internal bookkeeping
    evt.Skip();

    if (!m_renderer)
        return;

    ResizeRendererToClient();

    // Trigger a redraw to apply the new viewport immediately.
    Refresh(false);
}

void GLCanvas::OnLeftDown(wxMouseEvent& evt)
{
    if (!m_renderer) {
        evt.Skip();
        return;
    }

    // Position is in logical (DIP) units; convert to device pixels for hit-testing.
    const float scale = GetDPIScale();
    const wxPoint p   = evt.GetPosition();
    const int x_px    = static_cast<int>(p.x * scale);
    const int y_px    = static_cast<int>(p.y * scale);

    if (m_renderer->HitTestOverlay(x_px, y_px, scale)) {
        wxCommandEvent e(wxEVT_WXGL_TOGGLE_SIDEBAR);
        e.SetEventObject(this);
#ifdef __WXGTK__
        GetParent()->GetEventHandler()->ProcessEvent(e); // 同步
#else
        wxPostEvent(GetParent(), e);
#endif
        return;
    }
}

void GLCanvas::OnTimer(wxTimerEvent& /*evt*/)
{
    // Lightweight periodic refresh. Consider pausing when unfocused if needed.
}

void GLCanvas::OnEraseBackground(wxEraseEvent& /*evt*/)
{
    // Intentionally empty: we draw everything with OpenGL.
}

void GLCanvas::SetRotation(float deg)
{
    if (m_renderer)
        m_renderer->SetRotation(deg);
}

void GLCanvas::SetScale(float s)
{
    if (m_renderer)
        m_renderer->SetScale(s);
}

void GLCanvas::SetObjectVisible(bool v)
{
    if (m_renderer)
        m_renderer->SetObjectVisible(v);
}

void GLCanvas::RequestRedraw()
{
    Refresh(false);
    Update();
}