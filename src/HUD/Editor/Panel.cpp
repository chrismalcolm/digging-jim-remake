#include <algorithm>
#include "HUD/Editor/Panel.h"
#include "Renderer/TileRenderer.h"

HUD::Editor::Panel::Panel(Game* game): m_tileRenderer(game) {
    m_selectedBox.setSize(sf::Vector2f(32.f, 32.f));
    m_selectedBox.setFillColor(sf::Color::Transparent);
    m_selectedBox.setOutlineColor(sf::Color::Yellow);
    m_selectedBox.setOutlineThickness(2.f);

    for (int y = 0; y < 10; ++y) {
        for (int x = 0; x < 3; ++x) {  
            Cave::Entity::Type type = getType(x, y);
            Cave::Entity::Base entity = getNewEntity(type);
            m_entities[type] = entity;
        }
    }
}

void HUD::Editor::Panel::load() {
    if (!m_tileRenderer.load(Image::Texture::CaveTiles, { 32, 32 })) {
        throw std::runtime_error("Error: Unable to load editor panel.\n");
    }
}

void HUD::Editor::Panel::update(const Camera& camera)
{
    sf::Vector2f viewSize = camera.getSize();
    sf::Vector2f viewCenter = camera.getCenter();
    sf::Vector2f topLeft = viewCenter - viewSize / 2.f;
    sf::Vector2i position = { static_cast<int>(topLeft.x + viewSize.x - 96), static_cast<int>(topLeft.y) };
    m_selectedBox.setPosition({ m_selectedPosition.x * 32.f + position.x, m_selectedPosition.y * 32.f + position.y });

    std::vector<Cave::Entity::Base> entities;
    for (int y = 0; y < 10; ++y) {
        for (int x = 0; x < 3; ++x) {
            Cave::Entity::Type type = getType(x, y);
            if (m_TickCounter.onTick()) {
                m_entities[type].updateAnimation();
            }
            entities.push_back(m_entities[type]);
        }
    }

    m_tileRenderer.updateTexture(entities, position, sf::IntRect({ 0, 0 }, { 3, 10 }));

}

void HUD::Editor::Panel::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    m_tileRenderer.render(target, states);
    
    // Create a copy of the states with no shader
    sf::RenderStates noShaderStates = states;
    noShaderStates.shader = nullptr;

    // Draw selection box without shader
    target.draw(m_selectedBox, noShaderStates);
}

void HUD::Editor::Panel::selectType(const int& x, const int& y) {
    m_selectedType = getType(x, y);
    m_selectedPosition = { x, y };
}

Cave::Entity::Base HUD::Editor::Panel::getSelectedEntity() {
    return getNewEntity(m_selectedType);
}

Cave::Entity::Type HUD::Editor::Panel::getSelectedType() const {
    return m_selectedType;
}

Cave::Entity::Type HUD::Editor::Panel::getType(const int& x, const int& y) {
    int num = std::clamp(y, 0, 14) * 3 + std::clamp(x, 0, 2);
    switch (num) {

    case 0: return Cave::Entity::Type::Space;
    case 1: return Cave::Entity::Type::Dirt;
    case 2: return Cave::Entity::Type::Boulder;
    case 3: return Cave::Entity::Type::Diamond;
    case 4: return Cave::Entity::Type::FragileDiamond;
    case 5: return Cave::Entity::Type::Ore;
    case 6: return Cave::Entity::Type::Wall;
    case 7: return Cave::Entity::Type::SolidWall;
    case 8: return Cave::Entity::Type::MagicWallActive;
    case 9: return Cave::Entity::Type::HorizontalWall;
    case 10: return Cave::Entity::Type::VerticalWall;
    case 11: return Cave::Entity::Type::Plasma;
    case 12: return Cave::Entity::Type::Protozo;
    case 13: return Cave::Entity::Type::CaveGull;
    case 14: return Cave::Entity::Type::Eater;
    case 15: return Cave::Entity::Type::Aggressor;
    case 16: return Cave::Entity::Type::Cilia;
    case 17: return Cave::Entity::Type::Amoeba;
    case 18: return Cave::Entity::Type::Detonator;
    case 19: return Cave::Entity::Type::TNT;
    case 20: return Cave::Entity::Type::Bomb;
    case 21: return Cave::Entity::Type::TubeHorizontal;
    case 22: return Cave::Entity::Type::TubeVertical;
    case 23: return Cave::Entity::Type::TubeCross;
    case 24: return Cave::Entity::Type::TubeLeft;
    case 25: return Cave::Entity::Type::TubeRight;
    case 26: return Cave::Entity::Type::TubeUp;
    case 27: return Cave::Entity::Type::TubeDown;
    case 28: return Cave::Entity::Type::StartDoor;
    case 29: return Cave::Entity::Type::ExitDoor;
    default: return Cave::Entity::Type::Space;
    }
}

Cave::Entity::Base HUD::Editor::Panel::getNewEntity(Cave::Entity::Type type) {
    switch (type) {
    case Cave::Entity::Type::Space: return Cave::Entity::Space();
    case Cave::Entity::Type::Dirt: return Cave::Entity::Dirt();
    case Cave::Entity::Type::Boulder: return Cave::Entity::Boulder();
    case Cave::Entity::Type::Diamond: return Cave::Entity::Diamond();
    case Cave::Entity::Type::FragileDiamond: return Cave::Entity::FragileDiamond();
    case Cave::Entity::Type::Ore: return Cave::Entity::Ore();
    case Cave::Entity::Type::Wall: return Cave::Entity::Wall();
    case Cave::Entity::Type::SolidWall: return Cave::Entity::SolidWall();
    case Cave::Entity::Type::MagicWallActive: return Cave::Entity::MagicWallInactive();
    case Cave::Entity::Type::HorizontalWall: return Cave::Entity::HorizontalWall();
    case Cave::Entity::Type::VerticalWall: return Cave::Entity::VerticalWall();
    case Cave::Entity::Type::Plasma: return Cave::Entity::Plasma();
    case Cave::Entity::Type::Protozo: return Cave::Entity::Protozo();
    case Cave::Entity::Type::CaveGull: return Cave::Entity::CaveGull();
    case Cave::Entity::Type::Eater: return Cave::Entity::Eater();
    case Cave::Entity::Type::Aggressor: return Cave::Entity::Aggressor();
    case Cave::Entity::Type::Cilia: return Cave::Entity::Cilia();
    case Cave::Entity::Type::Amoeba: return Cave::Entity::Amoeba();
    case Cave::Entity::Type::Detonator: return Cave::Entity::Detonator();
    case Cave::Entity::Type::TNT: return Cave::Entity::TNT();
    case Cave::Entity::Type::Bomb: return Cave::Entity::Bomb();
    case Cave::Entity::Type::TubeHorizontal: return Cave::Entity::TubeHorizontal();
    case Cave::Entity::Type::TubeVertical: return Cave::Entity::TubeVertical();
    case Cave::Entity::Type::TubeCross: return Cave::Entity::TubeCross();
    case Cave::Entity::Type::TubeRight: return Cave::Entity::TubeRight();
    case Cave::Entity::Type::TubeLeft: return Cave::Entity::TubeLeft();
    case Cave::Entity::Type::TubeUp: return Cave::Entity::TubeUp();
    case Cave::Entity::Type::TubeDown: return Cave::Entity::TubeDown();
    case Cave::Entity::Type::StartDoor: return Cave::Entity::StartDoor();
    case Cave::Entity::Type::ExitDoor: return Cave::Entity::ExitDoor();
    default: return Cave::Entity::Space();
    }
}