// src/app/Events.h
#pragma once

#include <wx/event.h>

/**
 * Custom wxWidgets events for this project.
 *
 * Example:
 *   // In MainFrame.cpp (once):
 *   wxDEFINE_EVENT(wxEVT_WXGL_TOGGLE_SIDEBAR, wxCommandEvent);
 *
 *   // In GLCanvas.cpp:
 *   wxCommandEvent e(wxEVT_WXGL_TOGGLE_SIDEBAR);
 *   wxPostEvent(GetParent(), e);
 *
 *   // In MainFrame ctor:
 *   Bind(wxEVT_WXGL_TOGGLE_SIDEBAR, &MainFrame::OnToggleSidebar, this);
 */

// Declare a new event type identifier.
// Definition must appear once in a .cpp file (we use MainFrame.cpp).
wxDECLARE_EVENT(wxEVT_WXGL_TOGGLE_SIDEBAR, wxCommandEvent);