#include "HUD/Toolbar.h"
#include "Editor/Editor.h"
#include "Cave/Entity/Entity.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <filesystem>
#ifdef _WIN32
#include <windows.h>
#include <shellapi.h>
#endif
#include "tinyfiledialogs.h"

static void openUrl(const char* path)
{
    std::string absPath = std::filesystem::absolute(path).string();
#ifdef _WIN32
    ShellExecuteA(nullptr, "open", absPath.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
#elif defined(__APPLE__)
    std::string cmd = "open \"" + absPath + "\"";
    if (std::system(cmd.c_str())) {}
#else
    // Detect WSL and open via Windows explorer instead of xdg-open
    bool isWSL = false;
    {
        std::ifstream ver("/proc/sys/kernel/osrelease");
        std::string line;
        if (std::getline(ver, line))
        {
            std::string low = line;
            std::transform(low.begin(), low.end(), low.begin(), ::tolower);
            isWSL = low.find("microsoft") != std::string::npos;
        }
    }
    if (isWSL)
    {
        FILE* pipe = popen(("wslpath -w '" + absPath + "'").c_str(), "r");
        if (pipe)
        {
            char buf[4096] = {};
            if (fgets(buf, sizeof(buf), pipe))
            {
                std::string winPath(buf);
                if (!winPath.empty() && winPath.back() == '\n') winPath.pop_back();
                std::system(("explorer.exe \"" + winPath + "\"").c_str());
            }
            pclose(pipe);
        }
    }
    else
    {
        if (std::system(("xdg-open \"" + absPath + "\"").c_str())) {}
    }
#endif
}

HUD::Editor::Toolbar::Toolbar(::Editor* editor) : m_editor(editor)
{
    std::ifstream f("./assets/builder.txt");
    if (f)
    {
        std::ostringstream ss;
        ss << f.rdbuf();
        m_aboutText = ss.str();
    }
}

void HUD::Editor::Toolbar::draw(HUD::Editor::Panel* editorPanel, bool developerMode)
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4.f, 4.f));

    if (!ImGui::BeginMainMenuBar()) { ImGui::PopStyleVar(); return; }

    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, m_clrBg);
    ImGui::PushStyleColor(ImGuiCol_HeaderActive,  m_clrBg);

    drawFileMenu();
    drawEditMenu();
    drawViewMenu();
    drawToolsMenu(editorPanel);
    drawHelpMenu();

    if (developerMode)
        ImGui::TextUnformatted("(Developer Mode)");

    ImGui::PopStyleColor(2);
    ImGui::EndMainMenuBar();
    ImGui::PopStyleVar();

    drawAboutPopup();
}

void HUD::Editor::Toolbar::drawMenuBorder(ImVec2 r0, ImVec2 r1, bool pressed)
{
    ImDrawList* dl = ImGui::GetWindowDrawList();
    const ImU32 light = IM_COL32(255, 255, 255, 255);
    const ImU32 dark  = IM_COL32(128, 128, 128, 255);
    const ImU32 tl = pressed ? dark : light;
    const ImU32 br = pressed ? light : dark;
    dl->AddLine(r0,                    {r1.x - 1, r0.y},     tl);
    dl->AddLine(r0,                    {r0.x,     r1.y - 1}, tl);
    dl->AddLine({r0.x,     r1.y - 1}, {r1.x,     r1.y - 1}, br);
    dl->AddLine({r1.x - 1, r0.y},     {r1.x - 1, r1.y},     br);
}

void HUD::Editor::Toolbar::drawFileMenu()
{
    bool open = ImGui::BeginMenu("File");
    bool hov  = ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup);
    ImVec2 r0 = ImGui::GetItemRectMin(), r1 = ImGui::GetItemRectMax();
    if (open)
    {
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, m_clrNavy);
        ImGui::PushStyleColor(ImGuiCol_HeaderActive,  m_clrNavy);
        ImGui::Indent(m_menuIndent);
        if (ImGui::MenuItem("New", "Ctrl+N")) m_editor->actionNewFile();
        if (ImGui::MenuItem("Open...",    "Ctrl+O")) m_editor->actionOpenFile();
        if (ImGui::MenuItem("Save",       "Ctrl+S")) m_editor->actionSaveFile();
        if (ImGui::MenuItem("Save As...", "Ctrl+A")) m_editor->actionSaveFileAs();
        ImGui::Separator();
        if (ImGui::MenuItem("Settings..."))          m_editor->actionShowSettings();
        ImGui::Separator();
        if (ImGui::MenuItem("Test Level", "Ctrl+T")) m_editor->actionTest();
        ImGui::Separator();
        if (ImGui::MenuItem("Exit"))                 m_editor->actionExit();
        ImGui::Unindent(m_menuIndent);
        ImGui::PopStyleColor(2);
        ImGui::EndMenu();
    }
    if (hov || open) drawMenuBorder(r0, r1, open);
}

void HUD::Editor::Toolbar::drawEditMenu()
{
    bool open = ImGui::BeginMenu("Edit");
    bool hov  = ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup);
    ImVec2 r0 = ImGui::GetItemRectMin(), r1 = ImGui::GetItemRectMax();
    if (open)
    {
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, m_clrNavy);
        ImGui::PushStyleColor(ImGuiCol_HeaderActive,  m_clrNavy);
        ImGui::Indent(m_menuIndent);
        if (ImGui::MenuItem("Undo", "Ctrl+U")) m_editor->actionUndo();
        ImGui::Separator();
        if (ImGui::MenuItem("Copy Level",  "Ctrl+C")) m_editor->actionCopyLevel();
        if (ImGui::MenuItem("Paste Level", "Ctrl+V")) m_editor->actionPasteLevel();
        if (ImGui::MenuItem("Clear Level", "Ctrl+L")) m_editor->actionClearLevel();
        ImGui::Separator();
        if (ImGui::MenuItem("Insert Level", "Insert")) m_editor->actionInsertLevel();
        if (ImGui::MenuItem("Remove Level", "Delete")) m_editor->actionDeleteLevel();
        ImGui::Separator();
        if (ImGui::MenuItem("Cave Properties...", "Ctrl+P")) m_editor->actionShowCaveProperties();
        ImGui::Separator();
        if (ImGui::MenuItem("Random Distribution", "Ctrl+R")) m_editor->actionRandomDist();
        ImGui::Unindent(m_menuIndent);
        ImGui::PopStyleColor(2);
        ImGui::EndMenu();
    }
    if (hov || open) drawMenuBorder(r0, r1, open);
}

void HUD::Editor::Toolbar::drawViewMenu()
{
    bool open = ImGui::BeginMenu("View");
    bool hov  = ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup);
    ImVec2 r0 = ImGui::GetItemRectMin(), r1 = ImGui::GetItemRectMax();
    if (open)
    {
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, m_clrNavy);
        ImGui::PushStyleColor(ImGuiCol_HeaderActive,  m_clrNavy);
        ImGui::Indent(m_menuIndent);
        if (ImGui::MenuItem("Next Level",     "+")) m_editor->actionNextLevel();
        if (ImGui::MenuItem("Previous Level", "-")) m_editor->actionPrevLevel();
        ImGui::Separator();
        if (ImGui::MenuItem("Normal Blocks"))       m_editor->actionSetSmallBlocks(false);
        if (ImGui::MenuItem("Small Blocks"))        m_editor->actionSetSmallBlocks(true);
        ImGui::Unindent(m_menuIndent);
        ImGui::PopStyleColor(2);
        ImGui::EndMenu();
    }
    if (hov || open) drawMenuBorder(r0, r1, open);
}

void HUD::Editor::Toolbar::drawToolItems(HUD::Editor::Panel* editorPanel)
{
    Cave::Entity::Type selType = editorPanel->getSelectedType();
    auto tool = [&](const char* label, const char* shortcut,
                    int px, int py, Cave::Entity::Type type)
    {
        if (ImGui::MenuItem(label, shortcut))
            editorPanel->selectType(px, py);
        if (type == selType)
        {
            ImVec2 rmin = ImGui::GetItemRectMin();
            ImVec2 rmax = ImGui::GetItemRectMax();
            float cy = (rmin.y + rmax.y) * 0.5f;
            float cx = rmin.x - (m_menuScale > 1.f ? 9.f : 7.f);
            ImGui::GetWindowDrawList()->AddCircleFilled(
                ImVec2(cx, cy), ImGui::GetFontSize() * 0.15f, IM_COL32(0, 0, 0, 255));
        }
    };

    tool("Space",                    "1", 0, 0, Cave::Entity::Type::Space);
    tool("Dirt",                     "2", 1, 0, Cave::Entity::Type::Dirt);
    tool("Boulder",                  "3", 2, 0, Cave::Entity::Type::Boulder);
    tool("Diamond",                  "4", 0, 1, Cave::Entity::Type::Diamond);
    tool("Wall",                     "5", 0, 2, Cave::Entity::Type::Wall);
    tool("Solid Wall",               "6", 1, 2, Cave::Entity::Type::SolidWall);
    tool("Magic Wall",               "7", 2, 2, Cave::Entity::Type::MagicWallActive);
    tool("Protozo",                  "8", 0, 4, Cave::Entity::Type::Protozo);
    tool("Cave Gull",                "9", 1, 4, Cave::Entity::Type::CaveGull);
    tool("Amoeba",                   "0", 2, 5, Cave::Entity::Type::Amoeba);
    tool("Horizontal Tube",          "",  0, 7, Cave::Entity::Type::TubeHorizontal);
    tool("Vertical Tube",            "",  1, 7, Cave::Entity::Type::TubeVertical);
    tool("Cross Tube",               "",  2, 7, Cave::Entity::Type::TubeCross);
    tool("Tube Right",               "",  1, 8, Cave::Entity::Type::TubeRight);
    tool("Tube Left",                "",  0, 8, Cave::Entity::Type::TubeLeft);
    tool("Tube Up",                  "",  2, 8, Cave::Entity::Type::TubeUp);
    tool("Tube Down",                "",  0, 9, Cave::Entity::Type::TubeDown);
    tool("Horizontal Expanding Wall","",  0, 3, Cave::Entity::Type::HorizontalWall);
    tool("Vertical Expanding Wall",  "",  1, 3, Cave::Entity::Type::VerticalWall);
    tool("Plasma",                   "",  2, 3, Cave::Entity::Type::Plasma);
    tool("Detonator",                "",  0, 6, Cave::Entity::Type::Detonator);
    tool("TNT Box",                  "",  1, 6, Cave::Entity::Type::TNT);
    tool("Drop Bomb",                "",  2, 6, Cave::Entity::Type::Bomb);
    tool("Fragile Diamond",          "",  1, 1, Cave::Entity::Type::FragileDiamond);
    tool("Diamond Eater",            "",  2, 4, Cave::Entity::Type::Eater);
    tool("Aggressor",                "",  0, 5, Cave::Entity::Type::Aggressor);
    tool("Cilia",                    "",  1, 5, Cave::Entity::Type::Cilia);
    tool("Granite Ore",              "",  2, 1, Cave::Entity::Type::Ore);
    tool("Start Door",               "",  1, 9, Cave::Entity::Type::StartDoor);
    tool("Exit Door",                "",  2, 9, Cave::Entity::Type::ExitDoor);
}

void HUD::Editor::Toolbar::drawToolsMenu(HUD::Editor::Panel* editorPanel)
{
    bool open = ImGui::BeginMenu("Tools");
    bool hov  = ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup);
    ImVec2 r0 = ImGui::GetItemRectMin(), r1 = ImGui::GetItemRectMax();
    if (open)
    {
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, m_clrNavy);
        ImGui::PushStyleColor(ImGuiCol_HeaderActive,  m_clrNavy);
        ImGui::Indent(m_menuIndent);
        drawToolItems(editorPanel);
        ImGui::Unindent(m_menuIndent);
        ImGui::PopStyleColor(2);
        ImGui::EndMenu();
    }
    if (hov || open) drawMenuBorder(r0, r1, open);
}

void HUD::Editor::Toolbar::drawToolsContextPopup(HUD::Editor::Panel* editorPanel)
{
    if (ImGui::BeginPopup("##tools_ctx"))
    {
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, m_clrNavy);
        ImGui::PushStyleColor(ImGuiCol_HeaderActive,  m_clrNavy);
        ImGui::Indent(20.f);
        drawToolItems(editorPanel);
        ImGui::Unindent(20.f);
        ImGui::PopStyleColor(2);
        ImGui::EndPopup();
    }
}

void HUD::Editor::Toolbar::drawHelpMenu()
{
    bool open = ImGui::BeginMenu("Help");
    bool hov  = ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup);
    ImVec2 r0 = ImGui::GetItemRectMin(), r1 = ImGui::GetItemRectMax();
    if (open)
    {
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, m_clrNavy);
        ImGui::PushStyleColor(ImGuiCol_HeaderActive,  m_clrNavy);
        ImGui::Indent(m_menuIndent);
        if (ImGui::MenuItem("Contents..."))
            openUrl("./assets/manual/manual.html");
        if (ImGui::MenuItem("Object Reference..."))
            openUrl("./assets/manual/enviroment.html");
        ImGui::Separator();
        if (ImGui::MenuItem("About...")) m_showAbout = true;
        ImGui::Unindent(m_menuIndent);
        ImGui::PopStyleColor(2);
        ImGui::EndMenu();
    }
    if (hov || open) drawMenuBorder(r0, r1, open);
}

void HUD::Editor::Toolbar::drawAboutPopup()
{
    if (!m_showAbout) return;
    m_showAbout = false;
    tinyfd_messageBox("About Builder", m_aboutText.c_str(), "ok", "info", 1);
}
