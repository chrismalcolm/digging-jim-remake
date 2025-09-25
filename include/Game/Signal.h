#pragma once

/// @brief Signals that trigger game actions and state transitions.
enum class GameSignal {
    /// @brief Select Play in the main menu.
    PlayGame,

    /// @brief Change to the previous cave (if possible).
    PreviousCave,

    /// @brief Change to the next cave (if possible).
    NextCave,

    /// @brief Start the music.
    StartMusic,

    /// @brief Stop the music.
    StopMusic,

    /// @brief Set the window FPS.
    SetRefreshRate,

    /// @brief Load initial cave.
    CaveBegin,

    /// @brief Load or reset the cave.
    CaveLoad,

    /// @brief Begin play in the current cave.
    CaveStart,

    /// @brief Mark the cave as completed.
    CavePass,

    /// @brief Mark the cave as failed.
    CaveFail,

    /// @brief Player collected a diamond.
    CollectDiamond,

    /// @brief Player has paused the game.
    CavePause,

    /// @brief Player has unpaused the game.
    CaveUnpause,

    /// @brief Player has completed the game!
    GameCompleted,

    /// @brief Go back to the main menu.
    GotoMainMenu,

    /// @brief Exit the game
    ExitGame
};