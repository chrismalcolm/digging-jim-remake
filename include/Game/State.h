#pragma once

/// @brief Represents the current state of the game.
enum class GameState {
    /**
     * @brief The game's main menu.
     *
     * The player can navigate the main menu.
     * From here, the player can select a starting cave, choose a .cav file or adjust various options.
     * The player can choose to start the game on the starting cave, or exit.
     * The main menu soundtrack will run on a loop.
     */
    MainMenu,

    /**
     * @brief Cave is loading or resetting.
     *
     * All cave tiles and cave properties are loaded.
     * If not visible, the game panel should appear from the bottom of the screen until fully visible.
     * None of the cave tiles start of visible, as they are all hidden behind loading tiles.
     * This loading tiles will disappear randomly to eventually reveal the cave tiles.
     * The camera pans to focuses on the start door location.
     * If the start door is in the top half of the cave, the camera begins bottom right corner.
     * Otherwise, the camera will start in the top left corner of the cave.
     * Once the camera has focused on the start door and the loading tiles have all disappeared,
     * the start door will open and Jim will appear.
     * The GameSignal::CaveStart signal should be sent to transition into the CavePlay state.
    */
    CaveLoad,

    /**
     * @brief Main gameplay mode is active.
     *
     * The player is able to control Jim and interact with the cave entities.
     * The goal is to collect the diamond quota and reach the exit door to complete the cave.
     * If this goal is reached, a GameSignal::CavePass signal should be sent to transition into the CavePass state.
     * If Jim is killed, a GameSignal::CaveFail signal should be sent to transition into the CaveFail state.
     * The player can also pause and unpause the game in this state.
     * These can be triggered by the GameSignal::CavePause and GameSignal::CaveUnpause signals respectively.
     */
    CavePlay,

    /**
     * @brief Cave was successfully completed.
     *
     * The player has successfully collected the diamond quota and reached the exit door.
     * And time remaining will be added onto the total score.
     * Once completed, the game will either transition onto the next cave,
     * or if the last cave was completed, transition to the GameDone state.
     */
    CavePass,

    /**
     * @brief The player failed to complete the cave.
     *
     * Jim was killed and hence the player has failed to complete the cave.
     * A life will be deducted from the players lives.
     * If the number of lives reaches 0, the game will transition to the GameOver state.
     * Otherwise, the cave will be restarted, and transition to the CaveLoad state.
     */
    CaveFail,

    /**
     * @brief The player has completed the game.
     *
     * The player has completed the final cave and has hence completed the game.
     * A Game Completed panel will be shown, before transitioning to the MainMenu state.
     */
    GameDone,

    /**
     * @brief The player has no lives left.
     *
     * The player has failed the cave and has no lives left to continue.
     * A Game Over panel will be shown, before transitioning to the MainMenu state.
     */
    GameOver
};