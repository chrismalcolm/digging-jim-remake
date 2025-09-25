#include "HUD/Developer/PositionDisplay.h"
#include <sstream>

const sf::Font arialFont(".\\assets\\fonts\\arial.ttf");

HUD::Developer::PositionDisplay::PositionDisplay() : m_positionText(arialFont)
{
    m_positionText.setCharacterSize(14);
    m_positionText.setFillColor(sf::Color::White);

    m_background.setFillColor(sf::Color(0, 0, 0, 200));
    m_background.setSize({ 100.f, 20.f });
}

void HUD::Developer::PositionDisplay::update(const Camera& camera, const std::string& name, const sf::Vector2f& position, const float& yOffset)
{
    std::ostringstream ss;
    ss << name << " X: " << static_cast<int>(position.x)
        << " Y: " << static_cast<int>(position.y);
    m_positionText.setString(ss.str());

    // Adjust background size based on text
    m_background.setSize({
        m_positionText.getLocalBounds().size.x + 10.f,
        m_positionText.getLocalBounds().size.y + 10.f
        });

    // Position HUD in top-left corner of the camera
    sf::Vector2f topLeft = camera.getCenter() - camera.getSize() / 2.f;
    m_background.setPosition(topLeft + sf::Vector2f(0.f, 25.f + yOffset));
    m_positionText.setPosition(topLeft + sf::Vector2f(5.f, 28.f + yOffset));
}

void HUD::Developer::PositionDisplay::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(m_background, states);
    target.draw(m_positionText, states);
}
