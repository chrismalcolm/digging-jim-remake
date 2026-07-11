#include "HUD/Developer/CameraCenter.h"
#include <sstream>
#include <iomanip>

HUD::Developer::CameraCenter::CameraCenter()
{
    m_marker.setFillColor(sf::Color::Transparent);
    m_marker.setOutlineColor(sf::Color::Red);
    m_marker.setOutlineThickness(2.f);
    m_marker.setSize({ 32.f, 32.f });
    m_marker.setPosition(sf::Vector2f(0.f, 0.f));
}

void HUD::Developer::CameraCenter::update(Camera Camera, sf::Vector2f& position) {
    position.y -= 16.f;
    m_marker.setPosition(position);
}

void HUD::Developer::CameraCenter::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(m_marker, states);
}
