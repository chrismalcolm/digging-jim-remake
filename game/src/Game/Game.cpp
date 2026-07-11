#include "Game/Game.h"
#include "Shader/Shader.h"
#include "HUD/Developer/PositionDisplay.h"
#include "HUD/Developer/FPSCounter.h"
#include "HUD/Developer/CameraCenter.h"
#include "HUD/Level/Panel.h"
#include "HUD/MainMenu/MainMenu.h"

#include "Cave/Map/Map.h"
#include "Cave/Manager/Manager.h"
#include "Utils/Counter.h"

#include <algorithm>
#include <iostream>
#include <filesystem>
#ifdef _WIN32
#include <windows.h>
#endif

#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>

#include <fstream>
#include <sstream>
#include <string>
#include <cctype>
#include <system_error>

// ----------------------------------------------------------------------------------
// Construction
// ----------------------------------------------------------------------------------

Game::Game() :
    m_collected(0),
    m_caveNumber(INITIAL_CAVE),
    m_caveCount(0),
    m_lives(INITIAL_LIVES),
    m_extraLifeThreshold(EXTRA_LIFE_SCORE_STEP),
    m_time(0),
    m_score(0),
    m_gameIsPaused(0),
    m_caveFileIndex(0),
    m_chosenCaveNumber(INITIAL_CAVE)
{
    m_settings = loadGameOptionsFromFile(SETTINGS_FILE);
};

// ----------------------------------------------------------------------------------
// Handle signals and updates
// ----------------------------------------------------------------------------------

void Game::sendSignal(const GameSignal& signal) {
    switch (m_gameState) {
    case GameState::MainMenu: handleMainMenu(signal); break;
    case GameState::CaveLoad: handleCaveLoad(signal); break;
    case GameState::CavePlay: handleCavePlay(signal); break;
    case GameState::CavePass: handleCavePass(signal); break;
    case GameState::CaveFail: handleCaveFail(signal); break;
    case GameState::GameDone: handleGameDone(signal); break;
    case GameState::GameOver: handleGameOver(signal); break;
    }
}

void Game::handleMainMenu(const GameSignal& signal) {
    switch (signal) {
    case GameSignal::PlayGame:
        if (m_settings.setRefreshRateOnStart) setRefreshrate();
        resetGame();
        m_chosenCaveNumber = getCaveNumber();
        m_gameState = GameState::CaveLoad;
        m_revealCaveHUD = true;
        break;
    case GameSignal::PreviousCave:
        cheatMode() ?
            decrementCaveNumber(1) :
            decrementCaveNumber(5);
        break;
    case GameSignal::NextCave:
        if (cheatMode()) incrementCaveNumber(1);
        else if (m_caveNumber == 1 && m_caveCount >= 5) incrementCaveNumber(4);
        else if (m_caveNumber < m_caveCount - 5) incrementCaveNumber(5);
        break;
    case GameSignal::StartMusic:
        if (m_settings.audio) soundManager.playMusic(Sound::Music::MainMenu);
        break;
    case GameSignal::StopMusic:
        soundManager.stopMusic();
        break;
    case GameSignal::SetRefreshRate:
        setRefreshrate();
        break;
    case GameSignal::ExitGame:
        m_closeWindow = true;
        break;
    default:
        break;
    }
}

void Game::handleCaveLoad(const GameSignal& signal) {
    switch (signal) {
    case GameSignal::CaveBegin:
        m_caveActive = true;
        break;

    case GameSignal::CaveStart:
        m_gameState = GameState::CavePlay;
        m_time = initialCaveTime();
        break;

    case GameSignal::CaveLoad:
        resetCaveState();
        break;

    case GameSignal::GameCompleted:
        m_gameState = GameState::GameDone;
        break;

    case GameSignal::GotoMainMenu:
        if (m_editorMode) { m_closeWindow = true; break; }
        m_gameState = GameState::MainMenu;
        m_caveActive = false;
        m_revealCaveHUD = false;
        m_makeHUDDisappear = true;
        m_exitedFromCave = true;
        break;

    default:
        break;
    }
}

void Game::handleCavePlay(const GameSignal& signal) {
    switch (signal) {
    case GameSignal::CaveLoad:
        resetCaveState();
        break;

    case GameSignal::CavePass:
        m_gameState = GameState::CavePass;
        break;

    case GameSignal::CaveFail:
        m_gameState = GameState::CaveFail;
        break;

    case GameSignal::CollectDiamond:
        m_score += getDiamondValue();
        m_collected++;
        break;

    case GameSignal::CavePause:
        m_gameIsPaused = true;
        break;

    case GameSignal::CaveUnpause:
        m_gameIsPaused = false;
        break;

    case GameSignal::GameCompleted:
        m_gameState = GameState::GameDone;
        break;

    case GameSignal::GotoMainMenu:
        if (m_editorMode) { m_closeWindow = true; break; }
        m_gameState = GameState::MainMenu;
        m_caveActive = false;
        m_revealCaveHUD = false;
        m_makeHUDDisappear = true;
        m_exitedFromCave = true;
        soundManager.stop(Sound::Effect::Amoeba);
        soundManager.stop(Sound::Effect::MagicWall);
        break;

    default:
        break;
    }
}

void Game::handleCavePass(const GameSignal& signal) {
    switch (signal) {
    case GameSignal::CaveLoad:
        nextCave();
        if (m_gameState != GameState::GameDone) resetCaveState();
        break;

    case GameSignal::GameCompleted:
        m_gameState = GameState::GameDone;
        break;

    case GameSignal::GotoMainMenu:
        if (m_editorMode) { m_closeWindow = true; break; }
        m_gameState = GameState::MainMenu;
        m_caveActive = false;
        m_revealCaveHUD = false;
        m_makeHUDDisappear = true;
        soundManager.stop(Sound::Effect::Amoeba);
        soundManager.stop(Sound::Effect::MagicWall);
        break;

    default:
        break;
    }
}

void Game::handleCaveFail(const GameSignal& signal) {
    switch (signal) {
    case GameSignal::CaveLoad:
        m_lives--;
        if (m_lives == 0) {
            m_gameState = GameState::GameOver;
            break;
        }
        resetCaveState();
        break;

    case GameSignal::GameCompleted:
        m_gameState = GameState::GameDone;
        break;

    case GameSignal::GotoMainMenu:
        if (m_editorMode) { m_closeWindow = true; break; }
        m_gameState = GameState::MainMenu;
        m_caveActive = false;
        m_revealCaveHUD = false;
        m_makeHUDDisappear = true;
        m_exitedFromCave = true;
        soundManager.stop(Sound::Effect::Amoeba);
        soundManager.stop(Sound::Effect::MagicWall);
        break;

    default:
        break;
    }
}

void Game::handleGameDone(const GameSignal& signal) {
    m_caveActive = false;
    if (signal == GameSignal::GotoMainMenu) {
        if (m_editorMode) { m_closeWindow = true; return; }
        m_gameState = GameState::MainMenu;
        m_revealCaveHUD = false;
        m_makeHUDDisappear = true;
        setCaveNumber(m_chosenCaveNumber);
    }
}

void Game::handleGameOver(const GameSignal& signal) {
    m_caveActive = false;
    if (signal == GameSignal::GotoMainMenu) {
        if (m_editorMode) { m_closeWindow = true; return; }
        m_gameState = GameState::MainMenu;
        m_revealCaveHUD = false;
        setCaveNumber(m_chosenCaveNumber);
    }
}

bool Game::run(const std::string& caveFile, int startCave, bool editorMode, bool fullscreen, bool developerMode) {
    m_editorMode       = editorMode;
    m_editorFullscreen = fullscreen;
    m_developerMode    = developerMode;
    if (editorMode)
    {
        m_editorCaveFile  = caveFile;
        m_editorStartCave = startCave;
    }
    try {
        mainGameLoop();
        return true;
    }
    catch (const std::exception& e) {
        m_lastError = e.what();
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return false;
    }
}

void Game::mainGameLoop() {

    window = m_editorFullscreen
        ? sf::RenderWindow(sf::VideoMode::getDesktopMode(), APPLICATION_NAME, sf::State::Fullscreen)
        : sf::RenderWindow(sf::VideoMode({ SCREEN_WIDTH, SCREEN_HEIGHT }), APPLICATION_NAME);
    setRefreshrate();

    imageManager.loadAllImages();
    soundManager.loadAllSounds();

    Shader::Shader shaderManager(this);
    Cave::Map map(this);
    Cave::Manager caveManager(this);

    HUD::MainMenu::MainMenu mainMenu(this);
    HUD::Level::Panel levelPanel(this);
    HUD::Developer::PositionDisplay jimPositionDisplay;
    HUD::Developer::PositionDisplay cameraPositionDisplay;
    HUD::Developer::PositionDisplay cameraOffsetDisplay;
    HUD::Developer::FPSCounter fpsCounter;
    HUD::Developer::CameraCenter cameraCenter;

    if (m_editorMode) { mainMenu.toggleMainMenuHidden(true); }

    shaderManager.loadAllShaders();
    caveManager.load();
    m_caveFilenames = caveManager.getCaveFiles();

    levelPanel.load();
    mainMenu.load();

    if (m_editorMode)
    {
        // Find the file index matching the requested cave file
        bool found = false;
        for (int i = 0; i < (int)m_caveFilenames.size(); ++i)
        {
            if (m_caveFilenames[i] == m_editorCaveFile)
            {
                m_caveFileIndex = i;
                found = true;
                break;
            }
        }
        if (!found)
            throw std::runtime_error("Failed to load cave file: " + m_editorCaveFile);

        setCaveNumber(m_editorStartCave);
        resetGame();
        m_chosenCaveNumber = m_editorStartCave;
        m_gameState        = GameState::CaveLoad;
        m_revealCaveHUD    = true;
        m_caveActive       = true;
    }
    else
    {
        sendSignal(GameSignal::StartMusic);
    }

    map.load();


    

    {
        sf::Image largeIcon;
        if (largeIcon.loadFromFile("./assets/icons/DiggingJim/large_png.png"))
            window.setIcon(largeIcon);
#ifdef _WIN32
        {
            HWND hwnd = window.getNativeHandle();
            sf::Image smallIcon;
            if (smallIcon.loadFromFile("./assets/icons/DiggingJim/small_png.png"))
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
        }
#endif
    }

    sf::Vector2f cameraTarget = { 0.f, 0.f };

    bool caveActive = false;
    bool revealCaveHUD = false;

    int lastPlacedX = -1, lastPlacedY = -1;

    bool showProps = false;
    bool showtrigger = false;

    sf::Clock deltaClock;

    sf::Vector2f cameraOffset = { 0, 0 };

    Camera camera({ SCREEN_WIDTH, SCREEN_HEIGHT }, { 0.f, 0.f }, { 640.f, 480.f });

    // Fixed-size virtual render texture — all game content draws here,
    // then the texture is stretched to fill whatever size the window is.
    sf::RenderTexture rt(sf::Vector2u(SCREEN_WIDTH, SCREEN_HEIGHT));

    // View that maps the virtual screen (640x480) to fill the entire window.
    auto makeWindowView = [&]() -> sf::View {
        return sf::View(sf::FloatRect(sf::Vector2f(0.f, 0.f),
                                      sf::Vector2f((float)SCREEN_WIDTH, (float)SCREEN_HEIGHT)));
    };

    // set settings initially
    inputSystem.setJoystick(m_settings.joystickControl);
    soundManager.setVolume(m_settings.audioVolume);

    window.setView(makeWindowView());

    while (window.isOpen())
    {
        if (showtrigger) {
            showtrigger = false;
            showProps = !showProps;
            if (showProps) {
                if (!ImGui::SFML::Init(window)) showtrigger = true;;
            }
            else ImGui::SFML::Shutdown();
        }

        while (const std::optional event = window.pollEvent())
        {
            if (showProps) {
                if (!event) continue;
                ImGui::SFML::ProcessEvent(window, *event);
            }
            else {
                inputSystem.handleEvent(event.value());
            }

            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }

            if (event->is<sf::Event::Resized>())
            {
                window.setView(makeWindowView());
            }
        }

        if (showProps) ImGui::SFML::Update(window, deltaClock.restart());
        else {
            inputSystem.handleJoystick();
        }


        if (m_closeWindow) {
            window.close();
        }

        // Check if cheat mode is activated
        if (inputSystem.wasPressed(Input::Action::ActivateCheatMode)) {
            soundManager.play(Sound::Effect::Yippee);
            m_cheatMode = true;
        }

        if (cheatMode()) {

            if (inputSystem.wasPressed(Input::Action::RestartCave) && !isGameCompleted() && !isGameOver()) {
                map.markForReset();
            }
            if (inputSystem.wasPressed(Input::Action::ForwardCave) && !isGameCompleted() && !isGameOver()) {
                nextCave();
                map.markForReset();
            }
            if (inputSystem.wasPressed(Input::Action::BackwardCave) && !isGameCompleted() && !isGameOver()) {
                decrementCaveNumber(1);
                map.markForReset();
            }

        }

        if (cheatMode() && developerMode()) {

            if (inputSystem.wasPressed(Input::Action::ToggleEditorProperties)) {
                showtrigger = true;
            }

            if (inputSystem.isPressed(Input::Action::CameraUp)) {
                if (cameraOffset.y > -2000) cameraOffset.y -= 4;
            }
            if (inputSystem.isPressed(Input::Action::CameraDown)) {
                if (cameraOffset.y < 2000)  cameraOffset.y += 4;
            }
            if (inputSystem.isPressed(Input::Action::CameraLeft)) {
                if (cameraOffset.x > -2000) cameraOffset.x -= 4;
            }
            if (inputSystem.isPressed(Input::Action::CameraRight)) {
                if (cameraOffset.x < 2000)  cameraOffset.x += 4;
            }
            if (inputSystem.isPressed(Input::Action::CameraReset)) {
                cameraOffset = { 0, 0 };
            }

        }

        setCaveCount(static_cast<int>(caveManager.numCaves(getCaveFileIndex())));

        if (showProps) Cave::editCaveProperties(window, m_caveProperties, getCaveProperties(), map.getDiamondCount(), showtrigger);

        // ---- Render to virtual screen (fixed 640x480 render texture) --------
        rt.setView(camera.view());
        rt.clear(sf::Color::Black);

        if (caveActive) {
            map.update(camera);
            cameraTarget = map.updateCameraLocation(camera, cameraOffset);
        }

        rt.setView(camera.view());

        update();

        shaderManager.update();

        cameraPositionDisplay.update(camera, "Camera", { camera.getCenter().x, camera.getCenter().y }, 16.0);
        cameraOffsetDisplay.update(camera, "Offset", { cameraOffset.x, cameraOffset.y }, 35.0);
        cameraCenter.update(camera, cameraTarget);
        fpsCounter.update(camera);
        levelPanel.update(camera);
        mainMenu.update();

        if (caveActive) rt.draw(map, shaderManager.currentShader());

        if (cameraOffset.x != 0 || cameraOffset.y != 0) {
            rt.draw(jimPositionDisplay);
            rt.draw(cameraPositionDisplay);
            rt.draw(cameraOffsetDisplay);
            rt.draw(fpsCounter);
            rt.draw(cameraCenter);
        }
        if (!caveActive) rt.draw(mainMenu, shaderManager.defaultShader());

        rt.draw(levelPanel, shaderManager.defaultShader());

        rt.display();

        inputSystem.update();

        // ---- Stretch virtual screen to fill the real window -----------------
        window.setView(makeWindowView());
        window.clear(sf::Color::Black);
        window.draw(sf::Sprite(rt.getTexture()));
        if (showProps) ImGui::SFML::Render(window);
        window.display();

        if (m_revealCaveHUD && !revealCaveHUD) {
            caveManager.startCave(getCaveFileIndex(), getCaveNumber(), &m_caveProperties, &map);
            map.prepareForPlay();
            camera.setBounds(map.getCameraBounds());
            revealCaveHUD = true;
            if (m_editorMode) levelPanel.revealInstant();
            else              levelPanel.reveal();
        }

        if (!m_revealCaveHUD && revealCaveHUD) {
            revealCaveHUD = false;
            m_makeHUDDisappear ?
                levelPanel.disappear() :
                levelPanel.hide();
            m_makeHUDDisappear = false;
        }

        if ((m_caveActive && !caveActive)) {
            camera.setCentre(map.getCameraStartLocation());
            caveActive = true;
        }

        if ((!m_caveActive && caveActive)) {
            camera.setCentre({ 0, 0 });
            caveActive = false;
        }

        if (map.requiresReset()) {
            caveManager.startCave(getCaveFileIndex(), getCaveNumber(), &m_caveProperties, &map);
            map.prepareForPlay();
            camera.setBounds(map.getCameraBounds());
            if (map.resetCameraPosition()) camera.setCentre(map.getCameraStartLocation());
        }

        Utils::incrementGlobalCounter();
    }
}

void Game::update() {
    switch (m_gameState) {
    case GameState::MainMenu:
        break;

    case GameState::CavePlay:
        // Decrease timer once per second while cave is active
        if (!m_gameIsPaused && m_time > 0) m_time--;
        break;

    case GameState::CavePass:
        // Convert leftover time into bonus points at end of cave
        if (m_time > 0) {
            m_score++;
            m_time -= 64;
        }
        break;

    default:
        break;
    }

    // Always check for extra lives regardless of state
    checkExtraLife();

    // Update sound manager
    soundManager.update();
}

// ----------------------------------------------------------------------------------
// Useful getters/setters
// ----------------------------------------------------------------------------------

int Game::getDiamondValue() const {
    if (m_gameState == GameState::CaveLoad) return m_caveProperties.diamondValue;
    return caveQuotaReached()
        ? m_caveProperties.extraDiamondValue
        : m_caveProperties.diamondValue;
}

int Game::getCollected() const {
    return m_collected;
}

int Game::getQuota() const {
    return m_caveProperties.quota;
}

int Game::getCaveNumber() const {
    return m_caveNumber;
}

void Game::setCaveNumber(const int& num) {
    m_caveNumber = num;
}

int Game::getCaveCount() const {
    return m_caveCount;
}

std::vector<std::string> Game::getCaveFiles() {
    return m_caveFilenames;
}

void Game::setCaveCount(const int& num) {
    m_caveCount = num;
}

void Game::nextCave() {
    if (m_caveNumber == getCaveCount()) {
        sendSignal(GameSignal::GameCompleted);
    }
    else {
        incrementCaveNumber(1);
    }
}

void Game::incrementCaveNumber(const int& inc) {
    m_caveNumber = std::min(m_caveNumber + inc, getCaveCount());
}

void Game::decrementCaveNumber(const int& dec) {
    m_caveNumber = std::max(m_caveNumber - dec, 1);
}

int Game::getTime() const {
    return m_time / 64;
}

void Game::setTime(const int& seconds) {
    // Do not set time if the game is completed or over
    if (m_gameState == GameState::GameDone || m_gameState == GameState::GameOver) return;
    m_time = seconds * 64;
}

int Game::getLives() const {
    return m_lives;
}

int Game::getScore() const {
    return m_score;
}

int Game::getHue() const {
    return m_caveProperties.hue;
}

int Game::getSat() const {
    return m_caveProperties.sat;
}

int Game::getLum() const {
    return m_caveProperties.lum;
}

Cave::Properties Game::getCaveProperties() {
    return m_caveProperties;
}

void Game::setCaveProperties(const Cave::Properties& properties) {
    m_caveProperties = properties;
}

// ----------------------------------------------------------------------------------
// Useful conditionals
// ----------------------------------------------------------------------------------

bool Game::isGamePaused() const {
    return m_gameIsPaused;
}

bool Game::isGameCompleted() const {
    return m_gameState == GameState::GameDone;
}

bool Game::isGameOver() const {
    return m_gameState == GameState::GameOver;
}

bool Game::onMainMenu() const {
    return m_gameState == GameState::MainMenu;
}

int Game::getCaveFileIndex() {
    return m_caveFileIndex;
}

void Game::setCaveFileIndex(int caveFileIndex) {
    m_caveFileIndex = caveFileIndex;
}

bool Game::markedAsExitedFromCave() {
    if (!m_exitedFromCave) return false;
    m_exitedFromCave = false;
    return true;
}

bool Game::developerMode() const {
    return m_developerMode;
}

bool Game::cheatMode() const {
    return m_cheatMode || m_developerMode;
}

bool Game::caveQuotaReached() const {
    return m_collected >= m_caveProperties.quota;
}

bool Game::isTimeWarning() const {
    return (m_gameState == GameState::CavePlay && getTime() <= 10);
}

// ----------------------------------------------------------------------------------
// Other Internal Logic
// ----------------------------------------------------------------------------------

int Game::initialCaveTime() const {
    return m_caveProperties.time * 64 + 63;
}

void Game::resetCaveState() {
    m_gameState = GameState::CaveLoad;
    m_time = initialCaveTime();
    m_collected = 0;
    m_gameIsPaused = false;
}

void Game::resetGame() {
    m_lives = INITIAL_LIVES;
    m_score = 0;
}

void Game::checkExtraLife() {
    while (m_score >= m_extraLifeThreshold) {
        soundManager.play(Sound::Effect::Yahoo);
        m_extraLifeThreshold += EXTRA_LIFE_SCORE_STEP;
        m_lives++;
    }
}

void Game::setRefreshrate() {
    window.setFramerateLimit(64);
}

GameSettings Game::getGameOptions() const {
    return m_settings;
}

void Game::commitGameOptions(const GameSettings& options) {
    bool startMusic = !m_settings.audio && options.audio;
    bool stopMusic = m_settings.audio && !options.audio;
    m_settings = options;
    inputSystem.setJoystick(options.joystickControl);
    soundManager.setVolume(options.audioVolume);
    if (startMusic) sendSignal(GameSignal::StartMusic);
    if (stopMusic) sendSignal(GameSignal::StopMusic);
    saveGameOptionsToFile(m_settings, SETTINGS_FILE);
}