#include "Cave/Properties/Properties.h"
#include <wx/wx.h>
#include <wx/spinctrl.h>
#include <wx/slider.h>
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

class PropertiesDialog : public wxDialog
{
public:
    PropertiesDialog(Cave::Properties& props, int diamondCount, bool developerMode, std::function<void()> onTest)
        : wxDialog(nullptr, wxID_ANY, "Cave Properties",
                   wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE)
        , m_props(props)
        , m_diamondCount(diamondCount)
        , m_developerMode(developerMode)
        , m_onTest(std::move(onTest))
    {
        auto* outer = new wxBoxSizer(wxVERTICAL);

        // ── Size: Width / Height ──────────────────────────────────────────────
        if (developerMode)
        {
            auto* box  = new wxStaticBoxSizer(wxVERTICAL, this, "Size");
            wxWindow* p = box->GetStaticBox();

            auto* grid = new wxFlexGridSizer(2, 2, 6, 10);
            grid->Add(lbl(p, "Width:"),  0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
            m_width  = spin(p, 20, 255, props.width);
            grid->Add(m_width,  0, wxALIGN_CENTER_VERTICAL);
            grid->Add(lbl(p, "Height:"), 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
            m_height = spin(p, 13, 255, props.height);
            grid->Add(m_height, 0, wxALIGN_CENTER_VERTICAL);

            box->Add(grid, 0, wxALL, 10);
            outer->Add(box, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 8);
        }

        // ── Top row: Diamonds (left) | Amoeba+Time (right) ───────────────────
        auto* topRow = new wxBoxSizer(wxHORIZONTAL);

        {
            auto* box  = new wxStaticBoxSizer(wxVERTICAL, this, "");
            wxWindow* p = box->GetStaticBox();
            auto* grid = new wxFlexGridSizer(3, 3, 6, 10);

            grid->Add(lbl(p, "Diamonds to collect:"), 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
            m_quota = spin(p, QUOTA_MIN, QUOTA_MAX, props.quota);
            grid->Add(m_quota, 0, wxALIGN_CENTER_VERTICAL);
            m_btnCount = new wxButton(p, wxID_ANY, "Count", wxDefaultPosition, wxSize(120, -1));
            grid->Add(m_btnCount, 0, wxALIGN_CENTER_VERTICAL);

            grid->Add(lbl(p, "Diamond value:"), 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
            m_diamVal = spin(p, DIAMOND_VALUE_MIN, DIAMOND_VALUE_MAX, props.diamondValue);
            grid->Add(m_diamVal, 0, wxALIGN_CENTER_VERTICAL);
            grid->AddSpacer(0);

            grid->Add(lbl(p, "Extra diamond value:"), 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
            m_extraDiamVal = spin(p, EXTRA_DIAMOND_VALUE_MIN, EXTRA_DIAMOND_VALUE_MAX, props.extraDiamondValue);
            grid->Add(m_extraDiamVal, 0, wxALIGN_CENTER_VERTICAL);
            grid->AddSpacer(0);

            box->Add(grid, 0, wxALL, 10);
            topRow->Add(box, 1, wxEXPAND | wxRIGHT, 8);
        }

        {
            auto* rightCol = new wxBoxSizer(wxVERTICAL);

            {
                auto* box  = new wxStaticBoxSizer(wxVERTICAL, this, "");
                wxWindow* p = box->GetStaticBox();
                auto* grid = new wxFlexGridSizer(1, 2, 6, 10);
                grid->Add(lbl(p, "Amoeba growth max:"), 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
                m_amoebaMax = spin(p, AMOEBA_GRWOTH_MAX_MIN, AMOEBA_GRWOTH_MAX_MAX, props.amoebaGrowthMax);
                grid->Add(m_amoebaMax, 0, wxALIGN_CENTER_VERTICAL);
                box->Add(grid, 0, wxALL, 10);
                rightCol->Add(box, 0, wxEXPAND | wxBOTTOM, 8);
            }

            {
                auto* box  = new wxStaticBoxSizer(wxVERTICAL, this, "");
                wxWindow* p = box->GetStaticBox();
                auto* grid = new wxFlexGridSizer(2, 2, 6, 10);
                grid->Add(lbl(p, "Time:"), 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
                m_time = spin(p, TIME_MIN, TIME_MAX, props.time);
                grid->Add(m_time, 0, wxALIGN_CENTER_VERTICAL);
                grid->Add(lbl(p, "Magic wall expire time:"), 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
                m_magicTime = spin(p, MAGIC_WALL_TIME_MIN, MAGIC_WALL_TIME_MAX, props.magicWallTime);
                grid->Add(m_magicTime, 0, wxALIGN_CENTER_VERTICAL);
                box->Add(grid, 0, wxALL, 10);
                rightCol->Add(box, 0, wxEXPAND);
            }

            topRow->Add(rightCol, 1, wxEXPAND);
        }

        outer->Add(topRow, 0, wxEXPAND | wxALL, 8);

        // ── Amoeba growth speed ───────────────────────────────────────────────
        outer->Add(sliderGroup("Amoeba growth speed",
                               m_amoebaSpd, props.amoebaGrowthSpeed,
                               AMOEBA_GROWTH_SPEED_MIN, AMOEBA_GROWTH_SPEED_MAX),
                   0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 8);

        // ── Plasma growth speed ───────────────────────────────────────────────
        outer->Add(sliderGroup("Plasma growth speed",
                               m_plasmaSpd, props.plasmaGrowthSpeed,
                               PLASMA_GROWTH_SPEED_MIN, PLASMA_GROWTH_SPEED_MAX),
                   0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 8);

        // ── Colors ────────────────────────────────────────────────────────────
        {
            auto* box  = new wxStaticBoxSizer(wxVERTICAL, this, "Colors");
            wxWindow* p = box->GetStaticBox();
            auto* grid = new wxFlexGridSizer(3, 2, 6, 10);
            grid->AddGrowableCol(1);

            grid->Add(lbl(p, "Hue"),   0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
            m_hue = new wxSlider(p, wxID_ANY, props.hue,   HUE_MIN, HUE_MAX);
            grid->Add(m_hue, 1, wxEXPAND | wxALIGN_CENTER_VERTICAL);

            grid->Add(lbl(p, "Sat"),   0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
            m_sat = new wxSlider(p, wxID_ANY, props.sat,   SAT_MIN, SAT_MAX);
            grid->Add(m_sat, 1, wxEXPAND | wxALIGN_CENTER_VERTICAL);

            grid->Add(lbl(p, "Light"), 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
            m_lum = new wxSlider(p, wxID_ANY, props.lum,   LUM_MIN, LUM_MAX);
            grid->Add(m_lum, 1, wxEXPAND | wxALIGN_CENTER_VERTICAL);

            box->Add(grid, 0, wxEXPAND | wxALL, 10);

            auto* btnRow = new wxBoxSizer(wxHORIZONTAL);
            m_btnTest  = new wxButton(p, wxID_ANY, "Test");
            m_btnReset = new wxButton(p, wxID_ANY, "Reset");
            btnRow->Add(m_btnTest,  0, wxRIGHT, 8);
            btnRow->Add(m_btnReset);
            box->Add(btnRow, 0, wxLEFT | wxBOTTOM, 10);

            outer->Add(box, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 8);
        }

        outer->Add(CreateButtonSizer(wxOK | wxCANCEL), 0, wxALL | wxALIGN_RIGHT, 8);

        SetSizerAndFit(outer);
        Centre();

        m_btnCount->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
            m_quota->SetValue(m_diamondCount);
        });
        m_btnTest->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
            ApplyToProps();
            if (m_onTest) m_onTest();
        });
        m_btnReset->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
            m_hue->SetValue(100);
            m_sat->SetValue(100);
            m_lum->SetValue(100);
        });
    }

    void ApplyToProps()
    {
        auto clampGet = [](wxSpinCtrl* s, uint32_t lo, uint32_t hi) -> uint32_t {
            return (uint32_t)std::clamp(s->GetValue(), (int)lo, (int)hi);
        };
        auto clampSlider = [](wxSlider* s, uint32_t lo, uint32_t hi) -> uint32_t {
            return (uint32_t)std::clamp(s->GetValue(), (int)lo, (int)hi);
        };

        if (m_developerMode && m_width && m_height) {
            m_props.width  = (uint32_t)std::clamp(m_width->GetValue(),  20, 255);
            m_props.height = (uint32_t)std::clamp(m_height->GetValue(), 13, 255);
        }
        m_props.quota             = clampGet(m_quota,        QUOTA_MIN,               QUOTA_MAX);
        m_props.diamondValue      = clampGet(m_diamVal,      DIAMOND_VALUE_MIN,        DIAMOND_VALUE_MAX);
        m_props.extraDiamondValue = clampGet(m_extraDiamVal, EXTRA_DIAMOND_VALUE_MIN,  EXTRA_DIAMOND_VALUE_MAX);
        m_props.amoebaGrowthMax   = clampGet(m_amoebaMax,    AMOEBA_GRWOTH_MAX_MIN,    AMOEBA_GRWOTH_MAX_MAX);
        m_props.time              = clampGet(m_time,         TIME_MIN,                 TIME_MAX);
        m_props.magicWallTime     = clampGet(m_magicTime,    MAGIC_WALL_TIME_MIN,      MAGIC_WALL_TIME_MAX);
        m_props.amoebaGrowthSpeed = clampSlider(m_amoebaSpd, AMOEBA_GROWTH_SPEED_MIN,  AMOEBA_GROWTH_SPEED_MAX);
        m_props.plasmaGrowthSpeed = clampSlider(m_plasmaSpd, PLASMA_GROWTH_SPEED_MIN,  PLASMA_GROWTH_SPEED_MAX);
        m_props.hue               = clampSlider(m_hue,       HUE_MIN,                  HUE_MAX);
        m_props.sat               = clampSlider(m_sat,       SAT_MIN,                  SAT_MAX);
        m_props.lum               = clampSlider(m_lum,       LUM_MIN,                  LUM_MAX);
    }

private:
    wxStaticText* lbl(wxWindow* parent, const char* text)
    {
        return new wxStaticText(parent, wxID_ANY, text);
    }

    wxSpinCtrl* spin(wxWindow* parent, uint32_t lo, uint32_t hi, uint32_t val)
    {
        auto* s = new wxSpinCtrl(parent, wxID_ANY, wxEmptyString,
                                 wxDefaultPosition, wxDefaultSize);
        s->SetRange((int)lo, (int)hi);
        s->SetValue((int)val);
        return s;
    }

    wxStaticBoxSizer* sliderGroup(const char* title, wxSlider*& outSlider,
                                  uint32_t val, uint32_t lo, uint32_t hi)
    {
        auto* box = new wxStaticBoxSizer(wxVERTICAL, this, title);
        wxWindow* p = box->GetStaticBox();
        auto* row = new wxBoxSizer(wxHORIZONTAL);
        row->Add(new wxStaticText(p, wxID_ANY, "Min"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 6);
        outSlider = new wxSlider(p, wxID_ANY, val, lo, hi);
        row->Add(outSlider, 1, wxEXPAND);
        row->Add(new wxStaticText(p, wxID_ANY, "Max"), 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 6);
        box->Add(row, 0, wxEXPAND | wxALL, 10);
        return box;
    }

    Cave::Properties&       m_props;
    int                     m_diamondCount;
    bool                    m_developerMode;
    std::function<void()>   m_onTest;

    wxSpinCtrl*    m_width           = nullptr;
    wxSpinCtrl*    m_height          = nullptr;

    wxSpinCtrl* m_quota        = nullptr;
    wxSpinCtrl* m_diamVal      = nullptr;
    wxSpinCtrl* m_extraDiamVal = nullptr;
    wxSpinCtrl* m_amoebaMax    = nullptr;
    wxSpinCtrl* m_time         = nullptr;
    wxSpinCtrl* m_magicTime    = nullptr;
    wxSlider*   m_amoebaSpd    = nullptr;
    wxSlider*   m_plasmaSpd    = nullptr;
    wxSlider*   m_hue          = nullptr;
    wxSlider*   m_sat          = nullptr;
    wxSlider*   m_lum          = nullptr;
    wxButton*   m_btnCount     = nullptr;
    wxButton*   m_btnTest      = nullptr;
    wxButton*   m_btnReset     = nullptr;
};

} // namespace

void Cave::editCaveProperties(sf::RenderWindow&, Cave::Properties& props,
                               Cave::Properties original, const int& diamondCount,
                               bool& trigger, bool developerMode,
                               std::function<void()> onTest)
{
    ensureWxInit();
    PropertiesDialog dlg(props, diamondCount, developerMode, std::move(onTest));
    if (dlg.ShowModal() == wxID_OK)
        dlg.ApplyToProps();
    else
        props = original;
    trigger = true;
}
