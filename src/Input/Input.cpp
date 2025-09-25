#include "Input/Input.h"

void Input::System::handleEvent(const sf::Event& event) {

    if (const auto keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        for (auto& [key, action] : m_keyMap) {
            if (keyPressed->scancode == key) {
                m_pressedActions.insert(action);
                m_heldActions.insert(action);
                m_keyboardEventOccurred = true;
            }
        }
    }
    if (const auto keyPressed = event.getIf<sf::Event::KeyReleased>()) {
        for (auto& [key, action] : m_keyMap) {
            if (keyPressed->scancode == key) {
                m_heldActions.erase(action);
                m_keyboardEventOccurred = true;
            }
        }
    }
    if (const auto mousePressed = event.getIf<sf::Event::MouseButtonPressed>()) {
        for (const auto& [button, action] : m_mouseMap) {
            if (mousePressed->button == button) {
                m_pressedActions.insert(action);
                m_heldActions.insert(action);
                m_keyboardEventOccurred = true;
            }
        }
    }
    if (const auto mouseReleased = event.getIf<sf::Event::MouseButtonReleased>()) {
        for (const auto& [button, action] : m_mouseMap) {
            if (mouseReleased->button == button) {
                m_heldActions.erase(action);
                m_keyboardEventOccurred = true;
            }
        }
    }
}

void Input::System::handleJoystick() {
    if (!m_joystick || !sf::Joystick::isConnected(m_joystickId) || m_keyboardEventOccurred) return;

    float x = sf::Joystick::getAxisPosition(m_joystickId, m_horizontalAxis);
    float y = sf::Joystick::getAxisPosition(m_joystickId, m_verticalAxis);

    if (x < -m_deadzone) {
        if (!isPressed(Action::MoveLeft)) m_pressedActions.insert(Action::MoveLeft);
        m_heldActions.insert(Action::MoveLeft);
    }
    else m_heldActions.erase(Action::MoveLeft);

    if (x > m_deadzone) {
        if (!isPressed(Action::MoveRight)) m_pressedActions.insert(Action::MoveRight);
        m_heldActions.insert(Action::MoveRight);
    }
    else m_heldActions.erase(Action::MoveRight);

    if (y < -m_deadzone) {
        if (!isPressed(Action::MoveUp)) m_pressedActions.insert(Action::MoveUp);
        m_heldActions.insert(Action::MoveUp);
    }
    else m_heldActions.erase(Action::MoveUp);

    if (y > m_deadzone) {
        if (!isPressed(Action::MoveDown)) m_pressedActions.insert(Action::MoveDown);
        m_heldActions.insert(Action::MoveDown);
    }
    else m_heldActions.erase(Action::MoveDown);

    for (auto& [button, action] : m_joystickButtonMap) {
        if (sf::Joystick::isButtonPressed(m_joystickId, button)) {
            if (!isPressed(action)) {
                m_pressedActions.insert(action);
            }
            m_heldActions.insert(action);
        }
        else {
            m_heldActions.erase(action);
        }
    }
}

void Input::System::update() {
    bool movementPressed = (isPressed(Input::Action::MoveLeft)
        || isPressed(Input::Action::MoveRight)
        || isPressed(Input::Action::MoveUp)
        || isPressed(Input::Action::MoveDown)
        );
    if (!movementPressed) {
        pushTimer = 0;
    }
    m_pressedActions.clear();
    m_keyboardEventOccurred = false;
}

bool Input::System::isPressed(Input::Action action) const {
    return m_heldActions.count(action);
}

bool Input::System::wasPressed(Input::Action action) const {
    return m_pressedActions.count(action);
}

void Input::System::increasePushTimer() {
    pushTimer++;
}

bool Input::System::registerPush() const {
    if (pushTimer < 8) {
        return false;
    }
    return pushTimer % 8 == 0;
}

void Input::System::setJoystick(const bool& toggle) {
    m_joystick = toggle;
    if (m_joystick) detectJoystick();
}

void Input::System::detectJoystick() {
    m_joystickId = -1;
    for (unsigned int id = 0; id < sf::Joystick::Count; ++id) {
        if (sf::Joystick::isConnected(id)) {
            m_joystickId = static_cast<int>(id);
            break;
        }
    }
}