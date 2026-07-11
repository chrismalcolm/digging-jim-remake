#include "HUD/MainMenu/MainMenu.h"
#include "Utils/Counter.h"
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>

HUD::MainMenu::MainMenu::MainMenu(Game* game)
    : m_game(game), m_caveNumberRenderer(game, 3), m_selectArrow(game, 1), m_selectCaveFileArrow(game, 1), m_selectOptionsArrow(game, 1), m_credits(game, 1024), m_loadingTileRenderer(&game->imageManager)
    , m_creditsText(""), m_creditsPositionX(800), m_caveFilePlaceolder({}), m_caveFiles({})
    , m_optionOnOffAudio(game, 1), m_optionOnOffAudioVolumeDial(game, 1), m_optionOnOffJoystickControl(game, 1), m_optionOnOffFiedColours(game, 1), m_optionOnOffSetRefreshrateOnStart(game, 1)
{
    for (int index = 0; index < 20 * 15; index++) {
        loadingTiles.emplace_back(true);
    }

    for (int index = 0; index < 8; index++) {
        m_caveFilePlaceolder.emplace_back(Renderer::TextRenderer(game, 25));
    }
}

void HUD::MainMenu::MainMenu::load() {
    m_menu = m_game->imageManager.getTexture(Image::Texture::MainMenuTop);
    m_below = m_game->imageManager.getTexture(Image::Texture::MainMenuBelow);
    m_loadCaves = m_game->imageManager.getTexture(Image::Texture::MainMenuLoadCaves);
    m_options = m_game->imageManager.getTexture(Image::Texture::MainMenuOptions);
    m_completed = m_game->imageManager.getTexture(Image::Texture::GameCompleted);
    m_gameOver = m_game->imageManager.getTexture(Image::Texture::GameOver);

    m_caveNumberRenderer.load(Image::Texture::MainMenuNumbers, { 32, 32 });

    for (int index = 0; index < 8; index++) {
        m_caveFilePlaceolder[index].load(Image::Texture::GameFont, { 16, 32 });
    }

    m_selectArrow.load(Image::Texture::MainMenuSelectArrow, { 32, 32 });
    m_selectCaveFileArrow.load(Image::Texture::MainMenuSelectArrow, { 32, 32 });
    m_selectOptionsArrow.load(Image::Texture::MainMenuSelectArrow, { 32, 32 });
    m_credits.load(Image::Texture::GameFont, { 16, 32 });

    if (!m_loadingTileRenderer.load(Image::Texture::CaveLoadingTiles, { 32, 32 })) {
        throw std::runtime_error("Error: Unable to load map loading texture.\n");
    }

    std::ifstream file(CREDITS_TEXT_FILE, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Unable to load cave file: " + CREDITS_TEXT_FILE);
    }

    m_creditsText = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    m_caveFiles = m_game->getCaveFiles();

    m_optionOnOffAudio.load(Image::Texture::MainMenuOnOff, { 64, 32 });
    m_optionOnOffAudioVolumeDial.load(Image::Texture::MainMenuDial, { 16, 32 });
    m_optionOnOffJoystickControl.load(Image::Texture::MainMenuOnOff, { 64, 32 });
    m_optionOnOffFiedColours.load(Image::Texture::MainMenuOnOff, { 64, 32 });
    m_optionOnOffSetRefreshrateOnStart.load(Image::Texture::MainMenuOnOff, { 64, 32 });
}

void HUD::MainMenu::MainMenu::update() {

    if (m_game->isGameCompleted()) m_section = HUD::MainMenu::Section::GameCompleted;
    if (m_game->isGameOver()) m_section = HUD::MainMenu::Section::GameOver;
    if (m_game->markedAsExitedFromCave()) {
        m_section = HUD::MainMenu::Section::Main;
        m_game->sendSignal(GameSignal::StartMusic);
        for (int index = 0; index < 20 * 15; index++) loadingTiles[index] = true;
        m_creditsPositionX = 800;
    }

    switch (m_section) {
    case HUD::MainMenu::Section::Main:
        if (!m_hideMainMenuVisuals) updateMainSection();
        break;
    case HUD::MainMenu::Section::LoadCaves:
        if (!m_hideMainMenuVisuals) updateLoadCavesSection();
        break;
    case HUD::MainMenu::Section::Options:
        if (!m_hideMainMenuVisuals) updateOptionsSection();
        break;
    case HUD::MainMenu::Section::GameCompleted:
        updateGameCompletedSection();
        break;
    case HUD::MainMenu::Section::GameOver:
        if (m_hideMainMenuVisuals) {
            for (int index = 0; index < 20 * 15; index++) loadingTiles[index] = false;
        }
        updateGameOverSection();
        break;
    default:
        break;
    }

    m_creditsPositionX -= 2;
    if (m_creditsPositionX < -6500) m_creditsPositionX = 800;

    m_caveNumberRenderer.updateNumbers({
        {{ 480, 64 }, m_game->getCaveNumber(), 3, 0},
        });

    m_selectArrow.updateNumbers({
        {{ 200, 12 + m_selectArrowY}, m_tickCounter.tickCount(), 1, 0},
        });

    m_selectCaveFileArrow.updateNumbers({
        {{ 200, 26 + m_selectCaveFileArrowY}, m_tickCounter.tickCount(), 1, 0},
        });

    m_selectOptionsArrow.updateNumbers({
        {{ 200, 12 + m_selectOptionsArrowY}, m_tickCounter.tickCount(), 1, 0},
        });
    
    m_optionOnOffAudio.updateNumbers({
        {{ 320, 12 }, 1 - static_cast<int>(m_settings.audio), 1, 0},
        });

    m_optionOnOffAudioVolumeDial.updateNumbers({
        {{ 384 + 36 + static_cast<int>(m_settings.audioVolume * 1.65f), 44 }, 0, 1, 0},
        });

    m_optionOnOffJoystickControl.updateNumbers({
       {{ 384 + 80 + 16, 76 }, 1 - static_cast<int>(m_settings.joystickControl), 1, 0},
       });

    m_optionOnOffFiedColours.updateNumbers({
       {{ 384 + 32 + 16, 108 }, 1 - static_cast<int>(m_settings.fixedColours), 1, 0},
       });

    m_optionOnOffSetRefreshrateOnStart.updateNumbers({
       {{ 640 - 64, 172 }, 1 - static_cast<int>(m_settings.setRefreshRateOnStart), 1, 0},
       });

    m_credits.updateMovingText(m_creditsPositionX, 400, m_creditsText);

    m_loadingTileRenderer.updateLoadingTexture(loadingTiles, { 0, 0 }, sf::IntRect({ 0, 0 }, { 20, 15 }));

    int i = m_caveFileStartListIndex;
    for (int index = 0; index < 8; index++) {
        if (i >= m_caveFiles.size()) break;
        std::string caveName = m_caveFiles[i];
        if (caveName == "originals.cav") caveName = "Original Levels";
        m_caveFilePlaceolder[index].updateText(240, index * 32 + 28, caveName);
        i++;
    }
}

void HUD::MainMenu::MainMenu::updateMainSection() {
    // Handle main select cursor movement
    if (!m_gameOverTilesHide) {
        if (m_selectArrowY < static_cast<int>(m_selected) * 48) m_selectArrowY += 6;
        else if (m_selectArrowY > static_cast<int>(m_selected) * 48) m_selectArrowY -= 6;

        if (std::abs(static_cast<int>(m_selected) * 48 - m_selectArrowY) < 4) {
            if (m_game->inputSystem.isPressed(Input::Action::MoveUp)) m_selected = static_cast<HUD::MainMenu::Selection>(std::max(static_cast<int>(m_selected) - 1, 0));
            else if (m_game->inputSystem.isPressed(Input::Action::MoveDown)) m_selected = static_cast<HUD::MainMenu::Selection>(std::min(static_cast<int>(m_selected) + 1, 4));
        }
    }

    // Handle main select cursor selection
    switch (static_cast<HUD::MainMenu::Selection>(m_selected)) {

    case HUD::MainMenu::Selection::Play:
        if (m_game->inputSystem.wasPressed(Input::Action::Confirm)) {
            m_game->sendSignal(GameSignal::StopMusic);
            m_game->sendSignal(GameSignal::PlayGame);
            m_caveBegin = true;
            m_gameOverTilesHide = false;
        }
        break;

    case HUD::MainMenu::Selection::StartCave:
        if (m_tickCounter.onTick()) {
            if (m_game->inputSystem.isPressed(Input::Action::MoveRight)) {
                m_game->sendSignal(GameSignal::NextCave);
            }
            else if (m_game->inputSystem.isPressed(Input::Action::MoveLeft)) {
                m_game->sendSignal(GameSignal::PreviousCave);
            }
        }
        break;

    case HUD::MainMenu::Selection::LoadCaves:
        if (m_game->inputSystem.wasPressed(Input::Action::Confirm)) {
            m_section = HUD::MainMenu::Section::LoadCaves;
        }
        break;

    case HUD::MainMenu::Selection::Options:
        if (m_game->inputSystem.wasPressed(Input::Action::Confirm)) {
            m_settings = m_game->getGameOptions();
            m_section = HUD::MainMenu::Section::Options;
        }
        break;

    case HUD::MainMenu::Selection::Quit:
        if (m_game->inputSystem.wasPressed(Input::Action::Confirm)) {
            m_game->sendSignal(GameSignal::ExitGame);
        }
        break;

    default:
        break;
    }

    // Handle loading tile transitions
    if (m_caveBegin) {
        if (m_loadRate < 96) {
            m_loadRate++;
            for (int index = 0; index < 20 * 15; index++) {
                if (Utils::randomInteger(m_loadRate, 96) == 96) loadingTiles[index] = false;
            }
        }
        else {
            m_game->sendSignal(GameSignal::CaveBegin);
            m_caveBegin = false;
            m_section = HUD::MainMenu::Section::None;
            m_loadRate = 0;
        }
    }
    else if (m_gameOverTilesHide) {
        m_loadRate--;
        if (m_loadRate < 0) m_loadRate = 0;
        for (int index = 0; index < 20 * 15; index++) {
            if (Utils::randomInteger(0, m_loadRate) == 0) loadingTiles[index] = true;
        }
        if (m_loadRate == 0) m_gameOverTilesHide = false;
    }
}

void HUD::MainMenu::MainMenu::updateLoadCavesSection() {
    if (m_selectCaveFileArrowY == m_selectCaveFileArrowTargetY &&
        ((m_selectCaveFileArrowY != 0 && m_selectCaveFileArrowY != 7 * 32) || m_tickCounter.onTick())
        ) {
        if (m_game->inputSystem.isPressed(Input::Action::MoveUp)) {
            if (m_selectCaveFileArrowTargetY == 0 && m_caveFileStartListIndex > 0) m_caveFileStartListIndex--;
            m_selectCaveFileArrowTargetY = std::max(m_selectCaveFileArrowTargetY - 32, 0);
            m_selectedCaveFileIndex = std::max(m_selectedCaveFileIndex - 1, 0);
        }
        else if (m_game->inputSystem.isPressed(Input::Action::MoveDown)) {
            if (m_selectCaveFileArrowTargetY == std::min(static_cast<int>(m_caveFiles.size()) - 1, 7) * 32 && m_caveFileStartListIndex < std::max(0, static_cast<int>(m_caveFiles.size()) - 8)) m_caveFileStartListIndex++;
            m_selectCaveFileArrowTargetY = std::min(m_selectCaveFileArrowTargetY + 32, 32 * std::min(static_cast<int>(m_caveFiles.size()) - 1, 7));
            m_selectedCaveFileIndex = std::min(m_selectedCaveFileIndex + 1, static_cast<int>(m_caveFiles.size() - 1));
        }
    }

    if (m_selectCaveFileArrowY < m_selectCaveFileArrowTargetY) {
        m_selectCaveFileArrowY += 4;
    }
    else if (m_selectCaveFileArrowY > m_selectCaveFileArrowTargetY) {
        m_selectCaveFileArrowY -= 4;
    }

    if (m_game->inputSystem.wasPressed(Input::Action::Confirm)) {
        m_game->setCaveNumber(1);
        m_game->setCaveFileIndex(m_selectedCaveFileIndex);
        m_section = HUD::MainMenu::Section::Main;
    }
}

void HUD::MainMenu::MainMenu::updateOptionsSection() {
    // Handle main select cursor movement
    if (m_selectOptionsArrowY < static_cast<int>(m_selectedOption) * 32) m_selectOptionsArrowY += 4;
    else if (m_selectOptionsArrowY > static_cast<int>(m_selectedOption) * 32) m_selectOptionsArrowY -= 4;

    if (std::abs(static_cast<int>(m_selectedOption) * 32 - m_selectOptionsArrowY) < 4) {
        if (m_game->inputSystem.isPressed(Input::Action::MoveUp)) m_selectedOption = static_cast<HUD::MainMenu::Options>(std::max(static_cast<int>(m_selectedOption) - 1, 0));
        else if (m_game->inputSystem.isPressed(Input::Action::MoveDown)) m_selectedOption = static_cast<HUD::MainMenu::Options>(std::min(static_cast<int>(m_selectedOption) + 1, 7));
    }

    switch (m_selectedOption) {
    case HUD::MainMenu::Options::Audio:
        if (m_game->inputSystem.wasPressed(Input::Action::Confirm)) {
            m_settings.audio = !m_settings.audio;
        }
        break;
    case HUD::MainMenu::Options::AudioVolume:
        if (m_tickCounter.onTick()) {
            if (m_game->inputSystem.isPressed(Input::Action::MoveRight)) {
                m_settings.audioVolume = std::min(100.f, m_settings.audioVolume + 5.f);
            }
            if (m_game->inputSystem.isPressed(Input::Action::MoveLeft)) {
                m_settings.audioVolume = std::max(0.f, m_settings.audioVolume - 5.f);
            }
        }
        break;
    case HUD::MainMenu::Options::JoystickControl:
        if (m_game->inputSystem.wasPressed(Input::Action::Confirm)) {
            m_settings.joystickControl = !m_settings.joystickControl;
        }
        break;
    case HUD::MainMenu::Options::FixedColours:
        if (m_game->inputSystem.wasPressed(Input::Action::Confirm)) {
            m_settings.fixedColours = !m_settings.fixedColours;
        }
        break;
    case HUD::MainMenu::Options::SetRefreshrate:
        break;
    case HUD::MainMenu::Options::SetRefreshrateOnStart:
        if (m_game->inputSystem.wasPressed(Input::Action::Confirm)) {
            m_settings.setRefreshRateOnStart = !m_settings.setRefreshRateOnStart;
        }
        break;
    case HUD::MainMenu::Options::SaveSettings:
        if (m_game->inputSystem.wasPressed(Input::Action::Confirm)) {
            m_game->commitGameOptions(m_settings);
        }
        break;
    case HUD::MainMenu::Options::QuitToMainMenu:
        if (m_game->inputSystem.wasPressed(Input::Action::Confirm)) {
            m_section = HUD::MainMenu::Section::Main;
        }
        break;
    default:
        break;
    }
    if (m_game->inputSystem.wasPressed(Input::Action::Quit)) {
        m_section = HUD::MainMenu::Section::Main;
    }
}

void HUD::MainMenu::MainMenu::updateGameCompletedSection() {
    if (m_game->inputSystem.wasPressed(Input::Action::Confirm)) {
        m_game->sendSignal(GameSignal::GotoMainMenu);
        m_game->sendSignal(GameSignal::StartMusic);
        m_section = HUD::MainMenu::Section::Main;  
        for (int index = 0; index < 20 * 15; index++) {
            loadingTiles[index] = true;
        }
        m_creditsPositionX = 800;
    }
}

void HUD::MainMenu::MainMenu::updateGameOverSection() {
    if (!m_gameOverVanishing) {
        m_gameOverHeight = 0;
        if (m_game->inputSystem.wasPressed(Input::Action::Confirm)) {
            m_gameOverVanishing = true;
        }
    }
    else {
        m_gameOverHeight++;
        if (m_gameOverHeight >= 96) {
            m_section = HUD::MainMenu::Section::Main;
            m_gameOverTilesHide = true;
            m_loadRate = 192;
            m_gameOverHeight = 0;
            m_gameOverVanishing = false;
            m_game->sendSignal(GameSignal::GotoMainMenu);
            m_game->sendSignal(GameSignal::StartMusic);
            m_creditsPositionX = 800;
        }
    }
}

void HUD::MainMenu::MainMenu::toggleMainMenuHidden(bool hidden) {
    m_hideMainMenuVisuals = hidden;
}

void HUD::MainMenu::MainMenu::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();

    // Do not draw main menu parts when game is completed
    if (m_section != HUD::MainMenu::Section::GameCompleted) {
        if (!m_hideMainMenuVisuals) {
            sf::Sprite menu(m_menu);
            menu.setPosition({ 0.f, 0.f });
            target.draw(menu, states);

            sf::Sprite below(m_below);
            below.setPosition({ 0.f, 320.f });
            target.draw(below, states);

            m_credits.render(target, states);

            m_caveNumberRenderer.render(target, states);
            m_selectArrow.render(target, states);
        }

        m_loadingTileRenderer.render(target, states);
    }

    if (m_section == HUD::MainMenu::Section::LoadCaves) {

        sf::Sprite loadCaves(m_loadCaves);
        loadCaves.setPosition({ 190.f, 0.f });
        target.draw(loadCaves, states);

        for (int index = 0; index < 8; index++) {
            m_caveFilePlaceolder[index].render(target, states);
        }
        m_selectCaveFileArrow.render(target, states);
    }
    else if (m_section == HUD::MainMenu::Section::Options) {

        sf::Sprite options(m_options);
        options.setPosition({ 190.f, 0.f });
        target.draw(options, states);

        m_optionOnOffAudio.render(target, states);
        m_optionOnOffAudioVolumeDial.render(target, states);
        m_optionOnOffJoystickControl.render(target, states);
        m_optionOnOffFiedColours.render(target, states);
        m_optionOnOffSetRefreshrateOnStart.render(target, states);

        m_selectOptionsArrow.render(target, states);
    }
    else if (m_section == HUD::MainMenu::Section::GameCompleted) {
        // Then draw the "Game Completed" sprite on top
        sf::Sprite completed(m_completed);
        completed.setPosition({ 0.f, 96.f });
        target.draw(completed, states);
    }
    else if (m_section == HUD::MainMenu::Section::GameOver) {
        sf::Sprite gameover(m_gameOver);

        // Full dimensions of the texture
        int texWidth = m_gameOver.getSize().x;
        int texHeight = m_gameOver.getSize().y;

        // Crop region:
        // Top moves down by m_gameOverHeight
        // Height shrinks by 2 * m_gameOverHeight
        int cropTop = std::min(m_gameOverHeight, 64);
        int cropHeight = texHeight - (2 * cropTop);

        if (cropHeight > 0) {
            gameover.setTextureRect(sf::IntRect(
                { 0, cropTop },
                { texWidth, cropHeight } 
            ));

            // Position it so cropping looks "centered" at y=128
            gameover.setPosition({ 0.f, 144.f + static_cast<float>(cropTop) });
            target.draw(gameover, states);
        }
    }
}
