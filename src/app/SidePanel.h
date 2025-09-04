// src/app/SidePanel.h
#pragma once

#include <wx/panel.h>
#include <wx/slider.h>
#include <wx/checkbox.h>
#include <wx/stattext.h>

class GLCanvas; // forward declaration

/**
 * SidePanel
 * Right-side native controls panel.
 * - wxSlider: controls rotation (degrees 0..360)
 * - wxCheckBox: toggles object visibility
 *
 * Communicates with the GL canvas via its public setter methods.
 */
class SidePanel final : public wxPanel
{
public:
    SidePanel(wxWindow* parent, GLCanvas* canvas);
    ~SidePanel() override = default;

private:
    void BuildUi();
    void OnRotationChanged(wxCommandEvent& evt);
    void OnVisibilityToggled(wxCommandEvent& evt);

private:
    GLCanvas*     m_canvas   {nullptr};   // not owned
    wxSlider*     m_rotation {nullptr};
    wxCheckBox*   m_visible  {nullptr};
    wxStaticText* m_rotLabel {nullptr};
};