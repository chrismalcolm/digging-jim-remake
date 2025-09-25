#include "Camera/Camera.h"
#include <algorithm>

Camera::Camera(sf::Vector2f screenSize, sf::Vector2f bounds)
    : m_view({ 0, 0 }, screenSize), m_bounds(bounds) {
    setCentre({ 0, 0 });
}

Camera::Camera(sf::Vector2f screenSize, sf::Vector2f location, sf::Vector2f bounds)
    : m_view(location, screenSize), m_bounds(bounds) {
    setCentre(location);
}

sf::View& Camera::view() {
    return m_view;
}

sf::Vector2f Camera::getSize() const {
    return m_view.getSize();
}

sf::Vector2f Camera::getCenter() const {
    return m_view.getCenter();
}

void Camera::setCentre(sf::Vector2f location) {
    sf::Vector2f halfSize = { getSize().x / 2.f , getSize().y / 2.f };

    // Clamp location so camera does not move outside world bounds
    location.x = std::clamp(location.x, halfSize.x, m_bounds.x - halfSize.x);
    location.y = std::clamp(location.y, halfSize.y, m_bounds.y - halfSize.y);

    m_view.setCenter(location);
}

void Camera::setBounds(sf::Vector2f bounds) {
    m_bounds = bounds;
}
