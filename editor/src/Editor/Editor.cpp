#include "Editor/Editor.h"
#include "Cave/Map/Map.h"
#include "Cave/Properties/Properties.h"
#include "Camera/Camera.h"
#include "HUD/Panel.h"
#include "Utils/Counter.h"
#include "HUD/Toolbar.h"
#include "Shader/Shader.h"

#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <algorithm>
#include <filesystem>
#include <random>
#ifdef _WIN32
#include <windows.h>
#include <dwmapi.h>
#endif
#include "tinyfiledialogs.h"
#include "Cave/Manager/File.h"
#include "Cave/Manager/Data.h"
#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif
#if defined(__linux__) || defined(__APPLE__)
#include <unistd.h>
#include <sys/wait.h>
#endif

static std::string getExeDir() {
    std::filesystem::path exePath;
#if defined(_WIN32)
    wchar_t buf[4096];
    DWORD len = GetModuleFileNameW(nullptr, buf, 4096);
    if (len > 0) exePath = std::filesystem::path(buf);
#elif defined(__APPLE__)
    char buf[4096];
    uint32_t size = sizeof(buf);
    if (_NSGetExecutablePath(buf, &size) == 0) exePath = std::filesystem::path(buf);
#elif defined(__linux__)
    char buf[4096];
    ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
    if (len != -1) { buf[len] = '\0'; exePath = std::filesystem::path(buf); }
#endif
    return exePath.empty() ? "." : exePath.parent_path().string();
}

static std::string getCavesDir() {
    return (std::filesystem::path(getExeDir()) / "caves" / "").string();
}

// -----------------------------------------------------------------------
// Virtual screen layout (all sizes in pixels):
//
//   |<-------- 640 ------->|<16>|<------ 104 ------>|
//   +----------------------+----+-------------------+  ---
//   |     Menu toolbar     |    |   Menu toolbar    |   20  (TOOLBAR_H)
//   +----------------------+----+-------------------+  ---
//   |                      | V  |                   |
//   |    Cave map view     | S  |   Entity panel    |  480  (MAP_H)
//   |      640 x 480       | B  |    104 wide       |
//   |                      |    |                   |
//   +----------------------+----+-------------------+  ---
//   |  Horizontal scroll   |cor |                   |   16  (SB_THICK)
//   +----------------------+----+-------------------+  ---
//
//   WIN_W = 640 + 16 + 104 = 760
//   WIN_H =  20 + 480 + 16 = 516
// -----------------------------------------------------------------------
static constexpr unsigned int WIN_W = 760u;
static constexpr unsigned int WIN_H = 516u;

static constexpr float TOOLBAR_H = 20.f;
static constexpr float MAP_W     = 640.f;
static constexpr float MAP_H     = 480.f;
static constexpr float SB_THICK  = 16.f;
static constexpr float PANEL_W   = 104.f;

static constexpr float VSB_X          = MAP_W;
static constexpr float HSB_Y          = TOOLBAR_H + MAP_H;
static constexpr float PANEL_X        = MAP_W + SB_THICK;
static constexpr float PANEL_SCREEN_H = (float)WIN_H - TOOLBAR_H;

static constexpr float VSB_TRACK_H = MAP_H;
static constexpr float HSB_TRACK_W = MAP_W;

static constexpr float VSB_EFF_Y = TOOLBAR_H + SB_THICK;
static constexpr float VSB_EFF_H = MAP_H     - 2.f * SB_THICK;
static constexpr float HSB_EFF_X = SB_THICK;
static constexpr float HSB_EFF_W = MAP_W     - 2.f * SB_THICK;

static constexpr float CAVE_VP_X = 0.f;
static constexpr float CAVE_VP_Y = TOOLBAR_H / WIN_H;
static constexpr float CAVE_VP_W = MAP_W / WIN_W;
static constexpr float CAVE_VP_H = MAP_H / WIN_H;

static constexpr float PANEL_VP_X = PANEL_X / WIN_W;
static constexpr float PANEL_VP_Y = TOOLBAR_H / WIN_H;
static constexpr float PANEL_VP_W = PANEL_W / WIN_W;
static constexpr float PANEL_VP_H = PANEL_SCREEN_H / WIN_H;

static Cave::Properties defaultCaveProperties(uint32_t width = 50, uint32_t height = 30) {
    Cave::Properties p{};
    p.width             = width;
    p.height            = height;
    p.time              = 100;
    p.quota             = 25;
    p.diamondValue      = 10;
    p.extraDiamondValue = 15;
    p.amoebaGrowthSpeed = 128;
    p.amoebaGrowthMax   = 1000;
    p.magicWallTime     = 20;
    p.plasmaGrowthSpeed = 1000;
    p.hue               = 100;
    p.sat               = 100;
    p.lum               = 100;
    return p;
}

static char entityTypeToTile(Cave::Entity::Type type)
{
    switch (type) {
    case Cave::Entity::Type::Space:               return 0;
    case Cave::Entity::Type::Dirt:                return 1;
    case Cave::Entity::Type::Boulder:             return 2;
    case Cave::Entity::Type::Diamond:             return 3;
    case Cave::Entity::Type::Wall:                return 4;
    case Cave::Entity::Type::SolidWall:           return 5;
    case Cave::Entity::Type::Protozo:             return 7;
    case Cave::Entity::Type::CaveGull:            return 8;
    case Cave::Entity::Type::Amoeba:              return 9;
    case Cave::Entity::Type::MagicWallInactive:
    case Cave::Entity::Type::MagicWallActive:
    case Cave::Entity::Type::MagicWallUsed:       return 12;
    case Cave::Entity::Type::StartDoor:
    case Cave::Entity::Type::StartDoorOpen:       return 13;
    case Cave::Entity::Type::ExitDoor:
    case Cave::Entity::Type::ExitDoorOpen:
    case Cave::Entity::Type::ExitDoorOpening:
    case Cave::Entity::Type::ExitDoorComplete:
    case Cave::Entity::Type::ExitDoorFinished:    return 14;
    case Cave::Entity::Type::HorizontalWall:
    case Cave::Entity::Type::HorizontalWallPlaceholder: return 15;
    case Cave::Entity::Type::Detonator:
    case Cave::Entity::Type::DetonatorTriggered:
    case Cave::Entity::Type::DetonatorUsed:       return 16;
    case Cave::Entity::Type::TNT:                 return 17;
    case Cave::Entity::Type::Bomb:                return 18;
    case Cave::Entity::Type::TubeHorizontal:      return 19;
    case Cave::Entity::Type::TubeVertical:        return 20;
    case Cave::Entity::Type::TubeCross:           return 21;
    case Cave::Entity::Type::TubeRight:           return 22;
    case Cave::Entity::Type::TubeLeft:            return 23;
    case Cave::Entity::Type::TubeUp:              return 24;
    case Cave::Entity::Type::TubeDown:            return 25;
    case Cave::Entity::Type::FragileDiamond:      return 26;
    case Cave::Entity::Type::Eater:               return 27;
    case Cave::Entity::Type::Aggressor:           return 28;
    case Cave::Entity::Type::VerticalWall:
    case Cave::Entity::Type::VerticalWallPlaceholder:   return 30;
    case Cave::Entity::Type::Plasma:              return 31;
    case Cave::Entity::Type::Cilia:               return 32;
    case Cave::Entity::Type::Ore:                 return 33;
    default:                                      return 0;
    }
}

Editor::Editor() {}

void Editor::clearLevel(Cave::Map& map)
{
    auto p = game.getCaveProperties();
    std::vector<char> tileData(p.width * p.height);
    for (int y = 0; y < (int)p.height; ++y)
        for (int x = 0; x < (int)p.width; ++x)
            tileData[y * p.width + x] =
                (x == 0 || y == 0 || x == (int)p.width - 1 || y == (int)p.height - 1) ? 5 : 1;
    map.generateMap(&p, tileData);
    map.setEditorMode();
}

void Editor::copyLevel(const Cave::Map& map)
{
    m_clipboardTileData.resize(map.caveEntities.size());
    for (size_t i = 0; i < map.caveEntities.size(); ++i)
        m_clipboardTileData[i] = entityTypeToTile(map.caveEntities[i].getType());
}

void Editor::pasteLevel(Cave::Map& map)
{
    if (m_clipboardTileData.empty()) return;
    auto p = game.getCaveProperties();
    if (m_clipboardTileData.size() != (size_t)(p.width * p.height)) return;
    map.generateMap(&p, m_clipboardTileData);
    map.setEditorMode();
}

void Editor::syncCurrentCave(Cave::Map& map, Cave::File& loadedFile, int currentCaveIndex)
{
    if (currentCaveIndex < 0 || currentCaveIndex >= (int)loadedFile.caves.size()) return;
    Cave::Data& cave = loadedFile.caves[currentCaveIndex];
    cave.properties = game.getCaveProperties();
    cave.tileData.resize(map.caveEntities.size());
    for (size_t i = 0; i < map.caveEntities.size(); ++i)
        cave.tileData[i] = entityTypeToTile(map.caveEntities[i].getType());
}

void Editor::insertLevel(Cave::Map& map, Cave::File& loadedFile, int& currentCaveIndex)
{
    syncCurrentCave(map, loadedFile, currentCaveIndex);

    Cave::Properties p = defaultCaveProperties();

    Cave::Data newCave;
    newCave.properties = p;
    newCave.tileData.assign(p.width * p.height, 1);
    for (int y = 0; y < (int)p.height; ++y)
        for (int x = 0; x < (int)p.width; ++x)
            if (x == 0 || y == 0 || x == (int)p.width - 1 || y == (int)p.height - 1)
                newCave.tileData[y * p.width + x] = 5;

    loadedFile.caves.insert(loadedFile.caves.begin() + currentCaveIndex + 1, std::move(newCave));
    ++currentCaveIndex;

    const Cave::Data& cave = loadedFile.caves[currentCaveIndex];
    game.setCaveProperties(cave.properties);
    map.generateMap(&cave.properties, cave.tileData);
    map.setEditorMode();
}

void Editor::checkSaveWarnings(const Cave::File& loadedFile)
{
    for (int i = 0; i < (int)loadedFile.caves.size(); ++i)
    {
        const auto& tileData = loadedFile.caves[i].tileData;
        bool hasStart = std::any_of(tileData.begin(), tileData.end(), [](char t){ return t == 13; });
        bool hasExit  = std::any_of(tileData.begin(), tileData.end(), [](char t){ return t == 14; });
        char buf[64];
        if (!hasStart) { std::snprintf(buf, sizeof(buf), "Missing start door in level %03d!", i + 1); tinyfd_messageBox("Save Warning", buf, "ok", "warning", 1); }
        if (!hasExit)  { std::snprintf(buf, sizeof(buf), "Missing exit door in level %03d!",  i + 1); tinyfd_messageBox("Save Warning", buf, "ok", "warning", 1); }
    }
}

bool Editor::checkUnsavedChanges(Cave::Map& map, Cave::File& loadedFile, int currentCaveIndex)
{
    if (!m_isDirty) return true;

    std::string filename = m_savedFilePath.empty() ? "Untitled" : [&]() {
        size_t sep = m_savedFilePath.find_last_of("\\/");
        return (sep != std::string::npos) ? m_savedFilePath.substr(sep + 1) : m_savedFilePath;
    }();

    std::string msg = "Save changes to " + filename;
    // tinyfd returns: 1=yes, 2=no, 0=cancel
    int result = tinyfd_messageBox("Unsaved Changes", msg.c_str(), "yesnocancel", "warning", 0);

    if (result == 0) return false;
    if (result == 1)
    {
        saveFile(map, loadedFile, currentCaveIndex);
        // If still dirty, the user cancelled the save dialog — don't proceed
        if (m_isDirty) return false;
    }
    return true;
}

void Editor::saveFileAs(Cave::Map& map, Cave::File& loadedFile, int currentCaveIndex)
{
    syncCurrentCave(map, loadedFile, currentCaveIndex);

    const char* filterPatterns[] = {"*.cav"};
    // Use a full path including a placeholder filename so Windows common dialog
    // opens at the caves directory regardless of its remembered last-used location.
    std::string defaultPathStr = m_savedFilePath.empty()
        ? (std::filesystem::path(getCavesDir()) / "untitled.cav").string()
        : m_savedFilePath;
    const char* result = tinyfd_saveFileDialog("Save Cave File", defaultPathStr.c_str(), 1, filterPatterns, "Cave Files");
    if (!result) return;

    try
    {
        m_savedFilePath = result;
        if (std::filesystem::path(m_savedFilePath).extension() != ".cav")
            m_savedFilePath += ".cav";
        Cave::File::saveToFile(loadedFile, m_savedFilePath);
        m_isDirty = false;

        size_t sep = m_savedFilePath.find_last_of("/\\");
        loadedFile.filename = (sep != std::string::npos)
            ? m_savedFilePath.substr(sep + 1)
            : m_savedFilePath;

        checkSaveWarnings(loadedFile);
    }
    catch (const std::exception& e)
    {
        tinyfd_messageBox("Error saving cave file", e.what(), "ok", "error", 1);
    }
}

void Editor::saveFile(Cave::Map& map, Cave::File& loadedFile, int currentCaveIndex)
{
    if (m_savedFilePath.empty())
    {
        saveFileAs(map, loadedFile, currentCaveIndex);
        return;
    }

    syncCurrentCave(map, loadedFile, currentCaveIndex);
    try
    {
        Cave::File::saveToFile(loadedFile, m_savedFilePath);
        m_isDirty = false;
        checkSaveWarnings(loadedFile);
    }
    catch (const std::exception& e)
    {
        tinyfd_messageBox("Error saving cave file", e.what(), "ok", "error", 1);
    }
}

void Editor::deleteLevel(Cave::Map& map, Cave::File& loadedFile, int& currentCaveIndex)
{
    if ((int)loadedFile.caves.size() <= 1) return;

    loadedFile.caves.erase(loadedFile.caves.begin() + currentCaveIndex);
    currentCaveIndex = std::max(0, currentCaveIndex - 1);

    const Cave::Data& cave = loadedFile.caves[currentCaveIndex];
    game.setCaveProperties(cave.properties);
    map.generateMap(&cave.properties, cave.tileData);
    map.setEditorMode();
}

void Editor::actionNewFile()            { m_doNewFile       = true; }
void Editor::actionOpenFile()           { m_doOpenFile      = true; }
void Editor::actionSaveFile()           { m_doSaveFile      = true; }
void Editor::actionSaveFileAs()         { m_doSaveFileAs    = true; }
void Editor::actionExit()               { m_doExit          = true; }
void Editor::actionNextLevel()          { m_doNextLevel     = true; }
void Editor::actionPrevLevel()          { m_doPrevLevel     = true; }
void Editor::actionInsertLevel()        { m_doInsertLevel   = true; }
void Editor::actionDeleteLevel()        { m_doDeleteLevel   = true; }
void Editor::actionClearLevel()         { m_doClearLevel    = true; }
void Editor::actionCopyLevel()          { m_doCopyLevel     = true; }
void Editor::actionPasteLevel()         { m_doPasteLevel    = true; }
void Editor::actionRandomDist()         { m_doRandomDist    = true; }
void Editor::actionShowSettings()       { m_doShowSettings  = true; }
void Editor::actionShowCaveProperties() { m_doShowCaveProps = true; }
void Editor::actionSetSmallBlocks(bool small_blocks) { m_smallBlocks = small_blocks; }
void Editor::actionTest()               { m_doTest = true; }
void Editor::actionUndo()               { m_doUndo = true; }

void Editor::saveUndoSnapshot(const Cave::Map& map)
{
    m_undoTileData.resize(map.caveEntities.size());
    for (size_t i = 0; i < map.caveEntities.size(); ++i)
        m_undoTileData[i] = entityTypeToTile(map.caveEntities[i].getType());
    m_isDirty      = true;
    m_undoAvailable = true;
}

void Editor::actionChangeCursor(sf::Cursor::Type type)
{
    if (!m_window) return;
    sf::Cursor cursor(type);
    m_window->setMouseCursor(cursor);
}

void Editor::handleShortcuts(const sf::Event::KeyPressed& e, HUD::Editor::Panel* editorPanel)
{
    if (ImGui::GetIO().WantCaptureKeyboard) return;

    using Key = sf::Keyboard::Key;
    const bool ctrl = e.control;

    if (ctrl)
    {
        switch (e.code)
        {
        case Key::N: actionNewFile();            break;
        case Key::O: actionOpenFile();           break;
        case Key::S: actionSaveFile();           break;
        case Key::A: actionSaveFileAs();         break;
        case Key::T: actionTest();               break;
        case Key::U: actionUndo();               break;
        case Key::C: actionCopyLevel();          break;
        case Key::V: actionPasteLevel();         break;
        case Key::L: actionClearLevel();         break;
        case Key::P: actionShowCaveProperties();          break;
        case Key::R: actionRandomDist();                  break;
        case Key::D: m_developerMode = !m_developerMode;  break;
        default: break;
        }
    }
    else
    {
        switch (e.code)
        {
        case Key::Insert:   actionInsertLevel(); break;
        case Key::Delete:   actionDeleteLevel(); break;
        case Key::Add:      actionNextLevel();   break;
        case Key::Subtract: actionPrevLevel();   break;
        case Key::Num1: editorPanel->selectType(0, 0); break; // Space
        case Key::Num2: editorPanel->selectType(1, 0); break; // Dirt
        case Key::Num3: editorPanel->selectType(2, 0); break; // Boulder
        case Key::Num4: editorPanel->selectType(0, 1); break; // Diamond
        case Key::Num5: editorPanel->selectType(0, 2); break; // Wall
        case Key::Num6: editorPanel->selectType(1, 2); break; // Solid Wall
        case Key::Num7: editorPanel->selectType(2, 2); break; // Magic Wall
        case Key::Num8: editorPanel->selectType(0, 4); break; // Protozo
        case Key::Num9: editorPanel->selectType(1, 4); break; // Cave Gull
        case Key::Num0: editorPanel->selectType(2, 5); break; // Amoeba
        default: break;
        }
    }
}

static std::vector<sf::Vector2i> lineFillRight(sf::Vector2i start, sf::Vector2i end)
{
    std::vector<sf::Vector2i> pts;
    int dx = std::abs(end.x - start.x), dy = std::abs(end.y - start.y);
    int sx = (start.x < end.x) ? 1 : -1, sy = (start.y < end.y) ? 1 : -1;
    int err = dx - dy;
    bool t = false;
    for (;;) {
        pts.push_back(start);
        if (start == end) break;
        int e2 = 2 * err;
        if (!t && e2 > -dy) {
            err -= dy;
            start.x += sx;
        }
        if (t && e2 <  dx) {
            err += dx;
            start.y += sy;
        }
        t = !t;
    }
    return pts;
}

bool Editor::run()
{
    sf::VideoMode    windowMode(sf::Vector2u(WIN_W, WIN_H));
    sf::RenderWindow window(windowMode, EDITOR_NAME);
    m_window = &window;
    sf::RenderTexture rt(sf::Vector2u(WIN_W, WIN_H));
    window.setFramerateLimit(60);

    sf::Image smallIcon, largeIcon;
    (void)smallIcon.loadFromFile("./assets/icons/DiggingJimBuilder/small_png.png");
    (void)largeIcon.loadFromFile("./assets/icons/DiggingJimBuilder/large_png.png");

    auto applyIcons = [&]()
    {
        if (!largeIcon.getSize().x) return;
        window.setIcon(largeIcon);
#ifdef _WIN32
        HWND hwnd = window.getNativeHandle();
        DWM_WINDOW_CORNER_PREFERENCE pref = DWMWCP_DONOTROUND;
        DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &pref, sizeof(pref));
        if (smallIcon.getSize().x)
        {
            const sf::Vector2u sz = smallIcon.getSize();
            BITMAPV5HEADER bi  = {};
            bi.bV5Size         = sizeof(bi);
            bi.bV5Width        = (LONG)sz.x;
            bi.bV5Height       = -(LONG)sz.y;
            bi.bV5Planes       = 1;
            bi.bV5BitCount     = 32;
            bi.bV5Compression  = BI_BITFIELDS;
            bi.bV5RedMask      = 0x00FF0000;
            bi.bV5GreenMask    = 0x0000FF00;
            bi.bV5BlueMask     = 0x000000FF;
            bi.bV5AlphaMask    = 0xFF000000;
            void* pBits = nullptr;
            HDC hdc = GetDC(nullptr);
            HBITMAP hBmp = CreateDIBSection(hdc, (BITMAPINFO*)&bi, DIB_RGB_COLORS, &pBits, nullptr, 0);
            ReleaseDC(nullptr, hdc);
            if (hBmp)
            {
                const uint8_t* src = smallIcon.getPixelsPtr();
                uint8_t* dst = static_cast<uint8_t*>(pBits);
                for (uint32_t i = 0; i < sz.x * sz.y; ++i)
                {
                    dst[i*4+0] = src[i*4+2];
                    dst[i*4+1] = src[i*4+1];
                    dst[i*4+2] = src[i*4+0];
                    dst[i*4+3] = src[i*4+3];
                }
                HBITMAP hMask = CreateBitmap((int)sz.x, (int)sz.y, 1, 1, nullptr);
                ICONINFO ii = {};
                ii.fIcon    = TRUE;
                ii.hbmColor = hBmp;
                ii.hbmMask  = hMask;
                HICON hSmall = CreateIconIndirect(&ii);
                if (hSmall) SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hSmall);
                DeleteObject(hBmp);
                DeleteObject(hMask);
            }
        }
#endif
    };
    applyIcons();

    (void)ImGui::SFML::Init(window, /* loadDefaultFont= */ false);

    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF("./assets/fonts/tahoma.ttf", 12.f);
    (void)ImGui::SFML::UpdateFontTexture();

    bool isFullscreen = false;
    sf::Clock deltaClock;

    auto toggleFullscreen = [&]()
    {
        ImGui::SFML::Shutdown();
        // Reset to arrow before window.create() — SFML reapplies the stored cursor
        // during window recreation, and a stale X11 cursor XID causes BadCursor on Linux/WSL.
        { sf::Cursor arrow(sf::Cursor::Type::Arrow); window.setMouseCursor(arrow); }
        if (!isFullscreen)
        {
            const auto& modes = sf::VideoMode::getFullscreenModes();
            sf::VideoMode fsMode = modes.empty() ? sf::VideoMode::getDesktopMode() : modes[0];
            window.create(fsMode, EDITOR_NAME, sf::State::Fullscreen);
            isFullscreen = true;
        }
        else
        {
            window.create(sf::VideoMode(sf::Vector2u(WIN_W, WIN_H)), EDITOR_NAME);
            isFullscreen = false;
        }
        window.setFramerateLimit(60);
        applyIcons();
        { sf::Cursor arrow(sf::Cursor::Type::Arrow); window.setMouseCursor(arrow); }
        (void)ImGui::SFML::Init(window, /* loadDefaultFont= */ false);
        ImGui::GetIO().Fonts->AddFontFromFileTTF("./assets/fonts/tahoma.ttf", 12.f);
        (void)ImGui::SFML::UpdateFontTexture();

        ImGui::SFML::Update(window, sf::Time::Zero);
        ImGui::GetIO().DisplaySize = ImVec2((float)WIN_W, (float)WIN_H);
        ImGui::EndFrame();
        deltaClock.restart();
    };

    auto makeWindowView = [&]() -> sf::View
    {
        return sf::View(sf::FloatRect(sf::Vector2f(0.f, 0.f),
                                      sf::Vector2f((float)WIN_W, (float)WIN_H)));
    };

    auto windowToVirtual = [&](sf::Vector2i winPos) -> sf::Vector2f
    {
        return window.mapPixelToCoords(winPos, makeWindowView());
    };

    game.imageManager.loadAllImages();
    imageManager.loadAllImages();
    game.soundManager.loadAllSounds();

    Shader::Shader shaderManager(&game);
    shaderManager.loadAllShaders();

    constexpr int CAVE_W = 50;
    constexpr int CAVE_H = 30;

    {
        Cave::Properties p = defaultCaveProperties(CAVE_W, CAVE_H);
        game.setCaveProperties(p);
    }

    std::vector<char> tileData(CAVE_W * CAVE_H);
    for (int y = 0; y < CAVE_H; ++y)
        for (int x = 0; x < CAVE_W; ++x)
            tileData[y * CAVE_W + x] =
                (x == 0 || y == 0 || x == CAVE_W - 1 || y == CAVE_H - 1) ? 5 : 1;

    Cave::Map map(&game);
    map.load();
    {
        auto p = game.getCaveProperties();
        map.generateMap(&p, tileData);
    }
    map.setEditorMode();

    HUD::Editor::Panel editorPanel(this);
    editorPanel.load();

    HUD::Editor::Toolbar toolbar(this);

    const sf::Vector2f viewSize   = sf::Vector2f(MAP_W, MAP_H);
    sf::Vector2f caveBounds = sf::Vector2f(CAVE_W * 32.f, CAVE_H * 32.f);
    Camera caveCamera(viewSize, sf::Vector2f(viewSize.x / 2.f, viewSize.y / 2.f), caveBounds);

    // Call after any generateMap that may change cave dimensions
    auto syncCaveBounds = [&]() {
        auto cp = game.getCaveProperties();
        caveBounds = sf::Vector2f(cp.width * 32.f, cp.height * 32.f);
        caveCamera.setBounds(caveBounds);
        caveCamera.setCentre(caveCamera.getCenter()); // force re-clamp to new bounds
    };

    const sf::Vector2f panelViewSize = sf::Vector2f(PANEL_W, PANEL_SCREEN_H);
    Camera panelCamera(panelViewSize, sf::Vector2f(PANEL_W / 2.f, PANEL_SCREEN_H / 2.f),
                       sf::Vector2f(PANEL_W, PANEL_SCREEN_H));

    sf::Texture sbTex;
    (void)sbTex.loadFromFile("./assets/textures/Editor/scroll_bar.png");

    const sf::IntRect sbRectArrUp   ({0,  0},  {16, 16});
    const sf::IntRect sbRectArrDown ({0,  16}, {16, 16});
    const sf::IntRect sbRectThumb   ({0,  32}, {16, 16});
    const sf::IntRect sbRectArrLeft ({16, 0},  {16, 16});
    const sf::IntRect sbRectArrRight({16, 16}, {16, 16});
    const sf::IntRect sbRectBg      ({16, 32}, {16, 16});

    sf::Sprite vsbArrUp   (sbTex, sbRectArrUp);    vsbArrUp   .setPosition({VSB_X, TOOLBAR_H});
    sf::Sprite vsbArrDown (sbTex, sbRectArrDown);   vsbArrDown .setPosition({VSB_X, TOOLBAR_H + MAP_H - SB_THICK});
    sf::Sprite hsbArrLeft (sbTex, sbRectArrLeft);   hsbArrLeft .setPosition({0.f,              HSB_Y});
    sf::Sprite hsbArrRight(sbTex, sbRectArrRight);  hsbArrRight.setPosition({MAP_W - SB_THICK, HSB_Y});

    sf::Sprite vsbThumb(sbTex, sbRectThumb);
    sf::Sprite hsbThumb(sbTex, sbRectThumb);

    sf::Sprite sbBgTile(sbTex, sbRectBg);

    sf::RectangleShape sbCorner(sf::Vector2f(SB_THICK, SB_THICK));
    sbCorner.setFillColor(sf::Color(212, 208, 200));
    sbCorner.setPosition(sf::Vector2f(VSB_X, HSB_Y));

    sf::RectangleShape vsbTrack(sf::Vector2f(SB_THICK, VSB_EFF_H));
    vsbTrack.setPosition(sf::Vector2f(VSB_X, VSB_EFF_Y));
    sf::RectangleShape hsbTrack(sf::Vector2f(HSB_EFF_W, SB_THICK));
    hsbTrack.setPosition(sf::Vector2f(HSB_EFF_X, HSB_Y));

    Cave::File   loadedFile;
    int          currentCaveIndex = 0;

    // Seed loadedFile with the initial blank cave so insert/delete always have an entry
    {
        Cave::Data initialCave;
        initialCave.properties = game.getCaveProperties();
        initialCave.tileData.resize(map.caveEntities.size());
        for (size_t i = 0; i < map.caveEntities.size(); ++i)
            initialCave.tileData[i] = entityTypeToTile(map.caveEntities[i].getType());
        loadedFile.caves.push_back(std::move(initialCave));
    }

    auto updateTitle = [&]()
    {
        int current = currentCaveIndex + 1;
        int total   = loadedFile.caves.empty() ? 1 : (int)loadedFile.caves.size();

        std::string filename = "Untitled";
        if (!m_savedFilePath.empty())
        {
            size_t sep = m_savedFilePath.find_last_of("\\/");
            filename = (sep != std::string::npos) ? m_savedFilePath.substr(sep + 1) : m_savedFilePath;
        }

        char buf[256];
        std::snprintf(buf, sizeof(buf), "Builder (Level %03d/%03d) - %s", current, total, filename.c_str());
        window.setTitle(buf);
    };
    updateTitle();
    Cave::Properties originalProps = game.getCaveProperties();
    int          lastPlacedX      = -1, lastPlacedY = -1;
    bool         panning          = false;

    bool         fillDragging      = false;
    sf::Vector2i fillStartTile     = { 0, 0 };
    sf::Vector2i fillCurrentTile   = { 0, 0 };
    sf::Vector2f fillStartWorld    = { 0.f, 0.f };
    sf::Vector2f fillCurrentWorld  = { 0.f, 0.f };

    // Returns all tile positions along a Bresenham line from a to b (inclusive).
    auto bresenham = [](sf::Vector2i a, sf::Vector2i b) {
        std::vector<sf::Vector2i> pts;
        int dx = std::abs(b.x - a.x), dy = std::abs(b.y - a.y);
        int sx = (a.x < b.x) ? 1 : -1, sy = (a.y < b.y) ? 1 : -1;
        int err = dx - dy;
        for (;;) {
            pts.push_back(a);
            if (a == b) break;
            int e2 = 2 * err;
            if (e2 > -dy) { err -= dy; a.x += sx; }
            if (e2 <  dx) { err += dx; a.y += sy; }
        }
        return pts;
    };

    sf::RectangleShape fillPreview;
    fillPreview.setFillColor(sf::Color::Transparent);
    fillPreview.setOutlineColor(sf::Color::Yellow);
    fillPreview.setOutlineThickness(2.f);
    sf::Vector2i lastPanPos;
    int          arrowCooldown    = 0;
    bool         vsbDragging      = false;
    bool         hsbDragging      = false;
    bool         minimapDragging  = false;
    bool         openTilePicker   = false;
    ImVec2       tilePickerPos    = { 0.f, 0.f };
    float        vsbDragStartY    = 0.f;
    float        hsbDragStartX    = 0.f;
    float        vsbDragStartCamY = 0.f;
    float        hsbDragStartCamX = 0.f;

    while (window.isOpen())
    {
        const float camMinX = viewSize.x / 2.f;
        const float camMaxX = caveBounds.x - viewSize.x / 2.f;
        const float camMinY = viewSize.y / 2.f;
        const float camMaxY = caveBounds.y - viewSize.y / 2.f;

        constexpr float vThumbH = SB_THICK;
        constexpr float hThumbW = SB_THICK;

        const float vFrac = (camMaxY > camMinY)
            ? std::clamp((caveCamera.getCenter().y - camMinY) / (camMaxY - camMinY), 0.f, 1.f) : 0.f;
        const float hFrac = (camMaxX > camMinX)
            ? std::clamp((caveCamera.getCenter().x - camMinX) / (camMaxX - camMinX), 0.f, 1.f) : 0.f;

        vsbThumb.setPosition({VSB_X,                                       VSB_EFF_Y + vFrac * (VSB_EFF_H - vThumbH)});
        hsbThumb.setPosition({HSB_EFF_X + hFrac * (HSB_EFF_W - hThumbW),  HSB_Y});

        while (const std::optional event = window.pollEvent())
        {
            ImGui::SFML::ProcessEvent(window, *event);

            if (event->is<sf::Event::Closed>())
                m_doExit = true;

            if (event->is<sf::Event::Resized>())
                window.setView(makeWindowView());

            if (const auto* e = event->getIf<sf::Event::KeyPressed>())
            {
#if defined(_WIN32)
                if (e->code == sf::Keyboard::Key::F11)
                    toggleFullscreen();
#endif
                handleShortcuts(*e, &editorPanel);
            }

            if (window.hasFocus()) if (const auto* e = event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (e->button == sf::Mouse::Button::Middle) { panning = true; lastPanPos = e->position; }
                if (e->button == sf::Mouse::Button::Right)
                {
                    sf::Vector2f vp = windowToVirtual(e->position);
                    bool overToolbar = vp.y < (int)TOOLBAR_H;
                    bool overPanel   = vp.x >= PANEL_X;
                    bool overVsb     = vp.x >= VSB_X;
                    bool overHsb     = vp.y >= HSB_Y;
                    if (!ImGui::GetIO().WantCaptureMouse && !overToolbar && !overPanel && !overVsb && !overHsb)
                    {
                        tilePickerPos = ImVec2((float)e->position.x, (float)e->position.y);
                        openTilePicker = true;
                    }
                }
                if (e->button == sf::Mouse::Button::Left)
                {
                    sf::Vector2f vp = windowToVirtual(e->position);

                    if (vsbThumb.getGlobalBounds().contains(vp))
                    {
                        vsbDragging = true;
                        vsbDragStartY    = vp.y;
                        vsbDragStartCamY = caveCamera.getCenter().y;
                    }
                    else if (hsbThumb.getGlobalBounds().contains(vp))
                    {
                        hsbDragging = true;
                        hsbDragStartX    = vp.x;
                        hsbDragStartCamX = caveCamera.getCenter().x;
                    }
                    else if (vsbTrack.getGlobalBounds().contains(vp))
                    {
                        float frac = std::clamp((vp.y - VSB_EFF_Y - vThumbH * 0.5f) / (VSB_EFF_H - vThumbH), 0.f, 1.f);
                        float ny   = std::round((camMinY + frac * (camMaxY - camMinY)) / 32.f) * 32.f;
                        caveCamera.setCentre({caveCamera.getCenter().x, ny});
                    }
                    else if (hsbTrack.getGlobalBounds().contains(vp))
                    {
                        float frac = std::clamp((vp.x - HSB_EFF_X - hThumbW * 0.5f) / (HSB_EFF_W - hThumbW), 0.f, 1.f);
                        float nx   = std::round((camMinX + frac * (camMaxX - camMinX)) / 32.f) * 32.f;
                        caveCamera.setCentre({nx, caveCamera.getCenter().y});
                    }
                    else if (vsbArrUp  .getGlobalBounds().contains(vp))
                        caveCamera.setCentre({caveCamera.getCenter().x, caveCamera.getCenter().y - 32.f});
                    else if (vsbArrDown.getGlobalBounds().contains(vp))
                        caveCamera.setCentre({caveCamera.getCenter().x, caveCamera.getCenter().y + 32.f});
                    else if (hsbArrLeft .getGlobalBounds().contains(vp))
                        caveCamera.setCentre({caveCamera.getCenter().x - 32.f, caveCamera.getCenter().y});
                    else if (hsbArrRight.getGlobalBounds().contains(vp))
                        caveCamera.setCentre({caveCamera.getCenter().x + 32.f, caveCamera.getCenter().y});
                    else if (vp.x >= PANEL_X)
                    {
                        sf::Vector2f cavePos;
                        if (editorPanel.getMinimapCavePos(vp, PANEL_X, TOOLBAR_H, cavePos))
                        {
                            caveCamera.setCentre(cavePos);
                            minimapDragging = true;
                        }
                        else
                            editorPanel.handleClick(vp, PANEL_X, TOOLBAR_H);
                    }
                    else if (!ImGui::GetIO().WantCaptureMouse &&
                             vp.x < VSB_X && vp.y >= TOOLBAR_H && vp.y < HSB_Y &&
                             ((settings.fillMode == Cave::FillMode::Rectangle && editorPanel.getFillSelected() != 0) ||
                              (settings.fillMode != Cave::FillMode::Rectangle && editorPanel.getFillSelected() >= 1)))
                    {
                        sf::View caveView = caveCamera.view();
                        caveView.setViewport(sf::FloatRect(sf::Vector2f(CAVE_VP_X, CAVE_VP_Y),
                                                           sf::Vector2f(CAVE_VP_W, CAVE_VP_H)));
                        sf::Vector2f mw = rt.mapPixelToCoords(sf::Vector2i((int)vp.x, (int)vp.y), caveView);
                        int tx = std::clamp((int)mw.x / 32, 1, map.width  - 2);
                        int ty = std::clamp((int)mw.y / 32, 1, map.height - 2);
                        saveUndoSnapshot(map);
                        fillDragging     = true;
                        fillStartTile    = { tx, ty };
                        fillCurrentTile  = { tx, ty };
                        fillStartWorld   = mw;
                        fillCurrentWorld = mw;
                    }
                }
            }
            if (window.hasFocus()) if (const auto* e = event->getIf<sf::Event::MouseButtonReleased>())
            {
                if (e->button == sf::Mouse::Button::Middle) panning = false;
                if (e->button == sf::Mouse::Button::Left)
                {
                    vsbDragging = false; hsbDragging = false; minimapDragging = false; editorPanel.handleRelease();

                    if (fillDragging)
                    {
                        fillDragging = false;
                        int fillMode = editorPanel.getFillSelected();
                        Cave::Entity::Type type = editorPanel.getSelectedType();
                        if (type == Cave::Entity::Type::StartDoor ||
                            type == Cave::Entity::Type::ExitDoor) break;
                        auto placeSelected = [&](int index) {
                            Cave::Entity::Base e = editorPanel.getSelectedEntity();
                            map.placeEntity(index, e);
                        };

                        if (settings.fillMode == Cave::FillMode::Line && fillMode == 1)
                        {
                            // Rubber-band Bresenham line on release
                            for (auto& pt : bresenham(fillStartTile, fillCurrentTile))
                                placeSelected(pt.y * map.width + pt.x);
                        }
                        else if (settings.fillMode == Cave::FillMode::Line && fillMode == 2)
                        {
                            for (auto& pt : lineFillRight(fillStartTile, fillCurrentTile))
                                placeSelected(pt.y * map.width + pt.x);
                        }
                        else if (settings.fillMode == Cave::FillMode::Ellipse && (fillMode == 1 || fillMode == 2))
                        {
                            int x0 = std::min(fillStartTile.x, fillCurrentTile.x);
                            int x1 = std::max(fillStartTile.x, fillCurrentTile.x);
                            int y0 = std::min(fillStartTile.y, fillCurrentTile.y);
                            int y1 = std::max(fillStartTile.y, fillCurrentTile.y);
                            float cx = (x0 + x1 + 1) * 0.5f;
                            float cy = (y0 + y1 + 1) * 0.5f;
                            float rx = (x1 - x0 + 1) * 0.5f;
                            float ry = (y1 - y0 + 1) * 0.5f;
                            if (rx > 0.f && ry > 0.f)
                            {
                                for (int ty = y0; ty <= y1; ++ty)
                                    for (int tx = x0; tx <= x1; ++tx)
                                    {
                                        float dx = ((tx + 0.5f) - cx) / rx;
                                        float dy = ((ty + 0.5f) - cy) / ry;
                                        float val = dx * dx + dy * dy;
                                        if (fillMode == 2)
                                        {
                                            if (val <= 1.f)
                                                placeSelected(ty * map.width + tx);
                                        }
                                        else
                                        {
                                            if (val > 1.f) continue;
                                            float dxR = ((tx + 1.5f) - cx) / rx;
                                            float dxL = ((tx - 0.5f) - cx) / rx;
                                            float dyD = ((ty + 1.5f) - cy) / ry;
                                            float dyU = ((ty - 0.5f) - cy) / ry;
                                            bool neighbourOut =
                                                dxR * dxR + dy  * dy  > 1.f ||
                                                dxL * dxL + dy  * dy  > 1.f ||
                                                dx  * dx  + dyD * dyD > 1.f ||
                                                dx  * dx  + dyU * dyU > 1.f;
                                            if (neighbourOut)
                                                placeSelected(ty * map.width + tx);
                                        }
                                    }
                            }
                        }
                        else
                        {
                            int x0 = std::min(fillStartTile.x, fillCurrentTile.x);
                            int x1 = std::max(fillStartTile.x, fillCurrentTile.x);
                            int y0 = std::min(fillStartTile.y, fillCurrentTile.y);
                            int y1 = std::max(fillStartTile.y, fillCurrentTile.y);
                            for (int ty = y0; ty <= y1; ++ty)
                                for (int tx = x0; tx <= x1; ++tx)
                                {
                                    bool onBorder = (tx == x0 || tx == x1 || ty == y0 || ty == y1);
                                    if (fillMode == 1 && !onBorder) continue;
                                    placeSelected(ty * map.width + tx);
                                }
                        }
                    }
                }
            }
            if (window.hasFocus()) if (const auto* e = event->getIf<sf::Event::MouseMoved>())
            {
                sf::Vector2f vp = windowToVirtual(e->position);

                if (panning)
                {
                    sf::Vector2f prevVp = windowToVirtual(lastPanPos);
                    caveCamera.setCentre(caveCamera.getCenter() - (vp - prevVp));
                    lastPanPos = e->position;
                }
                if (fillDragging)
                {
                    sf::View caveView = caveCamera.view();
                    caveView.setViewport(sf::FloatRect(sf::Vector2f(CAVE_VP_X, CAVE_VP_Y),
                                                       sf::Vector2f(CAVE_VP_W, CAVE_VP_H)));
                    sf::Vector2f mw = rt.mapPixelToCoords(sf::Vector2i((int)vp.x, (int)vp.y), caveView);
                    fillCurrentTile  = {
                        std::clamp((int)mw.x / 32, 1, map.width  - 2),
                        std::clamp((int)mw.y / 32, 1, map.height - 2)
                    };
                    fillCurrentWorld = mw;
                }
                if (minimapDragging)
                {
                    sf::Vector2f cavePos;
                    if (editorPanel.getMinimapCavePos(vp, PANEL_X, TOOLBAR_H, cavePos))
                        caveCamera.setCentre(cavePos);
                }
                if (vsbDragging)
                {
                    float delta    = vp.y - vsbDragStartY;
                    float camRange = camMaxY - camMinY;
                    float ny = std::round((vsbDragStartCamY + delta / (VSB_EFF_H - vThumbH) * camRange) / 32.f) * 32.f;
                    caveCamera.setCentre({caveCamera.getCenter().x, ny});
                }
                if (hsbDragging)
                {
                    float delta    = vp.x - hsbDragStartX;
                    float camRange = camMaxX - camMinX;
                    float nx = std::round((hsbDragStartCamX + delta / (HSB_EFF_W - hThumbW) * camRange) / 32.f) * 32.f;
                    caveCamera.setCentre({nx, caveCamera.getCenter().y});
                }
            }
        }

        {
            const float scale = static_cast<float>(window.getSize().y) / static_cast<float>(WIN_H);
            ImGui::GetIO().FontGlobalScale     = scale;
            ImGui::GetStyle().FramePadding     = ImVec2(4.f * scale, 3.5f * scale);
            ImGui::GetStyle().ItemSpacing      = ImVec2(8.f * scale, 4.f * scale);
            ImGui::GetStyle().Colors[ImGuiCol_MenuBarBg] = ImVec4(212/255.f, 208/255.f, 200/255.f, 1.f);
            ImGui::GetStyle().Colors[ImGuiCol_PopupBg]  = ImVec4(212/255.f, 208/255.f, 200/255.f, 1.f);
            ImGui::GetStyle().Colors[ImGuiCol_Text]         = ImVec4(0.f, 0.f, 0.f, 1.f);
            ImGui::GetStyle().Colors[ImGuiCol_TextDisabled] = ImVec4(0.f, 0.f, 0.f, 1.f);
            ImGui::GetStyle().Colors[ImGuiCol_Separator]= ImVec4(0.5f, 0.5f, 0.5f, 1.f);
            ImGui::GetStyle().Colors[ImGuiCol_Border]   = ImVec4(0.f, 0.f, 0.f, 1.f);
            ImGui::GetStyle().PopupBorderSize           = 1.f;
        }
        ImGui::SFML::Update(window, deltaClock.restart());

        if (arrowCooldown > 0) --arrowCooldown;
        {
            bool L = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left);
            bool R = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right);
            bool U = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up);
            bool D = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down);

            if ((L || R || U || D) && arrowCooldown == 0)
            {
                sf::Vector2f c = caveCamera.getCenter();
                if (L) c.x -= 32.f;
                if (R) c.x += 32.f;
                if (U) c.y -= 32.f;
                if (D) c.y += 32.f;
                caveCamera.setCentre(c);
                arrowCooldown = 8;
            }
        }

        toolbar.draw(&editorPanel, m_developerMode);

        // Right-click tile picker popup
        {
            if (openTilePicker)
            {
                ImGui::SetNextWindowPos(tilePickerPos, ImGuiCond_Always, ImVec2(0.f, 0.f));
                ImGui::OpenPopup("##tools_ctx");
                openTilePicker = false;
            }
            toolbar.drawToolsContextPopup(&editorPanel);
        }

        // Apply zoom mode — double view size shows twice as many tiles
        {
            sf::Vector2f targetSize = m_smallBlocks
                ? sf::Vector2f(MAP_W * 2.f, MAP_H * 2.f)
                : sf::Vector2f(MAP_W,        MAP_H);
            if (caveCamera.getSize() != targetSize)
                caveCamera.setSize(targetSize);
        }

        {
            sf::Vector2f vpf  = windowToVirtual(sf::Mouse::getPosition(window));
            sf::Vector2i rtPx = sf::Vector2i((int)vpf.x, (int)vpf.y);

            bool overToolbar = rtPx.y < (int)TOOLBAR_H;
            bool overVsb     = rtPx.x >= (int)VSB_X;
            bool overHsb     = rtPx.y >= (int)HSB_Y;

            if (!ImGui::GetIO().WantCaptureMouse && !overToolbar && !overVsb && !overHsb
                && !vsbDragging && !hsbDragging)
            {
                sf::View caveView = caveCamera.view();
                caveView.setViewport(sf::FloatRect(sf::Vector2f(CAVE_VP_X, CAVE_VP_Y),
                                                   sf::Vector2f(CAVE_VP_W, CAVE_VP_H)));
                sf::Vector2f mw = rt.mapPixelToCoords(rtPx, caveView);

                editorPanel.setMouseTile(
                    std::clamp((int)mw.x / 32, 0, map.width  - 1),
                    std::clamp((int)mw.y / 32, 0, map.height - 1));

                if (window.hasFocus() && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) &&
                    editorPanel.getFillSelected() == 0)
                {
                    int tileX = (int)mw.x / 32;
                    int tileY = (int)mw.y / 32;
                    if (tileX >= 1 && tileX < map.width - 1 &&
                        tileY >= 1 && tileY < map.height - 1 &&
                        (tileX != lastPlacedX || tileY != lastPlacedY))
                    {
                        if (lastPlacedX == -1) saveUndoSnapshot(map);
                        Cave::Entity::Type type = editorPanel.getSelectedType();

                        bool canPlace = true;
                        if (type == Cave::Entity::Type::StartDoor ||
                            type == Cave::Entity::Type::ExitDoor)
                        {
                            int existing = 0;
                            for (const auto& e : map.caveEntities)
                                if (e.getType() == type) ++existing;
                            if (existing >= 1) canPlace = false;
                        }

                        if (canPlace)
                        {
                            Cave::Entity::Base e = editorPanel.getSelectedEntity();
                            map.placeEntity(tileY * map.width + tileX, e);
                            lastPlacedX = tileX;
                            lastPlacedY = tileY;
                        }
                    }
                }
                else { lastPlacedX = -1; lastPlacedY = -1; }
            }
        }

        if (settings.animation) map.update(caveCamera);
        else                    map.updateVisibleTiles(caveCamera);
        editorPanel.update(panelCamera, &caveCamera, &map, settings.animation, settings.fillMode);
        Utils::incrementGlobalCounter();

        shaderManager.update();

        sf::View caveRenderView = caveCamera.view();
        caveRenderView.setViewport(sf::FloatRect(sf::Vector2f(CAVE_VP_X, CAVE_VP_Y),
                                                  sf::Vector2f(CAVE_VP_W, CAVE_VP_H)));
        rt.setView(caveRenderView);
        rt.clear(sf::Color(20, 20, 20));
        rt.draw(map, shaderManager.currentShader());

        if (fillDragging)
        {
            int fillMode = editorPanel.getFillSelected();
            if (settings.fillMode == Cave::FillMode::Line && (fillMode == 1 || fillMode == 2))
            {
                sf::Vector2f a = fillStartWorld;
                sf::Vector2f b = fillCurrentWorld;
                float len = std::hypot(b.x - a.x, b.y - a.y);
                float angle = std::atan2(b.y - a.y, b.x - a.x) * 180.f / 3.14159265f;
                sf::RectangleShape lineShape({len, 2.f});
                lineShape.setPosition(a);
                lineShape.setRotation(sf::degrees(angle));
                lineShape.setFillColor(sf::Color::Yellow);
                rt.draw(lineShape);
            }
            else if (settings.fillMode == Cave::FillMode::Ellipse && (fillMode == 1 || fillMode == 2))
            {
                float wx0 = std::min(fillStartWorld.x, fillCurrentWorld.x);
                float wy0 = std::min(fillStartWorld.y, fillCurrentWorld.y);
                float wx1 = std::max(fillStartWorld.x, fillCurrentWorld.x);
                float wy1 = std::max(fillStartWorld.y, fillCurrentWorld.y);
                float rx = (wx1 - wx0) * 0.5f;
                float ry = (wy1 - wy0) * 0.5f;
                if (rx > 0.f && ry > 0.f)
                {
                    constexpr int N = 64;
                    float cx = (wx0 + wx1) * 0.5f;
                    float cy = (wy0 + wy1) * 0.5f;
                    for (int i = 0; i < N; ++i)
                    {
                        float t0 = i       * 2.f * 3.14159265f / N;
                        float t1 = (i + 1) * 2.f * 3.14159265f / N;
                        sf::Vector2f a(cx + rx * std::cos(t0), cy + ry * std::sin(t0));
                        sf::Vector2f b(cx + rx * std::cos(t1), cy + ry * std::sin(t1));
                        float len   = std::hypot(b.x - a.x, b.y - a.y);
                        float angle = std::atan2(b.y - a.y, b.x - a.x) * 180.f / 3.14159265f;
                        sf::RectangleShape seg({len, 2.f});
                        seg.setPosition(a);
                        seg.setRotation(sf::degrees(angle));
                        seg.setFillColor(sf::Color::Yellow);
                        rt.draw(seg);
                    }
                }
            }
            else
            {
                float wx0 = std::min(fillStartWorld.x, fillCurrentWorld.x);
                float wy0 = std::min(fillStartWorld.y, fillCurrentWorld.y);
                float wx1 = std::max(fillStartWorld.x, fillCurrentWorld.x);
                float wy1 = std::max(fillStartWorld.y, fillCurrentWorld.y);
                fillPreview.setPosition(sf::Vector2f(wx0, wy0));
                fillPreview.setSize(sf::Vector2f(wx1 - wx0, wy1 - wy0));
                fillPreview.setFillColor(sf::Color::Transparent);
                rt.draw(fillPreview);
            }
        }

        sf::View panelRenderView = panelCamera.view();
        panelRenderView.setViewport(sf::FloatRect(sf::Vector2f(PANEL_VP_X, PANEL_VP_Y),
                                                   sf::Vector2f(PANEL_VP_W, PANEL_VP_H)));
        rt.setView(panelRenderView);
        rt.draw(editorPanel, shaderManager.currentShader());

        rt.setView(rt.getDefaultView());
        for (float y = VSB_EFF_Y; y < VSB_EFF_Y + VSB_EFF_H; y += SB_THICK)
        {
            sbBgTile.setPosition({VSB_X, y});
            rt.draw(sbBgTile);
        }
        for (float x = HSB_EFF_X; x < HSB_EFF_X + HSB_EFF_W; x += SB_THICK)
        {
            sbBgTile.setPosition({x, HSB_Y});
            rt.draw(sbBgTile);
        }
        rt.draw(sbCorner);
        rt.draw(vsbArrUp);   rt.draw(vsbArrDown);
        rt.draw(hsbArrLeft); rt.draw(hsbArrRight);
        rt.draw(vsbThumb);   rt.draw(hsbThumb);

        rt.display();

        if (m_doNewFile)
        {
            m_doNewFile = false;
            if (!checkUnsavedChanges(map, loadedFile, currentCaveIndex)) goto endFrame;
            loadedFile = Cave::File{};
            m_savedFilePath.clear();
            currentCaveIndex = 0;

            Cave::Data blankCave;
            blankCave.properties = defaultCaveProperties();
            const Cave::Properties originalProps = blankCave.properties;
            int sz = originalProps.width * originalProps.height;
            blankCave.tileData.assign(sz, 1);
            for (int y = 0; y < (int)originalProps.height; ++y)
                for (int x = 0; x < (int)originalProps.width; ++x)
                    if (x == 0 || y == 0 || x == (int)originalProps.width - 1 || y == (int)originalProps.height - 1)
                        blankCave.tileData[y * originalProps.width + x] = 5;
            loadedFile.caves.push_back(std::move(blankCave));

            game.setCaveProperties(originalProps);
            auto p = game.getCaveProperties();
            map.generateMap(&p, loadedFile.caves[0].tileData);
            map.setEditorMode();
            syncCaveBounds();
            m_undoTileData.clear();
            m_isDirty       = false;
            m_undoAvailable = false;
            caveCamera.setCentre(sf::Vector2f(viewSize.x / 2.f, viewSize.y / 2.f));
            lastPlacedX = -1; lastPlacedY = -1;
            updateTitle();
        }

        if (m_doOpenFile)
        {
            m_doOpenFile = false;
            if (!checkUnsavedChanges(map, loadedFile, currentCaveIndex)) goto endFrame;

            const char* filterPatterns[] = {"*.cav"};
            std::string openDefaultPath = (std::filesystem::path(getCavesDir()) / "").string();
            const char* openResult = tinyfd_openFileDialog("Open Cave File", openDefaultPath.c_str(), 1, filterPatterns, "Cave Files", 0);
            if (openResult)
            {
                try
                {
                    std::string fullPath(openResult);
                    size_t sep = fullPath.find_last_of("/\\");
                    std::string dir  = (sep != std::string::npos) ? fullPath.substr(0, sep + 1) : "";
                    std::string name = (sep != std::string::npos) ? fullPath.substr(sep + 1)    : fullPath;

                    loadedFile       = Cave::File::loadFromFile(dir, name);
                    m_savedFilePath  = fullPath;
                    m_isDirty        = false;
                    currentCaveIndex = 0;

                    if (!loadedFile.caves.empty())
                    {
                        const Cave::Data& caveData = loadedFile.caves[currentCaveIndex];
                        game.setCaveProperties(caveData.properties);
                        originalProps = caveData.properties;
                        auto p = game.getCaveProperties();
                        map.generateMap(&p, caveData.tileData);
                        map.setEditorMode();
                        syncCaveBounds();
                        caveCamera.setCentre(sf::Vector2f(viewSize.x / 2.f, viewSize.y / 2.f));
                        lastPlacedX = -1; lastPlacedY = -1;
                        updateTitle();
                    }
                }
                catch (const std::exception& e)
                {
                    tinyfd_messageBox("Error loading cave file", e.what(), "ok", "error", 1);
                }
            }
        }

        {
            int delta = m_doNextLevel ? 1 : m_doPrevLevel ? -1 : 0;
            m_doNextLevel = m_doPrevLevel = false;
            if (delta != 0 && !loadedFile.caves.empty())
            {
                int n = (int)loadedFile.caves.size();
                int newIndex = std::clamp(currentCaveIndex + delta, 0, n - 1);
                if (newIndex != currentCaveIndex)
                {
                    syncCurrentCave(map, loadedFile, currentCaveIndex);
                    m_undoTileData.clear();
                    currentCaveIndex = newIndex;
                    const Cave::Data& caveData = loadedFile.caves[currentCaveIndex];
                    game.setCaveProperties(caveData.properties);
                    originalProps = caveData.properties;
                    auto p = game.getCaveProperties();
                    map.generateMap(&p, caveData.tileData);
                    map.setEditorMode();
                    syncCaveBounds();
                    caveCamera.setCentre(sf::Vector2f(viewSize.x / 2.f, viewSize.y / 2.f));
                    lastPlacedX = -1; lastPlacedY = -1;
                    updateTitle();
                }
            }
        }

        if (m_doShowCaveProps)
        {
            m_doShowCaveProps = false;
            bool trigger = false;
            Cave::Properties p = game.getCaveProperties();
            Cave::Properties oldP = p;
            int diamondCount = (int)std::count_if(map.caveEntities.begin(), map.caveEntities.end(),
                [](const Cave::Entity::Base& e) {
                    return e.getType() == Cave::Entity::Type::Diamond ||
                           e.getType() == Cave::Entity::Type::FragileDiamond;
                });
            Cave::editCaveProperties(window, p, p, diamondCount, trigger, m_developerMode, [&]()
            {
                game.setCaveProperties(p);
                shaderManager.update();

                rt.setView(rt.getDefaultView());
                rt.clear(sf::Color(20, 20, 20));

                // Draw toolbar background so it doesn't disappear (ImGui not running)
                sf::RectangleShape toolbarBg(sf::Vector2f((float)WIN_W, TOOLBAR_H));
                toolbarBg.setFillColor(sf::Color(212, 208, 200));
                rt.draw(toolbarBg);

                sf::View caveRV = caveCamera.view();
                caveRV.setViewport(sf::FloatRect(sf::Vector2f(CAVE_VP_X, CAVE_VP_Y),
                                                  sf::Vector2f(CAVE_VP_W, CAVE_VP_H)));
                rt.setView(caveRV);
                rt.draw(map, shaderManager.currentShader());

                sf::View panelRV = panelCamera.view();
                panelRV.setViewport(sf::FloatRect(sf::Vector2f(PANEL_VP_X, PANEL_VP_Y),
                                                   sf::Vector2f(PANEL_VP_W, PANEL_VP_H)));
                rt.setView(panelRV);
                rt.draw(editorPanel, shaderManager.currentShader());

                rt.setView(rt.getDefaultView());
                for (float y2 = VSB_EFF_Y; y2 < VSB_EFF_Y + VSB_EFF_H; y2 += SB_THICK)
                    { sbBgTile.setPosition({VSB_X, y2}); rt.draw(sbBgTile); }
                for (float x2 = HSB_EFF_X; x2 < HSB_EFF_X + HSB_EFF_W; x2 += SB_THICK)
                    { sbBgTile.setPosition({x2, HSB_Y}); rt.draw(sbBgTile); }
                rt.draw(sbCorner);
                rt.draw(vsbArrUp);   rt.draw(vsbArrDown);
                rt.draw(hsbArrLeft); rt.draw(hsbArrRight);
                rt.draw(vsbThumb);   rt.draw(hsbThumb);
                rt.display();

                sf::Sprite rtSpr(rt.getTexture());
                window.setView(makeWindowView());
                window.clear(sf::Color::Black);
                window.draw(rtSpr);
                window.display();
            });
            {
                game.setCaveProperties(p);
                //if (p.width != oldP.width || p.height != oldP.height)
                //{
                //    // Snapshot current tiles
                //    std::vector<char> oldTiles(map.caveEntities.size());
                //    for (size_t i = 0; i < map.caveEntities.size(); ++i)
                //        oldTiles[i] = entityTypeToTile(map.caveEntities[i].getType());

                //    int oW = (int)oldP.width,  oH = (int)oldP.height;
                //    int nW = (int)p.width,      nH = (int)p.height;

                //    // Inner dims (inside the solid-wall border)
                //    int oIW = oW - 2, oIH = oH - 2;
                //    int nIW = nW - 2, nIH = nH - 2;

                //    // Anchor offset: where old inner top-left lands in new inner space
                //    int anchorIdx = (int)settings.resizeAnchor;
                //    int anchorCol = anchorIdx % 3; // 0=left, 1=centre, 2=right
                //    int anchorRow = anchorIdx / 3; // 0=top,  1=middle, 2=bottom

                //    int ox = (anchorCol == 0) ? 0
                //           : (anchorCol == 1) ? (nIW - oIW) / 2
                //                              : (nIW - oIW);
                //    int oy = (anchorRow == 0) ? 0
                //           : (anchorRow == 1) ? (nIH - oIH) / 2
                //                              : (nIH - oIH);

                //    // Build new tile grid: border = solid wall (5), interior = dirt (1) default
                //    std::vector<char> newTiles((size_t)nW * (size_t)nH, 1);

                //    // Border
                //    for (int x = 0; x < nW; ++x) {
                //        newTiles[0       * nW + x] = 5;
                //        newTiles[(nH-1)  * nW + x] = 5;
                //    }
                //    for (int y = 0; y < nH; ++y) {
                //        newTiles[y * nW + 0]      = 5;
                //        newTiles[y * nW + (nW-1)] = 5;
                //    }

                //    // Copy old inner content into new inner region using anchor offset
                //    for (int iy = 0; iy < nIH; ++iy) {
                //        for (int ix = 0; ix < nIW; ++ix) {
                //            int srcX = ix - ox;
                //            int srcY = iy - oy;
                //            if (srcX >= 0 && srcX < oIW && srcY >= 0 && srcY < oIH)
                //                newTiles[(iy+1) * nW + (ix+1)] = oldTiles[(srcY+1) * oW + (srcX+1)];
                //        }
                //    }

                //    map.generateMap(&p, newTiles);
                //    map.setEditorMode();
                //}
                if (p.width != oldP.width || p.height != oldP.height)
                {
                    std::vector<char> newTiles(p.width * p.height, 1);
                    for (int y = 0; y < p.height; y++) {
                        for (int x = 0; x < p.width; x++) {
                            if (x == 0 || x == p.width - 1 || y == 0 || y == p.height - 1) {
                                newTiles[y * p.width + x] = entityTypeToTile(Cave::Entity::SolidWall().getType());
                            }
                            else {
                                newTiles[y * p.width + x] = entityTypeToTile(Cave::Entity::Dirt().getType());
                            }
                        }
                    }

                    int anchorIdx = (int)settings.resizeAnchor;
                    int anchorCol = anchorIdx % 3; // 0=left, 1=centre, 2=right
                    int anchorRow = anchorIdx / 3; // 0=top,  1=middle, 2=bottom

                    int oIW = (int)oldP.width - 2, oIH = (int)oldP.height - 2;
                    int nIW = (int)p.width    - 2, nIH = (int)p.height    - 2;
                    int copyW = std::min(oIW, nIW);
                    int copyH = std::min(oIH, nIH);

                    // Source start: which corner of the old inner region to read from
                    int srcX0 = (anchorCol == 0) ? 0 : (anchorCol == 1) ? std::max(0, (oIW - nIW) / 2) : std::max(0, oIW - nIW);
                    int srcY0 = (anchorRow == 0) ? 0 : (anchorRow == 1) ? std::max(0, (oIH - nIH) / 2) : std::max(0, oIH - nIH);

                    // Destination start: which corner of the new inner region to write to
                    int dstX0 = (anchorCol == 0) ? 0 : (anchorCol == 1) ? std::max(0, (nIW - oIW) / 2) : std::max(0, nIW - oIW);
                    int dstY0 = (anchorRow == 0) ? 0 : (anchorRow == 1) ? std::max(0, (nIH - oIH) / 2) : std::max(0, nIH - oIH);

                    for (int cy = 0; cy < copyH; cy++) {
                        for (int cx = 0; cx < copyW; cx++) {
                            int srcIdx = (srcY0 + cy + 1) * (int)oldP.width + (srcX0 + cx + 1);
                            int dstIdx = (dstY0 + cy + 1) * (int)p.width    + (dstX0 + cx + 1);
                            newTiles[dstIdx] = entityTypeToTile(map.caveEntities[srcIdx].getType());
                        }
                    }

                    map.generateMap(&p, newTiles);
                    map.setEditorMode();
                    syncCaveBounds();
                }
            }
        }

        if (m_doShowSettings)
        {
            m_doShowSettings = false;
            bool trigger = false;
            Cave::editCaveSettings(window, settings, trigger, m_developerMode);
        }

        if (m_doUndo)
        {
            m_doUndo = false;
            if (m_undoAvailable && !m_undoTileData.empty())
            {
                // Snapshot current state so a re-edit can undo again
                std::vector<char> current(map.caveEntities.size());
                for (size_t i = 0; i < map.caveEntities.size(); ++i)
                    current[i] = entityTypeToTile(map.caveEntities[i].getType());

                // Replace only changed cells so unaffected entities keep their animation frame
                for (size_t i = 0; i < map.caveEntities.size(); ++i)
                {
                    if (m_undoTileData[i] != current[i])
                    {
                        Cave::Entity::Base e = Cave::Data::getTileEntity(m_undoTileData[i]);
                        map.placeEntity((int)i, e);
                    }
                }
                map.setEditorMode();
                m_undoTileData  = std::move(current);
                m_undoAvailable = false;
                lastPlacedX = -1; lastPlacedY = -1;
            }
        }
        if (m_doClearLevel)   { m_doClearLevel   = false; saveUndoSnapshot(map); clearLevel(map); syncCaveBounds(); }
        if (m_doCopyLevel)    { m_doCopyLevel    = false; copyLevel(map);  }
        if (m_doPasteLevel)   { m_doPasteLevel   = false; saveUndoSnapshot(map); pasteLevel(map); syncCaveBounds(); }
        if (m_doInsertLevel)  { m_doInsertLevel  = false; insertLevel(map, loadedFile, currentCaveIndex); updateTitle(); syncCaveBounds(); }
        if (m_doDeleteLevel)  { m_doDeleteLevel  = false; deleteLevel(map, loadedFile, currentCaveIndex); updateTitle(); syncCaveBounds(); }
        if (m_doSaveFile)     { m_doSaveFile     = false; saveFile(map, loadedFile, currentCaveIndex);    updateTitle(); }
        if (m_doSaveFileAs)   { m_doSaveFileAs   = false; saveFileAs(map, loadedFile, currentCaveIndex); updateTitle(); }
        if (m_doTest)
        {
            m_doTest = false;
            saveFile(map, loadedFile, currentCaveIndex);
            updateTitle();

            if (!m_savedFilePath.empty())
            {
                size_t sep = m_savedFilePath.find_last_of("/\\");
                std::string caveFilename = (sep != std::string::npos)
                    ? m_savedFilePath.substr(sep + 1)
                    : m_savedFilePath;

                std::string args =
                    " --editor-mode"
                    " --cave=\"" + caveFilename + "\""
                    " --start=" + std::to_string(currentCaveIndex + 1) +
                    (m_developerMode ? " --developer" : "");

#ifdef _WIN32
                {
                    char exePath[MAX_PATH] = {};
                    GetModuleFileNameA(nullptr, exePath, MAX_PATH);
                    std::string exeDir(exePath);
                    size_t lastSep = exeDir.find_last_of("\\/");
                    if (lastSep != std::string::npos) exeDir = exeDir.substr(0, lastSep + 1);
                    std::string gamePath = exeDir + "DiggingJim.exe";

                    if (IsZoomed(window.getNativeHandle()))
                        args += " --fullscreen";

                    std::string cmdLine = "DiggingJim.exe" + args;
                    std::vector<char> cmdBuf(cmdLine.begin(), cmdLine.end());
                    cmdBuf.push_back('\0');

                    STARTUPINFOA si = {};
                    si.cb = sizeof(si);
                    PROCESS_INFORMATION pi = {};

                    if (CreateProcessA(gamePath.c_str(), cmdBuf.data(), nullptr, nullptr, FALSE, 0, nullptr, exeDir.c_str(), &si, &pi))
                    {
                        window.setVisible(false);
                        WaitForSingleObject(pi.hProcess, INFINITE);
                        CloseHandle(pi.hProcess);
                        CloseHandle(pi.hThread);
                        window.setVisible(true);
                    }
                    else
                    {
                        std::string errMsg = "Could not launch DiggingJim.\nLooked for: " + gamePath;
                        tinyfd_messageBox("Test Error", errMsg.c_str(), "ok", "error", 1);
                    }
                }
#elif defined(__APPLE__) || defined(__linux__)
                {
                    // Use fork/exec to pass arguments directly — avoids shell quoting
                    // issues with spaces in filenames that std::system() cannot handle.
                    std::string caveArg  = "--cave=" + caveFilename;
                    std::string startArg = "--start=" + std::to_string(currentCaveIndex + 1);
                    std::vector<const char*> argvVec = {
                        "./DiggingJim",
                        "--editor-mode",
                        caveArg.c_str(),
                        startArg.c_str(),
                    };
                    if (m_developerMode) argvVec.push_back("--developer");
                    argvVec.push_back(nullptr);
                    const char** argv = argvVec.data();
                    pid_t pid = fork();
                    if (pid == 0)
                    {
                        execvp("./DiggingJim", const_cast<char* const*>(argv));
                        _exit(1);
                    }
                    else if (pid > 0)
                    {
                        window.setVisible(false);
                        int status;
                        waitpid(pid, &status, 0);
                        window.setVisible(true);
                    }
                }
#endif
            }
        }
        if (m_doExit)         { m_doExit = false; if (checkUnsavedChanges(map, loadedFile, currentCaveIndex)) window.close(); }
        if (m_doRandomDist)
        {
            m_doRandomDist = false;
            Cave::Entity::Base entity = editorPanel.getSelectedEntity();
            auto t = entity.getType();
            if (t != Cave::Entity::Type::StartDoor && t != Cave::Entity::Type::ExitDoor)
            {
                saveUndoSnapshot(map);
                std::mt19937 rng(std::random_device{}());
                std::uniform_int_distribution<int> distX(1, map.width  - 2);
                std::uniform_int_distribution<int> distY(1, map.height - 2);
                for (uint32_t i = 0; i < settings.randomDistBlocks; ++i)
                {
                    int tx = distX(rng);
                    int ty = distY(rng);
                    map.placeEntity(ty * map.width + tx, entity);
                }
            }
        }

        endFrame:
        {
            sf::Sprite rtSprite(rt.getTexture());
            window.setView(makeWindowView());
            window.clear(sf::Color::Black);
            window.draw(rtSprite);
            ImGui::SFML::Render(window);
            window.display();
        }
    }

    ImGui::SFML::Shutdown();
    return true;
}
