#include <algorithm>
#include <cmath>
#include <memory>
#include "Renderer/TextRenderer.h"

Renderer::TextRenderer::TextRenderer(Game* game, const size_t length)
    : m_game(game), m_length(length) {}

bool Renderer::TextRenderer::load(const Image::Texture& texture, const sf::Vector2u& tileSize) {
    m_tileset = m_game->imageManager.getTexture(texture);

    // All text textures should be smooth
    m_tileset.setSmooth(true);
    m_tilesize = tileSize;
    m_vertices.setPrimitiveType(sf::PrimitiveType::Triangles);
    m_vertices.resize(m_length * 6);
    return true;
}

void Renderer::TextRenderer::updateNumbers(const std::vector<NumberItem>& items) {

    // Prepare a digit for each number item in buffer.
    int index = 0;
    for (const auto& item : items) {
        int n = item.value;

        for (int x = item.digits - 1; x >= 0; --x) {

            sf::Vector2i textureCoords = { n % 10 + item.highlight * 10, 0 };
            n /= 10;

            sf::IntRect ep = { { 0, 0 }, { static_cast<int>(m_tilesize.x), static_cast<int>(m_tilesize.y) } };
            sf::IntRect tp = { { 0, 0 }, { static_cast<int>(m_tilesize.x), static_cast<int>(m_tilesize.y) } };

            // Position
            float px1 = x * m_tilesize.x + item.position.x + ep.position.x;
            float py1 = item.position.y + ep.position.y;
            float px2 = x * m_tilesize.x + item.position.x + ep.position.x + ep.size.x;
            float py2 = item.position.y + ep.position.y + ep.size.y;

            // Texture co-ords
            float tx1 = textureCoords.x * m_tilesize.x + tp.position.x;
            float ty1 = textureCoords.y * m_tilesize.y + tp.position.y;
            float tx2 = textureCoords.x * m_tilesize.x + tp.position.x + tp.size.x;
            float ty2 = textureCoords.y * m_tilesize.y + tp.position.y + tp.size.y;

            // Pointer to the 6 vertices for this tile
            sf::Vertex* tri = &m_vertices[index * 6];

            // First triangle
            tri[0].texCoords = { tx1, ty1 };
            tri[1].texCoords = { tx2, ty1 };
            tri[2].texCoords = { tx2, ty2 };

            // Second triangle
            tri[3].texCoords = { tx1, ty1 };
            tri[4].texCoords = { tx2, ty2 };
            tri[5].texCoords = { tx1, ty2 };

            // First triangle
            tri[0].position = { px1, py1 };
            tri[1].position = { px2, py1 };
            tri[2].position = { px2, py2 };

            // Second triangle
            tri[3].position = { px1, py1 };
            tri[4].position = { px2, py2 };
            tri[5].position = { px1, py2 };

            index++;
        }
    }
}

void Renderer::TextRenderer::updateText(const int& x, const int& y, const std::string& text) {
    /*
     * Notes for special glyphs
     * \u00C6 -> Æ
     * \u00D8 -> Ø
     * \u00C5 -> Å
     * \u00E6 -> æ
     * \u00F8 -> ø
     * \u00E5 -> å
     */
    static const std::string charset = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz+,-.!:*?\u00C6\u00D8\u00C5\u00E6\u00F8\u00E5/ ";

    int index = 0;

    for (int i = 0; i < m_length; ++i) {
        char c = (i < text.size()) ? text[i] : ' ';

        // Find character index in charset
        size_t charIndex = charset.find(c);
        if (charIndex == std::string::npos) {
            charIndex = charset.find(' ');
            if (charIndex == std::string::npos) continue;
        }

        // Overlay is asssumed to be 40
        sf::Vector2i textureCoords = {
            static_cast<int>(charIndex) % 40,
            static_cast<int>(charIndex) / 40
        };

        // Size is assumed to be { 16, 32 }
        sf::IntRect ep = { { 0, 0 }, { 16, 32 } };
        sf::IntRect tp = { { 0, 0 }, { 16, 32 } };

        // Position
        float px1 = i * m_tilesize.x + x + ep.position.x;
        float py1 = y + ep.position.y;
        float px2 = i * m_tilesize.x + x + ep.position.x + ep.size.x;
        float py2 = y + ep.position.y + ep.size.y;

        // Texture co-ords
        float tx1 = textureCoords.x * m_tilesize.x + tp.position.x;
        float ty1 = textureCoords.y * m_tilesize.y + tp.position.y;
        float tx2 = textureCoords.x * m_tilesize.x + tp.position.x + tp.size.x;
        float ty2 = textureCoords.y * m_tilesize.y + tp.position.y + tp.size.y;

        sf::Vertex* tri = &m_vertices[index * 6];

        // First triangle
        tri[0].texCoords = { tx1, ty1 };
        tri[1].texCoords = { tx2, ty1 };
        tri[2].texCoords = { tx2, ty2 };

        // Second triangle
        tri[3].texCoords = { tx1, ty1 };
        tri[4].texCoords = { tx2, ty2 };
        tri[5].texCoords = { tx1, ty2 };

        // First triangle
        tri[0].position = { px1, py1 };
        tri[1].position = { px2, py1 };
        tri[2].position = { px2, py2 };

        // Second triangle
        tri[3].position = { px1, py1 };
        tri[4].position = { px2, py2 };
        tri[5].position = { px1, py2 };

        index++;
    }
    
}

void Renderer::TextRenderer::updateMovingText(const int& x, const int& y, const std::string& text) {
    /*
     * Notes for special glyphs
     * \u00C6 -> Æ
     * \u00D8 -> Ø
     * \u00C5 -> Å
     * \u00E6 -> æ
     * \u00F8 -> ø
     * \u00E5 -> å
     */
    static const std::string charset = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz+,-.!:*?\u00C6\u00D8\u00C5\u00E6\u00F8\u00E5/ ";

    int index = 0;

    for (int i = 0; i < m_length; ++i) {
        char c = (i < text.size()) ? text[i] : ' ';

        // Get yOffset for moving text
        int xPos = x + 1.25 * i;
        float yOffset = 16.f * std::sin(((xPos % 90) / 90.f) * 3.14159265358979323846f * 2.f);

        // Find character index in charset
        size_t charIndex = charset.find(c);
        if (charIndex == std::string::npos) {
            charIndex = charset.find(' ');
            if (charIndex == std::string::npos) continue;
        }

        // Overlay is asssumed to be 40
        sf::Vector2i textureCoords = {
            static_cast<int>(charIndex) % 40,
            static_cast<int>(charIndex) / 40
        };

        // Size is assumed to be { 16, 32 }
        sf::IntRect ep = { { 0, 0 }, { 16, 32 } };
        sf::IntRect tp = { { 0, 0 }, { 16, 32 } };

        // Position
        float px1 = i * m_tilesize.x + static_cast<float>(x) + ep.position.x;
        float py1 = y + ep.position.y + yOffset;
        float px2 = i * m_tilesize.x + static_cast<float>(x) + ep.position.x + ep.size.x;
        float py2 = y + ep.position.y + ep.size.y + yOffset;

        // Texture co-ords
        float tx1 = textureCoords.x * m_tilesize.x + tp.position.x;
        float ty1 = textureCoords.y * m_tilesize.y + tp.position.y;
        float tx2 = textureCoords.x * m_tilesize.x + tp.position.x + tp.size.x;
        float ty2 = textureCoords.y * m_tilesize.y + tp.position.y + tp.size.y;

        sf::Vertex* tri = &m_vertices[index * 6];

        // First triangle
        tri[0].texCoords = { tx1, ty1 };
        tri[1].texCoords = { tx2, ty1 };
        tri[2].texCoords = { tx2, ty2 };

        // Second triangle
        tri[3].texCoords = { tx1, ty1 };
        tri[4].texCoords = { tx2, ty2 };
        tri[5].texCoords = { tx1, ty2 };

        // First triangle
        tri[0].position = { px1, py1 };
        tri[1].position = { px2, py1 };
        tri[2].position = { px2, py2 };

        // Second triangle
        tri[3].position = { px1, py1 };
        tri[4].position = { px2, py2 };
        tri[5].position = { px1, py2 };

        index++;
    }

}

void Renderer::TextRenderer::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();
    states.texture = &m_tileset;
    target.draw(m_vertices, states);
}

void Renderer::TextRenderer::render(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();
    states.texture = &m_tileset;
    target.draw(m_vertices, states);
}