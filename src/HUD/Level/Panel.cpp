#include <algorithm>
#include "HUD/Level/Panel.h"

HUD::Level::Panel::Panel(Game* game)
    : m_game(game), m_textRenderer(game, 23), m_pauseTextRenderer(game, 6), m_displayCaveNumber(m_game->getCaveNumber())
{
    m_panelHeight = -64;
    m_panelRevealed = false;
}

void HUD::Level::Panel::load() {
    m_caveHUD = m_game->imageManager.getTexture(Image::Texture::CaveHUD);
    m_caveHUD.setSmooth(true);

    m_textRenderer.load(Image::Texture::CaveNumbers, { 16, 32 });
    m_pauseTextRenderer.load(Image::Texture::GameFont, { 16, 32 });
}

void HUD::Level::Panel::reveal() {
    m_panelRevealed = true;
}

void HUD::Level::Panel::hide() {
    m_panelRevealed = false;
}

void HUD::Level::Panel::disappear() {
    m_panelRevealed = false;
    m_panelHeight = -64;
}

void HUD::Level::Panel::update(const Camera& camera) {
    sf::Vector2f viewSize = camera.getSize();
    sf::Vector2f viewCenter = camera.getCenter();
    sf::Vector2f topLeft = viewCenter - viewSize / 2.f;
    topLeft.y -= m_panelHeight;
    if (m_panelRevealed && m_panelHeight < 0) m_panelHeight++;
    if (!m_panelRevealed && m_panelHeight > -64) m_panelHeight--;

    float padding = 0.f;

    // Size of the HUD image
    sf::Vector2f hudSize(static_cast<float>(m_caveHUD.getSize().x), static_cast<float>(m_caveHUD.getSize().y));

    // Bottom-left corner position with padding
    sf::Vector2f position(
        topLeft.x + padding,
        topLeft.y + viewSize.y - hudSize.y - padding
    );

    // Resize and define the triangle list (6 vertices = 2 triangles)
    m_vertices.setPrimitiveType(sf::PrimitiveType::Triangles);
    m_vertices.resize(6);

    // Vertex positions (clockwise from top-left)
    topLeft = position;
    sf::Vector2f topRight = position + sf::Vector2f(hudSize.x, 0.f);
    sf::Vector2f bottomRight = position + hudSize;
    sf::Vector2f bottomLeft = position + sf::Vector2f(0.f, hudSize.y);

    // Texture coordinates match positions
    sf::Vector2f texTopLeft = sf::Vector2f(0.f, 0.f);
    sf::Vector2f texTopRight = sf::Vector2f(hudSize.x, 0.f);
    sf::Vector2f texBottomRight = sf::Vector2f(hudSize.x, hudSize.y);
    sf::Vector2f texBottomLeft = sf::Vector2f(0.f, hudSize.y);

    // Triangle 1: top-left, top-right, bottom-right
    m_vertices[0].position = topLeft;
    m_vertices[0].texCoords = texTopLeft;

    m_vertices[1].position = topRight;
    m_vertices[1].texCoords = texTopRight;

    m_vertices[2].position = bottomRight;
    m_vertices[2].texCoords = texBottomRight;

    // Triangle 2: top-left, bottom-right, bottom-left
    m_vertices[3].position = topLeft;
    m_vertices[3].texCoords = texTopLeft;

    m_vertices[4].position = bottomRight;
    m_vertices[4].texCoords = texBottomRight;

    m_vertices[5].position = bottomLeft;
    m_vertices[5].texCoords = texBottomLeft;

    if (m_previousCollected < m_game->getCollected()) m_diamondHighlight = 3;
    m_previousCollected = m_game->getCollected();

    if (m_diamondHighlight > 0 && m_TickCounter.onTick()) m_diamondHighlight--;
    int dH = m_diamondHighlight <= 3 ? m_diamondHighlight : 6 - m_diamondHighlight;

    if (m_previousLives < m_game->getLives()) m_livesHighlight = 6;
    m_previousLives = m_game->getLives();
    
    if (m_livesHighlight > 0 && m_TickCounter.onTick()) m_livesHighlight--;
    int lH = m_livesHighlight <= 3 ? m_livesHighlight : 6 - m_livesHighlight;

    if (m_previousPoints < m_game->getScore()) m_pointsHighlight = 3;
    m_previousPoints = m_game->getScore();

    if (m_pointsHighlight > 0 && m_TickCounter.onTick()) m_pointsHighlight--;
    int pH = m_pointsHighlight <= 3 ? m_pointsHighlight : 6 - m_pointsHighlight;

    if (m_TickCounter.onTick()) {
        if (m_game->isTimeWarning()) {
            if (m_timeHighlight >= 3) m_timeHighlightIncrease = -1;
            else if (m_timeHighlight <= 0) m_timeHighlightIncrease = 1;
            m_timeHighlight += m_timeHighlightIncrease;
        }
        else {
            if (m_timeHighlight > 0) m_timeHighlight--;
            if (m_timeHighlight < 0) m_timeHighlight = 0;
        }
    }

    if (!m_game->onMainMenu()) m_displayCaveNumber = m_game->getCaveNumber();

    m_textRenderer.updateNumbers({
        {{ static_cast<int>(topLeft.x + 16), static_cast<int>(topLeft.y + 16) }, m_game->getDiamondValue(), 2, 0},
        {{ static_cast<int>(topLeft.x + 80), static_cast<int>(topLeft.y + 16) }, m_game->getCollected(), 3, dH},
        {{ static_cast<int>(topLeft.x + 160), static_cast<int>(topLeft.y + 16) }, m_game->getQuota(), 3, 0},
        {{ static_cast<int>(topLeft.x + 256), static_cast<int>(topLeft.y + 16) }, m_displayCaveNumber, 3, 0},
        {{ static_cast<int>(topLeft.x + 352), static_cast<int>(topLeft.y + 16) }, m_game->getTime(), 3, m_timeHighlight},
        {{ static_cast<int>(topLeft.x + 448), static_cast<int>(topLeft.y + 16) }, m_game->getLives(), 2, lH},
        {{ static_cast<int>(topLeft.x + 512), static_cast<int>(topLeft.y + 16) }, m_game->getScore(), 7, pH}
        });

    m_pauseTextRenderer.updateText(viewCenter.x - 48, viewCenter.y - 48, "Paused");
}


void HUD::Level::Panel::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    
    states.transform *= getTransform();
    states.texture = &m_caveHUD;
    target.draw(m_vertices, states);

    m_textRenderer.render(target, states);
    if (m_game->isGamePaused()) m_pauseTextRenderer.render(target, states);
}
