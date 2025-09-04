// src/app/MainFrame.h
#pragma once

#include <wx/frame.h>
#include <wx/sizer.h>

class GLCanvas;   // forward declaration
class SidePanel;  // forward declaration

class MainFrame final : public wxFrame
{
public:
    MainFrame(wxWindow* parent, wxWindowID id, const wxString& title);
    ~MainFrame() override;

    void ToggleSidebar();
    bool IsSidebarVisible() const;

private:
    void OnToggleSidebar(wxCommandEvent& evt);
    void BuildUi();

private:
    wxBoxSizer* m_rootSizer {nullptr};
    GLCanvas*   m_canvas    {nullptr};
    SidePanel*  m_side      {nullptr};
    bool        m_sideVisible {true};
};