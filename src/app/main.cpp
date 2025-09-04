// src/app/main.cpp
// Application entry point for wxgl_overlay_demo.
// Creates the wxApp instance and shows the MainFrame.

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
#include <wx/image.h> // wxInitAllImageHandlers

#include "MainFrame.h"

class WxglApp final : public wxApp
{
public:
    bool OnInit() override;
    int  OnExit() override;
};

wxIMPLEMENT_APP(WxglApp);

bool WxglApp::OnInit()
{
    if (!wxApp::OnInit())
        return false;

    // Enable common image handlers (PNG, etc.) for any wx-side assets if used later.
    wxInitAllImageHandlers();

    SetAppName("wxgl_overlay_demo");
    SetVendorName("wxgl");

    try
    {
        // Construct and show the main frame.
        // MainFrame is responsible for creating the GL canvas and side panel.
        MainFrame* frame = new MainFrame(nullptr, wxID_ANY, "wxGL Overlay Demo");
        frame->SetClientSize(1024, 640);
        frame->CentreOnScreen();
        frame->Show(true);
        SetTopWindow(frame);
    }
    catch (const std::exception& e)
    {
        wxLogError("Failed to start application: %s", e.what());
        return false;
    }
    catch (...)
    {
        wxLogError("Failed to start application: unknown exception.");
        return false;
    }

    return true;
}

int WxglApp::OnExit()
{
    // Place for any global teardown if needed later.
    return wxApp::OnExit();
}