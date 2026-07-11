#include <algorithm>
#include <memory>
#include "Renderer/TileRenderer.h"
#include "Cave/Entity/Base.h"

Renderer::TileRenderer::TileRenderer(Image::Manager* imageManager): m_imageManager(imageManager) {}

bool Renderer::TileRenderer::load(const Image::Texture& texture, const sf::Vector2u& tileSize) {
    m_tileset = m_imageManager->getTexture(texture);

    m_tilesize = tileSize;
    m_vertices.setPrimitiveType(sf::PrimitiveType::Triangles);
    m_tilesetCols = m_tileset.getSize().x / tileSize.x;

    return true;
}

void Renderer::TileRenderer::updateTexture(const std::vector<Cave::Entity::Base>& entities, const sf::Vector2i& position, const sf::IntRect& gridRange, int gap) {
    int index = 0;
    m_vertices.resize(gridRange.size.x * gridRange.size.y * 12);
    for (int y = gridRange.position.y; y < gridRange.position.y + gridRange.size.y; ++y) {
        for (int x = gridRange.position.x; x < gridRange.position.x + gridRange.size.x; ++x) {

            const Cave::Entity::Base& entity = entities[index];

            int tileIndex = entity.getCurrentTextureIndex();
            if (tileIndex == Cave::Entity::NO_TEXTURE_INDEX) {
                index++;
                continue;
            }
            sf::Vector2i textureCoords = { tileIndex % m_tilesetCols, tileIndex / m_tilesetCols };

            sf::IntRect ep = entity.getCurrentPosition();
            sf::IntRect tp = entity.getCurrentTextureCoords();

            float baseX = (gap == 0)
                ? (float)(x * (int)m_tilesize.x + position.x)
                : (float)((x - gridRange.position.x) * ((int)m_tilesize.x + gap) + gap + position.x);
            float baseY = (gap == 0)
                ? (float)(y * (int)m_tilesize.y + position.y)
                : (float)((y - gridRange.position.y) * ((int)m_tilesize.y + gap) + gap + position.y);

            float px1 = baseX + ep.position.x;
            float py1 = baseY + ep.position.y;
            float px2 = baseX + ep.position.x + ep.size.x;
            float py2 = baseY + ep.position.y + ep.size.y;
            float tx1 = textureCoords.x * m_tilesize.x + tp.position.x;
            float ty1 = textureCoords.y * m_tilesize.y + tp.position.y;
            float tx2 = textureCoords.x * m_tilesize.x + tp.position.x + tp.size.x;
            float ty2 = textureCoords.y * m_tilesize.y + tp.position.y + tp.size.y;
            
            // Pointer to the 6 vertices for this tile
            sf::Vertex* tri = &m_vertices[index * 12];

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

            tileIndex = entity.getPreviousTextureIndex();
            if (tileIndex == Cave::Entity::NO_TEXTURE_INDEX) {
                // Pointer to the 6 vertices for this tile
                sf::Vertex* tri2 = &m_vertices[index * 12 + 6];

                // First triangle
                tri2[0].texCoords = { 0, 0 };
                tri2[1].texCoords = { 0, 0 };
                tri2[2].texCoords = { 0, 0 };

                // Second triangle
                tri2[3].texCoords = { 0, 0 };
                tri2[4].texCoords = { 0, 0 };
                tri2[5].texCoords = { 0, 0 };

                // First triangle
                tri2[0].position = { 0, 0 };
                tri2[1].position = { 0, 0 };
                tri2[2].position = { 0, 0 };

                // Second triangle
                tri2[3].position = { 0, 0 };
                tri2[4].position = { 0, 0 };
                tri2[5].position = { 0, 0 };

                index++;
                continue;
            }
            textureCoords = { tileIndex % m_tilesetCols, tileIndex / m_tilesetCols };

            ep = entity.getPreviousPosition();
            tp = entity.getPreviousTextureCoords();

            px1 = baseX + ep.position.x;
            py1 = baseY + ep.position.y;
            px2 = baseX + ep.position.x + ep.size.x;
            py2 = baseY + ep.position.y + ep.size.y;
            tx1 = textureCoords.x * m_tilesize.x + tp.position.x;
            ty1 = textureCoords.y * m_tilesize.y + tp.position.y;
            tx2 = textureCoords.x * m_tilesize.x + tp.position.x + tp.size.x;
            ty2 = textureCoords.y * m_tilesize.y + tp.position.y + tp.size.y;

            // Pointer to the 6 vertices for this tile
            sf::Vertex* tri2 = &m_vertices[index * 12 + 6];

            // First triangle
            tri2[0].texCoords = { tx1, ty1 };
            tri2[1].texCoords = { tx2, ty1 };
            tri2[2].texCoords = { tx2, ty2 };

            // Second triangle
            tri2[3].texCoords = { tx1, ty1 };
            tri2[4].texCoords = { tx2, ty2 };
            tri2[5].texCoords = { tx1, ty2 };

            // First triangle
            tri2[0].position = { px1, py1 };
            tri2[1].position = { px2, py1 };
            tri2[2].position = { px2, py2 };

            // Second triangle
            tri2[3].position = { px1, py1 };
            tri2[4].position = { px2, py2 };
            tri2[5].position = { px1, py2 };

            index++;
        }
    }
}

void Renderer::TileRenderer::updateLoadingTexture(const std::vector<bool>& loaded, const sf::Vector2i& position, const sf::IntRect& gridRange) {
    int index = 0;
    m_vertices.resize(gridRange.size.x * gridRange.size.y * 6);
    for (int y = gridRange.position.y; y < gridRange.position.y + gridRange.size.y; ++y) {
        for (int x = gridRange.position.x; x < gridRange.position.x + gridRange.size.x; ++x) {
            float px1 = x * m_tilesize.x + position.x;
            float py1 = y * m_tilesize.y + position.y;
            float px2 = x * m_tilesize.x + position.x + m_tilesize.x;
            float py2 = y * m_tilesize.y + position.y + m_tilesize.y;
            float tx1 = 0;
            float ty1 = 0;
            float tx2 = m_tilesize.x;
            float ty2 = m_tilesize.y;
            if (loaded[index]) {
                tx1 += 32;
                tx2 += 32;
            }

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

void Renderer::TileRenderer::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();
    states.texture = &m_tileset;
    target.draw(m_vertices, states);
}

void Renderer::TileRenderer::render(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();
    states.texture = &m_tileset;
    target.draw(m_vertices, states);
}