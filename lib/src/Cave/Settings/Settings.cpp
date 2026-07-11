#include "Cave/Settings/Settings.h"
#include <wx/wx.h>
#include <wx/spinctrl.h>
#include <algorithm>

namespace {

void ensureWxInit()
{
    if (wxTheApp) return;
    static wxApp s_app;
    wxApp::SetInstance(&s_app);
    int argc = 0;
    wxEntryStart(argc, static_cast<wchar_t**>(nullptr));
}

class SettingsDialog : public wxDialog
{
public:
    SettingsDialog(Cave::Settings& settings, bool developerMode)
        : wxDialog(nullptr, wxID_ANY, "Settings",
                   wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE)
        , m_settings(settings)
    {
        auto* outer = new wxBoxSizer(wxVERTICAL);
        auto* grid  = new wxFlexGridSizer(2, 2, 8, 12);

        grid->Add(new wxStaticText(this, wxID_ANY, "Random dist. blocks:"),
                  0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
        m_spinRandom = new wxSpinCtrl(this, wxID_ANY, wxEmptyString,
                                      wxDefaultPosition, wxDefaultSize);
        m_spinRandom->SetRange((int)RANDOM_DIST_BLOCKS_MIN, (int)RANDOM_DIST_BLOCKS_MAX);
        m_spinRandom->SetValue((int)settings.randomDistBlocks);
        grid->Add(m_spinRandom, 0, wxALIGN_CENTER_VERTICAL);

        grid->Add(new wxStaticText(this, wxID_ANY, "Animation:"),
                  0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
        m_checkAnim = new wxCheckBox(this, wxID_ANY, "");
        m_checkAnim->SetValue(settings.animation);
        grid->Add(m_checkAnim, 0, wxALIGN_CENTER_VERTICAL);

        outer->Add(grid, 0, wxALL | wxEXPAND, 14);

        // ── Resize anchor (developer mode only) ──────────────────────────────
        if (developerMode) {
            auto* box = new wxStaticBoxSizer(wxVERTICAL, this, "Resize anchor");
            wxWindow* p = box->GetStaticBox();

            static const char* labels[3][3] = {
                { "Top Left",    "Top",    "Top Right"    },
                { "Left",        "Centre", "Right"        },
                { "Bottom Left", "Bottom", "Bottom Right" }
            };
            auto* grid3 = new wxGridSizer(3, 3, 2, 2);
            bool first = true;
            int currentAnchor = (int)settings.resizeAnchor;
            for (int r = 0; r < 3; ++r) {
                for (int c = 0; c < 3; ++c) {
                    int idx = r * 3 + c;
                    long style = first ? wxRB_GROUP : 0;
                    auto* rb = new wxRadioButton(p, wxID_ANY, labels[r][c],
                                                 wxDefaultPosition, wxDefaultSize, style);
                    rb->SetValue(idx == currentAnchor);
                    m_anchorBtns[idx] = rb;
                    grid3->Add(rb, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT);
                    first = false;
                }
            }
            box->Add(grid3, 0, wxALL, 10);
            outer->Add(box, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 8);

            // ── Fill mode ────────────────────────────────────────────────────
            auto* fillBox = new wxStaticBoxSizer(wxHORIZONTAL, this, "Fill");
            wxWindow* fp = fillBox->GetStaticBox();
            m_fillSquare  = new wxRadioButton(fp, wxID_ANY, "Rectangle",
                                              wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
            m_fillLine    = new wxRadioButton(fp, wxID_ANY, "Line");
            m_fillEllipse = new wxRadioButton(fp, wxID_ANY, "Ellipse");
            m_fillSquare ->SetValue(settings.fillMode == Cave::FillMode::Rectangle);
            m_fillLine   ->SetValue(settings.fillMode == Cave::FillMode::Line);
            m_fillEllipse->SetValue(settings.fillMode == Cave::FillMode::Ellipse);
            fillBox->Add(m_fillSquare,  0, wxALIGN_CENTER_VERTICAL | wxALL, 6);
            fillBox->Add(m_fillLine,    0, wxALIGN_CENTER_VERTICAL | wxALL, 6);
            fillBox->Add(m_fillEllipse, 0, wxALIGN_CENTER_VERTICAL | wxALL, 6);
            outer->Add(fillBox, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 8);
        }

        outer->Add(CreateButtonSizer(wxOK | wxCANCEL), 0, wxALL | wxALIGN_RIGHT, 8);

        SetSizerAndFit(outer);
        Centre();
    }

    void Apply()
    {
        m_settings.randomDistBlocks = (uint32_t)std::clamp(
            m_spinRandom->GetValue(),
            (int)RANDOM_DIST_BLOCKS_MIN, (int)RANDOM_DIST_BLOCKS_MAX);
        m_settings.animation = m_checkAnim->GetValue();
        for (int i = 0; i < 9; ++i) {
            if (m_anchorBtns[i] && m_anchorBtns[i]->GetValue()) {
                m_settings.resizeAnchor = static_cast<Cave::ResizeAnchor>(i);
                break;
            }
        }
        if (m_fillSquare) {
            if      (m_fillLine   ->GetValue()) m_settings.fillMode = Cave::FillMode::Line;
            else if (m_fillEllipse->GetValue()) m_settings.fillMode = Cave::FillMode::Ellipse;
            else                                m_settings.fillMode = Cave::FillMode::Rectangle;
        }
    }

private:
    Cave::Settings& m_settings;
    wxSpinCtrl*     m_spinRandom     = nullptr;
    wxCheckBox*     m_checkAnim      = nullptr;
    wxRadioButton*  m_anchorBtns[9] = {};
    wxRadioButton*  m_fillSquare     = nullptr;
    wxRadioButton*  m_fillLine       = nullptr;
    wxRadioButton*  m_fillEllipse    = nullptr;
};

} // namespace

void Cave::editCaveSettings(sf::RenderWindow&, Cave::Settings& settings, bool& trigger, bool developerMode)
{
    ensureWxInit();
    Cave::Settings backup = settings;
    SettingsDialog dlg(settings, developerMode);
    if (dlg.ShowModal() == wxID_OK)
        dlg.Apply();
    else
        settings = backup;
    trigger = true;
}
