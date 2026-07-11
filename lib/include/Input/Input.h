#pragma once

#include <SFML/Window.hpp>
#include <unordered_map>
#include <set>

namespace Input {

    /// @brief Represents all possible input actions supported by the game.
    enum class Action {
        /// @brief Move Jim up.
        MoveUp,
        /// @brief Move Jim down.
        MoveDown,
        /// @brief Move Jim left.
        MoveLeft,
        /// @brief Move Jim right.
        MoveRight,
        /// @brief Confirmation for navigation.
        Confirm,
        /// @brief Whilst held, Jim will be in collect mode.
        Collect,
        /// @brief Pause the game.
        Pause,
        /// @brief Trigger Jim self-destruction.
        SelfDestruct,
        /// @brief Quit the game.
        Quit,

        /// @brief Activate cheat mode.
        ActivateCheatMode,
        /// @brief Restart the current cave.
        RestartCave,
        /// @brief Advance to the next cave.
        ForwardCave,
        /// @brief Return to the previous cave.
        BackwardCave,

        /// @brief Move the camera upward.
        CameraUp,
        /// @brief Move the camera downward.
        CameraDown,
        /// @brief Move the camera leftward.
        CameraLeft,
        /// @brief Move the camera rightward.
        CameraRight,
        /// @brief Reset the camera position.
        CameraReset,

        /// @brief Perform a left mouse click.
        MouseLeftClick,
        /// @brief Perform a right mouse click.
        MouseRightClick,

        /// @brief Toggle the debug properties panel.
        ToggleEditorProperties,
        /// @brief Toggle the editor panel.
        ToggleEditorPanel
    };

    /**
     * @class System
     * 
     * @brief Handles mapping, state tracking, and processing of player/developer inputs.
     */
    class System {
    public:
        /**
         * @brief Process a single SFML event and update the input state accordingly.
         *
         * Handles keyboard and mouse press/release events, updating the pressed
         * and held action sets in the input system.
         * If the joystick controls are toggled, all keyboard and mouse presses are ignored.
         *
         * @param event The SFML event to process.
         */
        void handleEvent(const sf::Event& event);

        /**
         * @brief Handle joystick input for movement and actions.
         *
         * Polls the given joystick for axis movement and button presses.
         * Updates both the pressed and held action sets accordingly.
         *
         * @note If joystick input is disabled (see @ref m_joystick) or the joystick is
         *       not connected, or a keyboard event occurred @ref (m_keyboardEventOccurred)
         *       this function returns immediately without modifying input state.
         */
        void handleJoystick();

        /**
         * @brief Update the input state for the new frame.
         *
         * Clears all one-frame pressed actions, and resets the push timer if
         * no movement actions are being held.
         */
        void update();

        /**
         * @brief Check if an action is currently being held down.
         *
         * @param action The action to query.
         *
         * @return true if the action is being held, false otherwise.
         */
        bool isPressed(Input::Action action) const;

        /**
         * @brief Check if an action was pressed during the current frame.
         *
         * Unlike isPressed(), this ignores actions already held and only
         * reports newly pressed inputs.
         *
         * @param action The action to query.
         * @return true if the action was pressed this frame, false otherwise.
         */
        bool wasPressed(Input::Action action) const;

        /**
         * @brief Increase the push timer counter.
         *
         * Called when Jim attempts to push an object, tracking the number
         * of consecutive frames spent pushing.
         */
        void increasePushTimer();

        /**
         * @brief Check if the current push duration qualifies as a registered push.
         *
         * A push is considered registered if the push timer has been active
         * for at least 8 frames, and occurs on multiples of 8 thereafter.
         *
         * @return true if a push is registered, false otherwise.
         */
        bool registerPush() const;

        /**
         * @brief Toggle whether to use joystick controls.
         */
        void setJoystick(const bool& toggle);

        /**
         * @brief Get the joystick ID (0 - 7 for SFML).
         */
        void detectJoystick();

    private:
        /// @brief Mapping of keyboard keys to actions.
        std::unordered_map<sf::Keyboard::Scan, Action> m_keyMap = {
            // Developer buttons
            {sf::Keyboard::Scan::F1,  Input::Action::ForwardCave},
            {sf::Keyboard::Scan::F2,  Input::Action::RestartCave},
            {sf::Keyboard::Scan::F3,  Input::Action::BackwardCave},
            {sf::Keyboard::Scan::F4,  Input::Action::ToggleEditorProperties},
            {sf::Keyboard::Scan::F5,  Input::Action::ToggleEditorPanel},
            {sf::Keyboard::Scan::F6,  Input::Action::CameraLeft},
            {sf::Keyboard::Scan::F7,  Input::Action::CameraUp},
            {sf::Keyboard::Scan::F8,  Input::Action::CameraDown},
            {sf::Keyboard::Scan::F9,  Input::Action::CameraRight},
            {sf::Keyboard::Scan::F10, Input::Action::CameraReset},
            {sf::Keyboard::Scan::F11, Input::Action::ActivateCheatMode},

            // Player controls
            {sf::Keyboard::Scan::Up,      Input::Action::MoveUp},
            {sf::Keyboard::Scan::Down,    Input::Action::MoveDown},
            {sf::Keyboard::Scan::Left,    Input::Action::MoveLeft},
            {sf::Keyboard::Scan::Right,   Input::Action::MoveRight},
            {sf::Keyboard::Scan::Enter,   Input::Action::Confirm},
            {sf::Keyboard::Scan::Space,   Input::Action::Collect},
            {sf::Keyboard::Scan::P,       Input::Action::Pause},
            {sf::Keyboard::Scan::Tab,     Input::Action::SelfDestruct},
            {sf::Keyboard::Scan::Escape,  Input::Action::Quit},

            // Additional player controls
            {sf::Keyboard::Scan::W, Input::Action::MoveUp},
            {sf::Keyboard::Scan::A, Input::Action::MoveLeft},
            {sf::Keyboard::Scan::S, Input::Action::MoveDown},
            {sf::Keyboard::Scan::D, Input::Action::MoveRight},
        };


        /// @brief Mapping of mouse buttons to actions.
        std::unordered_map<sf::Mouse::Button,Action> m_mouseMap = {
            {sf::Mouse::Button::Left,  Input::Action::MouseLeftClick},
            {sf::Mouse::Button::Right, Input::Action::MouseRightClick},
        };

        /// @brief Actions that were pressed during this frame.
        std::set<Input::Action> m_pressedActions;

        /// @brief Actions that are currently being held down.
        std::set<Input::Action> m_heldActions;

        /// @brief Whether a keyboard event occurred before the next update.
        bool m_keyboardEventOccurred = false;

        /// @brief Timer for tracking Jim's push timing.
        unsigned int pushTimer = 0;

        /// @brief Mapping of joystick buttons to actions.
        std::unordered_map<unsigned int, Action> m_joystickButtonMap = {
            {0, Input::Action::Confirm},      // A / Cross
            {1, Input::Action::SelfDestruct}, // B / Circle
            {2, Input::Action::Collect},      // X / Square
            {6, Input::Action::Quit},         // Back / Select
            {7, Input::Action::Pause},        // Start
        };

        /// @brief Whether to use JoyStick controls.
        bool m_joystick = false;

        /// @brief ID of the joystick (0 - 7 for SFML).
        int m_joystickId = -1;

        /// @brief Deadzone region of the joystick.
        float m_deadzone = 0.25f;

        /// @brief Joystick axis for movement.
        sf::Joystick::Axis m_horizontalAxis = sf::Joystick::Axis::X;
        sf::Joystick::Axis m_verticalAxis = sf::Joystick::Axis::Y;
    };
}
