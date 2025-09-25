#pragma once
#include <SFML/Graphics.hpp>
#include "Game/Game.h"
#include "Renderer/TextRenderer.h"
#include "Renderer/TileRenderer.h"
#include "Camera/Camera.h"
#include "Utils/Counter.h"
#include <string>

/// @brief The path to the credits text file.
const std::string CREDITS_TEXT_FILE = ".\\assets\\credits.txt";

namespace HUD::MainMenu {

    /// @brief Menu selection options on the main menu.
    enum class Selection {
        /// @brief Play the game
        Play = 0,
        /// @brief Start the currently selected cave
        StartCave,
        /// @brief Open cave selection menu
        LoadCaves,
        /// @brief Open options menu
        Options,
        /// @brief Quit the game
        Quit
    };

    /// @brief Main menu sections (different screens/views).
    enum class Section {
        /// @brief Main menu screen
        Main = 0,
        /// @brief Cave selection screen
        LoadCaves,
        /// @brief Options/settings screen
        Options,
        /// @brief Game completed screen
        GameCompleted,
        /// @brief Game over screen
        GameOver,
        /// @brief No active section
        None 
    };

    /// @brief Options within the main menu options screen.
    enum class Options {
        /// @brief Toggle audio on/off
        Audio = 0,
        /// @brief Adjust audio volume
        AudioVolume,
        /// @brief Toggle joystick control
        JoystickControl,
        /// @brief Toggle fixed colors mode
        FixedColours,
        /// @brief Set refresh rate
        SetRefreshrate,
        /// @brief Apply refresh rate on startup
        SetRefreshrateOnStart,
        /// @brief Save settings
        SaveSettings,
        /// @brief Return to main menu
        QuitToMainMenu
    };

    /// @brief Main menu HUD, handles drawing and updating the main menu, cave selection, and options.
    class MainMenu : public sf::Drawable, public sf::Transformable {
    public:
        /**
         * @brief Constructs the main menu with a reference to the game.
         * @param game Pointer to the Game instance.
         */
        MainMenu(Game* game);

        /**
         * @brief Loads all textures, fonts, and renderers used by the menu.
         */
        void load();

        /**
         * @brief Updates the menu state each frame, handling user input and animations.
         */
        void update();

        /**
         * @brief Updates the main menu section, including selection cursor and cave start logic.
         */
        void updateMainSection();

        /**
         * @brief Updates the load caves section, including cave file selection.
         */
        void updateLoadCavesSection();

        /**
         * @brief Updates the options section, including toggles and sliders.
         */
        void updateOptionsSection();

        /**
         * @brief Updates the game completed section, handling user confirmation.
         */
        void updateGameCompletedSection();

        /**
         * @brief Updates the game over section, handling animation and transition back to main menu.
         */
        void updateGameOverSection();

    private:
        /**
         * @brief Draws the current menu section, including textures, text, and arrows.
         * @param target Render target to draw to.
         * @param states Current render states.
         */
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

        /// @brief Pointer to the main Game instance.
        Game* m_game;

        /// @brief Textures for menu layers and sections.
        sf::Texture m_menu;
        sf::Texture m_below;
        sf::Texture m_loadCaves;
        sf::Texture m_options;
        sf::Texture m_completed;
        sf::Texture m_gameOver;

        /// @brief Renderers for options toggles and dials.
        Renderer::TextRenderer m_optionOnOffAudio;
        Renderer::TextRenderer m_optionOnOffAudioVolumeDial;
        Renderer::TextRenderer m_optionOnOffJoystickControl;
        Renderer::TextRenderer m_optionOnOffFiedColours;
        Renderer::TextRenderer m_optionOnOffSetRefreshrateOnStart;

        /// @brief Renderer for cave number in the HUD.
        Renderer::TextRenderer m_caveNumberRenderer;

        /// @brief Renderers for selection arrows.
        Renderer::TextRenderer m_selectArrow;
        Renderer::TextRenderer m_selectCaveFileArrow;
        Renderer::TextRenderer m_selectOptionsArrow;

        /// @brief Renderer for credits.
        Renderer::TextRenderer m_credits;
        /// @brief The credits text.
        std::string m_creditsText;
        /// @brief X position of credits.
        int m_creditsPositionX;

        /// @brief Renderer for loading tiles animation.
        Renderer::TileRenderer m_loadingTileRenderer;

        /// @brief Placeholders for cave file names.
        std::vector<Renderer::TextRenderer> m_caveFilePlaceolder;
        /// @brief List of cave file names.
        std::vector<std::string> m_caveFiles;
        /// @brief Start index for cave files displayed.
        int m_caveFileStartListIndex = 0;

        /// @brief Flags for loading tile animation (true = visible).
        std::vector<bool> loadingTiles;

        /// @brief Tick counter for updating animations.
        Utils::TickCounter m_tickCounter = Utils::TickCounter(8);

        /// @brief Vertical position of main selection arrow.
        int m_selectArrowY = 0;

        /// @brief Whether cave loading animation has begun.
        bool m_caveBegin = false;
        /// @brief Loading rate (for tile animation).
        int m_loadRate = 0;

        /// @brief Whether to hide game over tiles.
        bool m_gameOverTilesHide = true;

        /// @brief Whether game over screen is vanishing.
        bool m_gameOverVanishing = false;
        /// @brief Height of the game over cropping animation.
        int m_gameOverHeight = 0;

        /// @brief Vertical positions for cave file arrow animation.
        int m_selectCaveFileArrowY = 0;
        int m_selectCaveFileArrowTargetY = 0;
        /// @brief Currently selected cave file index.
        int m_selectedCaveFileIndex = 0;

        /// @brief Vertical position of options selection arrow.
        int m_selectOptionsArrowY = 0;
        /// @brief Currently selected option in the options menu.
        Options m_selectedOption = Options::Audio;

        /// @brief Local copy of game settings.
        GameSettings m_settings;

        /// @brief Currently selected main menu option.
        Selection m_selected = Selection::Play;
        /// @brief Currently active section of the main menu.
        Section m_section = Section::Main;
    };
}
