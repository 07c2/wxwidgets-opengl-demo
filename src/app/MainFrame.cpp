// src/app/MainFrame.cpp
#include "MainFrame.h"
#include "GLCanvas.h"
#include "SidePanel.h"
#include "Events.h"

#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/wupdlock.h>

// Define the custom event declared in Events.h once in the program.
wxDEFINE_EVENT(wxEVT_WXGL_TOGGLE_SIDEBAR, wxCommandEvent);

MainFrame::MainFrame(wxWindow* parent, wxWindowID id, const wxString& title)
    : wxFrame(parent, id, title, wxDefaultPosition, wxDefaultSize,
              wxDEFAULT_FRAME_STYLE | wxCLIP_CHILDREN)
{
    SetMinClientSize(wxSize(800, 480));
    BuildUi();

    // Bind to the custom event fired by GLCanvas when overlay button is clicked.
    Bind(wxEVT_WXGL_TOGGLE_SIDEBAR, &MainFrame::OnToggleSidebar, this);
}

MainFrame::~MainFrame() = default;

void MainFrame::BuildUi()
{
    // Root horizontal layout: [ GLCanvas | SidePanel ]
    m_rootSizer = new wxBoxSizer(wxHORIZONTAL);

    // Left: OpenGL canvas (expands to fill)
    m_canvas = new GLCanvas(this, wxID_ANY);
    m_rootSizer->Add(m_canvas, 1, wxEXPAND);

    // Right: side panel (fixed width, native controls)
    m_side = new SidePanel(this, m_canvas);
    m_rootSizer->Add(m_side, 0, wxEXPAND | wxLEFT, 4);

    SetSizer(m_rootSizer);
    Layout();
}

void MainFrame::OnToggleSidebar(wxCommandEvent& /*evt*/)
{
    ToggleSidebar();
}

void MainFrame::ToggleSidebar()
{
    m_sideVisible = !m_sideVisible;
    if (m_side) m_side->Show(m_sideVisible);

    // Relayout to let GLCanvas occupy/release the space.
    if (m_rootSizer) {
        m_rootSizer->Layout();
    } else {
        Layout();
    }

    // Notify children about size change to ensure GL viewport is updated.
    SendSizeEvent();
    Update();
}

bool MainFrame::IsSidebarVisible() const
{
    return m_side && m_side->IsShown();
}