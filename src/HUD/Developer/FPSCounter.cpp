#include "HUD/Developer/FPSCounter.h"
#include <sstream>
#include <iomanip>

const sf::Font arialFont(".\\assets\\fonts\\arial.ttf");

HUD::Developer::FPSCounter::FPSCounter() : m_fpsText(arialFont)
{
    m_fpsText.setCharacterSize(14);
    m_fpsText.setFillColor(sf::Color::White);
    m_fpsText.setPosition(sf::Vector2f(5.f, 3.f));

    m_background.setFillColor(sf::Color(0, 0, 0, 200));
    m_background.setSize({ 60.f, 20.f });
    m_background.setPosition(sf::Vector2f(0.f, 0.f));
}

void HUD::Developer::FPSCounter::update(Camera Camera)
{
    m_frameCount++;
    m_updateTimer += m_clock.restart().asSeconds();

    if (m_updateTimer >= 0.5f)
    {
        float fps = static_cast<float>(m_frameCount) / m_updateTimer;
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(1) << "FPS: " << fps;
        m_fpsText.setString(ss.str());

        m_background.setSize({ m_fpsText.getLocalBounds().size.x + 10.f, 20.f});

        m_frameCount = 0;
        m_updateTimer = 0.f;
    }

    // Position HUD in top-left corner of the current camera view
    sf::Vector2f topLeft = Camera.getCenter() - Camera.getSize() / 2.f;

    m_background.setPosition(topLeft);
    m_fpsText.setPosition(topLeft + sf::Vector2f(5.f, 3.f));
}

void HUD::Developer::FPSCounter::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(m_background, states);
    target.draw(m_fpsText, states);
}
