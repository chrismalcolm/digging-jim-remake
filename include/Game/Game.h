#pragma once
#include "Game/State.h"
#include "Game/Signal.h"
#include "Game/Settings.h"
#include "Input/Input.h"
#include "Cave/Properties/Properties.h"
#include "Image/Manager.h"
#include "Sound/Manager.h"
#include "Utils/Counter.h"
#include <string>
#include <vector>

/// @brief The name given to the application, displayed in the window.
const std::string APPLICATION_NAME = "Digging Jim";

/// @brief The width of the window.
const unsigned int SCREEN_WIDTH = 640u;

/// @brief The height of the window.
const unsigned int SCREEN_HEIGHT = 480u;

/// @brief The path to the settings file.
const std::string SETTINGS_FILE = ".\\assets\\settings\\settings.txt";

/// @brief The initial cave to begin at.
constexpr int INITIAL_CAVE = 1;

/// @brief The number of lives the player will start the game with.
constexpr int INITIAL_LIVES = 3;

/// @brief Score interval required to award extra lives.
constexpr int EXTRA_LIFE_SCORE_STEP = 500;

/**
 * @class Game
 * @brief Central manager for game assets, game progression and state.
 *
 * This class is designed to be a singleton instance.
 * Each major game component should contain a pointer to this singleton instance.
 * In order to modify the game state, signals should be sent using the sendSignal function.
 * This instance will then handle the game progression and state,
 * based on its current state and the signal it receives
 */
class Game {
public:
    // ----------------------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------------------

    /// @brief Construct a new Game instance with default values.
    Game();

    /**
     * Load GameSettings from a simple key=value file.
     *
     * - Missing file or parse errors -> returns defaults (no exception thrown).
     * - Supported keys (case-insensitive):
     *     audio = true|false|1|0|yes|no
     *     audioVolume = float (clamped to 0.0 - 100.0)
     *     joystickControl = bool
     *     fixedColours = bool
     *     setRefreshRateOnStart = bool
     *
     * Lines starting with '#' or ';' are treated as comments. Blank lines ignored.
     */
    GameSettings loadGameOptionsFromFile(const std::string& path);

    /**
     * @brief Save GameSettings to a key=value file.
     * 
     * Writes directly to the specified path (creates or overwrites).
     * On failure, does nothing (returns false).
     */
    bool saveGameOptionsToFile(const GameSettings& opts, const std::string& path);

    // ----------------------------------------------------------------------------------
    // Core Functionality
    // ----------------------------------------------------------------------------------

    /// @brief Send an external signal to drive game progression and state transitions.
    /// @param signal The signal to process.
    void sendSignal(const GameSignal& signal);

    /// @brief Run the game.
    bool run();

    // ----------------------------------------------------------------------------------
    // Useful getters/setters
    // ----------------------------------------------------------------------------------

    /// @brief Get the value of the next diamond.
    /// @return Diamond score value.
    int getDiamondValue() const;

    /// @brief Get the number of diamonds collected in the current cave.
    int getCollected() const;

    /// @brief Get the diamond quota required to complete the cave.
    int getQuota() const;

    /// @brief Get the current cave number.
    int getCaveNumber() const;

    /// @brief Set the current cave number.
    /// @param num Cave number.
    void setCaveNumber(const int& num);

    /// @brief Get the current cave count.
    int getCaveCount() const;

    /// @brief Get the filenames of the caves loaded into the game.
    std::vector<std::string> getCaveFiles();

    /// @brief Set the cave count.
    /// @param num Cave count.
    void setCaveCount(const int& num);

    /// @brief Increment the cave number by the given amount.
    /// @param inc amount of caves to increment by.
    void incrementCaveNumber(const int& inc);

    /// @brief Decrement the cave number by the given amount.
    /// @param dec amount of caves to decrement by.
    void decrementCaveNumber(const int& dec);

    /// @brief Increment the cave number by 1.
    /// If on the final cave, send a signal that the game is completed
    void nextCave();

    /// @brief Get the remaining cave time.
    int getTime() const;

    /// @brief Force the cave timer to a specific value.
    /// @param time New time value.
    void setTime(const int& time);

    /// @brief Get the number of player lives remaining.
    int getLives() const;

    /// @brief Get the current score.
    int getScore() const;

    /// @brief Get the current cave hue setting.
    int getHue() const;

    /// @brief Get the current cave saturation setting.
    int getSat() const;

    /// @brief Get the current cave luminance setting.
    int getLum() const;

    /// @brief Retrieve the active cave properties.
    Cave::Properties getCaveProperties();

    /// @brief Replace the active cave properties.
    /// @param properties The new cave properties.
    void setCaveProperties(const Cave::Properties& properties);

    // ----------------------------------------------------------------------------------
    // Useful conditionals
    // ----------------------------------------------------------------------------------

    /// @brief Determine whether the game is currently paused.
    /// @return true if the game is paused, false otherwise
    bool isGamePaused() const;

    /// @brief Determine whether the game is completed.
    /// @return true if the game is completed, false otherwise
    bool isGameCompleted() const;

    /// @brief Determine whether the game is over.
    /// @return true if the game is over, false otherwise
    bool isGameOver() const;

    /// @brief Whether the game is on the main menu
    bool onMainMenu() const;

    /// @brief Get the current index of the chosen cave file.
    int getCaveFileIndex();

    // @brief Set the index of the cave file, from the list of cave files.
    void setCaveFileIndex(int caveFileIndex);

    /// @brief Marked as exited from a cave.
    /// @return true if the game has been marked as exited from a cave.
    bool markedAsExitedFromCave();

    /// @brief Check whether game is in developer mode.
    /// @return true if the game is in Developer mode, false otherwise
    bool developerMode() const;

    /// @brief Check if the required quota has been met.
    /// @return true if quota is reached, false otherwise.
    bool caveQuotaReached() const;

    /// @brief Check if time is below the warning threshold.
    /// @return true if time is running out.
    bool isTimeWarning() const;

    /// @brief Set the refresh rate.
    void setRefreshrate();

    /// @brief Retrieve the Game Options.
    GameSettings getGameOptions() const;

    /// @brief Commit the game options.
    /// @param GameSettings The current game options.
    void commitGameOptions(const GameSettings& options);

private:
    // ----------------------------------------------------------------------------------
    // Signal Handlers
    // ----------------------------------------------------------------------------------

    /// @brief Handle signal when the current game state is MainMenu.
    void handleMainMenu(const GameSignal& signal);

    /// @brief Handle signal when the current game state is CaveLoad.
    void handleCaveLoad(const GameSignal& signal);

    /// @brief Handle signal when the current game state is CavePlay.
    void handleCavePlay(const GameSignal& signal);

    /// @brief Handle signal when the current game state is CavePass.
    void handleCavePass(const GameSignal& signal);

    /// @brief Handle signal when the current game state is CaveFail.
    void handleCaveFail(const GameSignal& signal);

    /// @brief Handle signal when the current game state is GameDoned.
    void handleGameDone(const GameSignal& signal);

    /// @brief Handle signal when the current game state is GameOver.
    void handleGameOver(const GameSignal& signal);

    // ----------------------------------------------------------------------------------
    // Other Internal Logic
    // ----------------------------------------------------------------------------------

    /// @brief Run the main game.
    void mainGameLoop();

    /// @brief Update any game logic (called once per frame).
    void update();

    /// @rief Initial m_time value for the cave.
    int initialCaveTime() const;

    /// @brief Award extra lives if score crosses threshold.
    void checkExtraLife();

    /// @brief Reset cave-related state values to defaults.
    void resetCaveState();

    /// @brief Reset game-related state values to defaults.
    void resetGame();

public:
    ///@brief The current render window.
    sf::RenderWindow window;

    /// @brief Image manager used for loading images.
    Image::Manager imageManager = Image::Manager();

    /// @brief Sound manager used for audio playback.
    Sound::Manager soundManager = Sound::Manager();

    /// @brief Input system for player controls.
    Input::System inputSystem = Input::System();

private:
    // ----------------------------------------------------------------------------------
    // Game Tracking
    // ----------------------------------------------------------------------------------

    /// @brief Number of diamond collected in the current cave
    int m_collected;

    /// @brief Current cave number
    int m_caveNumber;

    /// @brief Number of caves
    int m_caveCount;

    /// @brief Remaining player lives
    int m_lives;

    /// @brief Next score threshold for awarding an extra life
    int m_extraLifeThreshold;

    /// @brief The cave timer countdown value
    int m_time;

    /// @brief Current player score
    int m_score;

    /// @brief Cave number chosen on main menu
    int m_chosenCaveNumber;

    /// @brief Whether the game is paused
    bool m_gameIsPaused;

    /// @brief Whether the game is in Developer mode
    bool m_developerMode = false;

    /// @brief Whether the cave is active
    bool m_caveActive = false;

    /// @brief Whether to load the cave HUD
    bool m_revealCaveHUD = false;

    /// @brief Whether to make cave HUD completely disappear
    bool m_makeHUDDisappear = false;

    /// @brief Whether to close the window
    bool m_closeWindow = false;

    /// @brief Whether to close the window
    bool m_exitedFromCave = false;

    // ----------------------------------------------------------------------------------
    // Dependencies
    // ----------------------------------------------------------------------------------

    /// @brief The current game state.
    GameState m_gameState = GameState::MainMenu;

    /// @brief The properties of the current cave.
    Cave::Properties m_caveProperties = Cave::Properties();

    /// @brief The properties of the current cave.
    GameSettings m_settings = GameSettings();

    /// @brief List of the cave file names.
    std::vector<std::string> m_caveFilenames;

    /// @brief The index of the chosen cave file, in @ref m_caveFilenames;
    int m_caveFileIndex;
};
