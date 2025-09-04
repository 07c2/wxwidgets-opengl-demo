// src/app/SidePanel.cpp
#include "SidePanel.h"
#include "GLCanvas.h"

#include <wx/sizer.h>
#include <wx/statline.h>

namespace {
    const int ROT_MIN  = 0;
    const int ROT_MAX  = 360;
    const int ROT_INIT = 0;
}

SidePanel::SidePanel(wxWindow* parent, GLCanvas* canvas)
    : wxPanel(parent, wxID_ANY), m_canvas(canvas)
{
    SetMinSize(wxSize(240, -1)); // fixed-ish width

    BuildUi();

    // Event bindings
    m_rotation->Bind(wxEVT_SLIDER,  &SidePanel::OnRotationChanged,   this);
    m_visible->Bind(wxEVT_CHECKBOX, &SidePanel::OnVisibilityToggled, this);
}

void SidePanel::BuildUi()
{
    auto* root = new wxBoxSizer(wxVERTICAL);

    // --- Rotation group ---
    auto* rotBox = new wxBoxSizer(wxVERTICAL);
    m_rotLabel = new wxStaticText(this, wxID_ANY, wxS("Rotation: 0°"));
    m_rotation = new wxSlider(this, wxID_ANY, ROT_INIT, ROT_MIN, ROT_MAX,
                              wxDefaultPosition, wxDefaultSize,
                              wxSL_HORIZONTAL | wxSL_AUTOTICKS | wxSL_LABELS);
#if wxCHECK_VERSION(3,1,0)
    m_rotation->SetTickFreq(30);
#endif

    rotBox->Add(m_rotLabel, 0, wxALL, 6);
    rotBox->Add(m_rotation, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 6);

    // Separator
    auto* sep = new wxStaticLine(this);

    // --- Visibility group ---
    auto* visBox = new wxBoxSizer(wxVERTICAL);
    m_visible = new wxCheckBox(this, wxID_ANY, "Show Object");
    m_visible->SetValue(true);

    visBox->Add(m_visible, 0, wxALL, 6);

    // Layout assembly
    root->Add(rotBox, 0, wxEXPAND | wxALL, 4);
    root->Add(sep,   0, wxEXPAND | wxLEFT | wxRIGHT, 4);
    root->Add(visBox,0, wxEXPAND | wxALL, 4);
    root->AddStretchSpacer(1);

    SetSizer(root);
    Layout();
}

void SidePanel::OnRotationChanged(wxCommandEvent& evt)
{
    const int deg = evt.GetInt();
    if (m_rotLabel) {
        m_rotLabel->SetLabel(wxString::Format(wxS("Rotation: %d°"), deg));
        m_rotLabel->Refresh();
        m_rotLabel->Update();
    }

    if (m_canvas) {
        m_canvas->SetRotation(static_cast<float>(deg));
        m_canvas->RequestRedraw();
    }

    this->Refresh();
    this->Update();
}

void SidePanel::OnVisibilityToggled(wxCommandEvent& evt)
{
    const bool visible = evt.IsChecked();
    if (m_canvas) {
        m_canvas->SetObjectVisible(visible);
        m_canvas->RequestRedraw();
    }
    if (m_visible) {
        m_visible->SetValue(visible);
        m_visible->Refresh();
        m_visible->Update();
    }

    this->Refresh();
    this->Update();
    if (wxWindow* top = wxGetTopLevelParent(this)) {
        top->Update();
    }
}