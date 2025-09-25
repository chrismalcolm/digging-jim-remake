#pragma once

namespace Cave {

    /**
     * @enum State
     * @brief Represents the different states of a cave during gameplay.
     */
    enum class State {
        /**
         * @brief The cave is currently being set up.
         *
         * @details
         * - Cave entity tiles start as loading tiles and transition to visible.
         * - The camera begins either in the top-left or bottom-right corner, depending on the start door.
         * - The camera pans toward Jim while tiles finish loading.
         * - Once all visible tiles are loaded, the cave enters the Intro state.
         */
        Load,

        /**
         * @brief The intro sequence after cave entity tiles have loaded.
         *
         * @details
         * - Cave entities become active.
         * - Jim opens the start door.
         * - When Jim finishes opening the start door, the cave transitions to Play.
         */
        Intro,

        /**
         * @brief The main gameplay state.
         *
         * @details
         * - All cave entities are active and interacting.
         * - If Jim collects the diamond quota and enters the locked door, the state becomes Pass.
         * - If Jim dies, the state becomes Fail.
         */
        Play,

        /**
         * @brief The paused gameplay state.
         *
         * @details
         * - Triggered by pressing the pause button.
         * - All cave entities are inactive while paused.
         * - If Jim is dead, the player cannot pause.
         */
        Pause,

        /**
         * @brief The passed gameplay state.
         *
         * @details
         * - Jim has completed the cave.
         * - All cave entities remain active and interacting.
         * - The camera is fixed where Jim finished the cave.
         * - Once the final score is calculated, the next cave loads.
         */
        Pass,

        /**
         * @brief The failed gameplay state.
         *
         * @details
         * - Jim has died.
         * - Other cave entities remain active and interacting.
         * - The camera is fixed on Jim’s death location.
         * - Once Enter is pressed, the cave transitions to End.
         */
        Fail,

        /**
         * @brief The cave unloading state.
         *
         * @details
         * - Cave entity tiles transition to unloading tiles and eventually back to loading tiles.
         * - When no visible tiles remain:
         *   - If the player has lives remaining, the current cave reloads.
         *   - Otherwise, the game is over.
         */
        End,
    };
}
