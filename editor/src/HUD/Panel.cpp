#include <algorithm>
#include "HUD/Panel.h"
#include "Editor/Editor.h"

static constexpr float PANEL_W      = 104.f;
static constexpr float FILL_Y       = 460.f;
static constexpr float FILL_LX      = 10.f;
static constexpr float FILL_CX      = 40.f;
static constexpr float FILL_RX      = 70.f;
static constexpr float CORDS_BASE_X = (PANEL_W - 60.f) / 2.f;
static constexpr float CORDS_BASE_Y = 348.f;

HUD::Editor::Panel::Panel(::Editor* editor) : m_editor(editor), m_tileRenderer(&editor->imageManager) {
    m_background.setSize(sf::Vector2f(104.f, 342.f));
    m_background.setFillColor(sf::Color(128, 128, 128));

    m_selectedBox.setSize(sf::Vector2f(32.f, 32.f));
    m_selectedBox.setFillColor(sf::Color::Transparent);
    m_selectedBox.setOutlineColor(sf::Color::Yellow);
    m_selectedBox.setOutlineThickness(2.f);

    for (int y = 0; y < 10; ++y) {
        for (int x = 0; x < 3; ++x) {
            Cave::Entity::Type type = getType(x, y);
            m_entities[type] = getNewEntity(type);
        }
    }

    m_builderMenu.setSize(sf::Vector2f(PANEL_W, 496.f - 342.f));
    m_builderMenu.setFillColor(sf::Color(128, 128, 128));
    m_builderMenu.setPosition(sf::Vector2f(0.f, 342.f));

    m_minimapVerts.setPrimitiveType(sf::PrimitiveType::Triangles);
}

void HUD::Editor::Panel::load()
{
    if (!m_tileRenderer.load(Image::Texture::CaveTiles, { 32, 32 }))
        throw std::runtime_error("Error: Unable to load editor panel.\n");

    const sf::Texture& fillTex   = m_editor->imageManager.getTexture(Image::Texture::EditorFill);
    const sf::Texture& coordsTex = m_editor->imageManager.getTexture(Image::Texture::EditorCoords);
    const sf::Texture& testTex   = m_editor->imageManager.getTexture(Image::Texture::EditorTestButton);

    // 1×1 white pixel sampled from the blank gap in the label row
    m_cordsBackground.emplace(coordsTex, sf::IntRect({28, 10}, {1, 1}));
    m_cordsBackground->setPosition({CORDS_BASE_X, CORDS_BASE_Y});

    m_cordsLabelX.emplace(coordsTex, sf::IntRect({0, 10}, {30, 16}));
    m_cordsLabelX->setPosition({CORDS_BASE_X, CORDS_BASE_Y});
    m_cordsLabelY.emplace(coordsTex, sf::IntRect({30, 10}, {30, 16}));
    m_cordsLabelY->setPosition({CORDS_BASE_X + 30.f, CORDS_BASE_Y});

    auto initDigit = [&](std::optional<sf::Sprite>& s, float ox, float oy) {
        s.emplace(coordsTex, sf::IntRect({0, 0}, {6, 10}));
        s->setPosition({CORDS_BASE_X + ox, CORDS_BASE_Y + oy});
    };
    initDigit(m_digitX0, 13.f, 3.f);
    initDigit(m_digitX1, 21.f, 3.f);
    initDigit(m_digitX2, 5.f,  3.f);
    initDigit(m_digitY0, 45.f, 3.f);
    initDigit(m_digitY1, 53.f, 3.f);
    initDigit(m_digitY2, 61.f, 3.f);

    m_testBtn.emplace(testTex, sf::IntRect({0, 0}, {42, 20}));
    m_testBtn->setPosition({(PANEL_W - 42.f) / 2.f, 434.f});

    m_fillBtnL.emplace(fillTex, sf::IntRect({0,  0}, {24, 26}));  m_fillBtnL->setPosition({FILL_LX, FILL_Y});
    m_fillBtnC.emplace(fillTex, sf::IntRect({24, 0}, {24, 26}));  m_fillBtnC->setPosition({FILL_CX, FILL_Y});
    m_fillBtnR.emplace(fillTex, sf::IntRect({48, 0}, {24, 26}));  m_fillBtnR->setPosition({FILL_RX, FILL_Y});
}

int HUD::Editor::Panel::miniTileIndex(Cave::Entity::Type type)
{
    using T = Cave::Entity::Type;
    switch (type) {
    case T::Space:           return 0;
    case T::Dirt:            return 1;
    case T::Boulder:         return 2;
    case T::Diamond:         return 3;
    case T::FragileDiamond:  return 4;
    case T::Ore:             return 5;
    case T::Wall:            return 6;
    case T::SolidWall:       return 7;
    case T::MagicWallActive: return 8;
    case T::HorizontalWallPlaceholder:  return 9;
    case T::VerticalWallPlaceholder:    return 10;
    case T::Plasma:          return 11;
    case T::Protozo:         return 12;
    case T::CaveGull:        return 13;
    case T::Eater:           return 14;
    case T::Aggressor:       return 15;
    case T::Cilia:           return 16;
    case T::Amoeba:          return 17;
    case T::Detonator:       return 18;
    case T::TNT:             return 19;
    case T::Bomb:            return 20;
    case T::TubeHorizontal:  return 21;
    case T::TubeVertical:    return 22;
    case T::TubeCross:       return 23;
    case T::TubeLeft:        return 24;
    case T::TubeRight:       return 25;
    case T::TubeUp:          return 26;
    case T::TubeDown:        return 27;
    case T::StartDoor:       return 28;
    case T::ExitDoor:        return 29;
    default:                 return 0;
    }
}

void HUD::Editor::Panel::rebuildMinimap(const Cave::Map& map)
{
    static constexpr float MINI_X = (PANEL_W - 100.f) / 2.f;
    static constexpr float MINI_Y = 370.f;
    static constexpr float MINI_W = 100.f;
    static constexpr float MINI_H = 60.f;
    static constexpr int   COLS   = 3;

    m_minimapMapW = map.width;
    m_minimapMapH = map.height;

    // Scale tile size down only when cave exceeds the fixed minimap window
    const float tileW = (map.width  <= 50) ? 2.f : MINI_W / map.width;
    const float tileH = (map.height <= 30) ? 2.f : MINI_H / map.height;

    const int n = (int)map.caveEntities.size();
    m_minimapVerts.resize(n * 6);

    for (int i = 0; i < n; ++i)
    {
        int idx = miniTileIndex(map.caveEntities[i].getType());
        int tcx = idx % COLS;
        int tcy = idx / COLS;

        float px1 = MINI_X + (i % map.width) * tileW;
        float py1 = MINI_Y + (i / map.width) * tileH;
        float px2 = px1 + tileW;
        float py2 = py1 + tileH;
        float tx1 = tcx * 2.f;
        float ty1 = tcy * 2.f;
        float tx2 = tx1 + 2.f;
        float ty2 = ty1 + 2.f;

        sf::Vertex* tri = &m_minimapVerts[i * 6];
        tri[0].position = { px1, py1 }; tri[0].texCoords = { tx1, ty1 };
        tri[1].position = { px2, py1 }; tri[1].texCoords = { tx2, ty1 };
        tri[2].position = { px2, py2 }; tri[2].texCoords = { tx2, ty2 };
        tri[3].position = { px1, py1 }; tri[3].texCoords = { tx1, ty1 };
        tri[4].position = { px2, py2 }; tri[4].texCoords = { tx2, ty2 };
        tri[5].position = { px1, py2 }; tri[5].texCoords = { tx1, ty2 };
    }
}

void HUD::Editor::Panel::update(const Camera& panelCamera, const Camera* caveCamera, const Cave::Map* map, bool animate, Cave::FillMode fillMode)
{
    const Camera& camera    = panelCamera;
    sf::Vector2f viewSize   = camera.getSize();
    sf::Vector2f viewCenter = camera.getCenter();
    sf::Vector2f topLeft    = viewCenter - viewSize / 2.f;
    sf::Vector2i position   = { static_cast<int>(topLeft.x + viewSize.x - 104),
                                 static_cast<int>(topLeft.y) };

    m_background.setPosition(sf::Vector2f((float)position.x, (float)position.y));
    m_selectedBox.setPosition({
        m_selectedPosition.x * 34.f + 2.f + position.x,
        m_selectedPosition.y * 34.f + 2.f + position.y
    });

    std::vector<Cave::Entity::Base> entities;
    for (int y = 0; y < 10; ++y)
        for (int x = 0; x < 3; ++x) {
            Cave::Entity::Type type = getType(x, y);
            if (animate && m_TickCounter.onTick()) m_entities[type].updateAnimation();
            entities.push_back(m_entities[type]);
        }

    m_tileRenderer.updateTexture(entities, position, sf::IntRect({ 0, 0 }, { 3, 10 }), 2);

    auto digitRect = [](int n) { return sf::IntRect({n * 6, 0}, {6, 10}); };

    bool xIs3 = (m_mouseTileX >= 100);
    bool yIs3 = (m_mouseTileY >= 100);

    float labelX  = CORDS_BASE_X - (xIs3 ? 8.f : 0.f);
    float bgLeft  = labelX;
    float bgRight = CORDS_BASE_X + 60.f + (yIs3 ? 9.f : 0.f);
    if (m_cordsBackground) {
        m_cordsBackground->setPosition({bgLeft, CORDS_BASE_Y});
        m_cordsBackground->setScale({bgRight - bgLeft, 16.f});
    }
    if (m_cordsLabelX) m_cordsLabelX->setPosition({labelX, CORDS_BASE_Y});

    if (xIs3) {
        m_digitX2->setPosition({labelX + 13.f, CORDS_BASE_Y + 3.f});
        m_digitX2->setTextureRect(digitRect(m_mouseTileX / 100));
        m_digitX0->setPosition({labelX + 21.f, CORDS_BASE_Y + 3.f});
        m_digitX0->setTextureRect(digitRect((m_mouseTileX / 10) % 10));
        m_digitX1->setPosition({labelX + 29.f, CORDS_BASE_Y + 3.f});
        m_digitX1->setTextureRect(digitRect(m_mouseTileX % 10));
    } else {
        m_digitX2->setPosition({-100.f, -100.f});
        m_digitX0->setPosition({CORDS_BASE_X + 13.f, CORDS_BASE_Y + 3.f});
        m_digitX0->setTextureRect(digitRect(m_mouseTileX / 10));
        m_digitX1->setPosition({CORDS_BASE_X + 21.f, CORDS_BASE_Y + 3.f});
        m_digitX1->setTextureRect(digitRect(m_mouseTileX % 10));
    }

    if (yIs3) {
        m_digitY0->setPosition({CORDS_BASE_X + 45.f, CORDS_BASE_Y + 3.f});
        m_digitY0->setTextureRect(digitRect(m_mouseTileY / 100));
        m_digitY1->setPosition({CORDS_BASE_X + 53.f, CORDS_BASE_Y + 3.f});
        m_digitY1->setTextureRect(digitRect((m_mouseTileY / 10) % 10));
        m_digitY2->setPosition({CORDS_BASE_X + 61.f, CORDS_BASE_Y + 3.f});
        m_digitY2->setTextureRect(digitRect(m_mouseTileY % 10));
    } else {
        m_digitY2->setPosition({-100.f, -100.f});
        m_digitY0->setPosition({CORDS_BASE_X + 45.f, CORDS_BASE_Y + 3.f});
        m_digitY0->setTextureRect(digitRect(m_mouseTileY / 10));
        m_digitY1->setPosition({CORDS_BASE_X + 53.f, CORDS_BASE_Y + 3.f});
        m_digitY1->setTextureRect(digitRect(m_mouseTileY % 10));
    }

    m_testBtn ->setTextureRect(m_testPressed    ? sf::IntRect({42, 0}, {42, 20}) : sf::IntRect({0, 0}, {42, 20}));
    {
        Image::Texture fillTexId =
            (fillMode == Cave::FillMode::Line)    ? Image::Texture::EditorLineFill    :
            (fillMode == Cave::FillMode::Ellipse) ? Image::Texture::EditorEllipseFill :
                                                    Image::Texture::EditorFill;
        const sf::Texture& fillTex = m_editor->imageManager.getTexture(fillTexId);
        if (m_fillBtnL) m_fillBtnL->setTexture(fillTex);
        if (m_fillBtnC) m_fillBtnC->setTexture(fillTex);
        if (m_fillBtnR) m_fillBtnR->setTexture(fillTex);
    }
    m_fillBtnL->setTextureRect(m_fillSelected==0 ? sf::IntRect({0,  26},{24,26}) : sf::IntRect({0,  0},{24,26}));
    m_fillBtnC->setTextureRect(m_fillSelected==1 ? sf::IntRect({24, 26},{24,26}) : sf::IntRect({24, 0},{24,26}));
    m_fillBtnR->setTextureRect(m_fillSelected==2 ? sf::IntRect({48, 26},{24,26}) : sf::IntRect({48, 0},{24,26}));

    if (map) rebuildMinimap(*map);

    static constexpr float MINI_X = (PANEL_W - 100.f) / 2.f;
    static constexpr float MINI_Y = 370.f;
    static constexpr float MINI_W = 100.f;
    static constexpr float MINI_H = 60.f;
    if (caveCamera) {
        // World units per minimap pixel (accounts for caves wider/taller than default)
        const float wppX = (m_minimapMapW <= 50) ? 16.f : (m_minimapMapW * 32.f / MINI_W);
        const float wppY = (m_minimapMapH <= 30) ? 16.f : (m_minimapMapH * 32.f / MINI_H);
        sf::Vector2f camTopLeft = caveCamera->getCenter() - caveCamera->getSize() / 2.f;
        m_minimapViewPos = {
            MINI_X + camTopLeft.x / wppX,
            MINI_Y + camTopLeft.y / wppY
        };
        m_minimapViewSize = {
            caveCamera->getSize().x / wppX,
            caveCamera->getSize().y / wppY
        };
    }
}

void HUD::Editor::Panel::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    sf::RenderStates noShader = states;
    noShader.shader = nullptr;

    target.draw(m_background,  noShader);
    m_tileRenderer.render(target, states);
    target.draw(m_selectedBox, noShader);
    target.draw(m_builderMenu,   noShader);
    if (m_cordsBackground) target.draw(*m_cordsBackground, noShader);
    if (m_cordsLabelX) target.draw(*m_cordsLabelX, noShader);
    if (m_cordsLabelY) target.draw(*m_cordsLabelY, noShader);
    if (m_digitX2)    target.draw(*m_digitX2,    noShader);
    if (m_digitX0)    target.draw(*m_digitX0,    noShader);
    if (m_digitX1)    target.draw(*m_digitX1,    noShader);
    if (m_digitY0)    target.draw(*m_digitY0,    noShader);
    if (m_digitY1)    target.draw(*m_digitY1,    noShader);
    if (m_digitY2)    target.draw(*m_digitY2,    noShader);
    sf::RenderStates minimapStates = states;
    minimapStates.texture = &m_editor->imageManager.getTexture(Image::Texture::EditorMiniTiles);
    target.draw(m_minimapVerts, minimapStates);

    sf::RectangleShape viewRect(m_minimapViewSize);
    viewRect.setFillColor(sf::Color::Transparent);
    viewRect.setOutlineColor(sf::Color::Yellow);
    viewRect.setOutlineThickness(-2.f);
    viewRect.setPosition(m_minimapViewPos);
    target.draw(viewRect, noShader);

    if (m_testBtn)    target.draw(*m_testBtn,    noShader);
    if (m_fillBtnL)   target.draw(*m_fillBtnL,   noShader);
    if (m_fillBtnC)   target.draw(*m_fillBtnC,   noShader);
    if (m_fillBtnR)   target.draw(*m_fillBtnR,   noShader);
}

void HUD::Editor::Panel::handleClick(sf::Vector2f vp, float panelX, float toolbarH)
{
    float lx = vp.x - panelX;
    float ly = vp.y - toolbarH;

    int px = static_cast<int>(lx) / 34;
    int py = static_cast<int>(ly) / 34;
    if (px >= 0 && px < 3 && py >= 0 && py < 10)
    {
        selectType(px, py);
        return;
    }

    if      (lx >= FILL_LX && lx < FILL_LX+24.f && ly >= FILL_Y && ly < FILL_Y+26.f) m_fillSelected = 0;
    else if (lx >= FILL_CX && lx < FILL_CX+24.f && ly >= FILL_Y && ly < FILL_Y+26.f) m_fillSelected = 1;
    else if (lx >= FILL_RX && lx < FILL_RX+24.f && ly >= FILL_Y && ly < FILL_Y+26.f) m_fillSelected = 2;
    else if (lx >= (PANEL_W-42.f)/2.f && lx < (PANEL_W-42.f)/2.f+42.f &&
             ly >= 434.f && ly < 454.f)
    {
        m_testPressed = true;
        m_editor->actionTest();
    }
}

void HUD::Editor::Panel::handleRelease()
{
    m_testPressed = false;
}

bool HUD::Editor::Panel::getMinimapCavePos(sf::Vector2f vp, float panelX, float toolbarH, sf::Vector2f& outCavePos) const
{
    static constexpr float MINI_X = (PANEL_W - 100.f) / 2.f;
    static constexpr float MINI_Y = 370.f;
    static constexpr float MINI_W = 100.f;
    static constexpr float MINI_H = 60.f;

    float lx = vp.x - panelX - MINI_X;
    float ly = vp.y - toolbarH - MINI_Y;

    if (lx < 0.f || lx >= MINI_W || ly < 0.f || ly >= MINI_H)
        return false;

    const float wppX = (m_minimapMapW <= 50) ? 16.f : (m_minimapMapW * 32.f / MINI_W);
    const float wppY = (m_minimapMapH <= 30) ? 16.f : (m_minimapMapH * 32.f / MINI_H);
    outCavePos = sf::Vector2f(lx * wppX, ly * wppY);
    return true;
}

void HUD::Editor::Panel::setMouseTile(int x, int y)
{
    m_mouseTileX = x;
    m_mouseTileY = y;
}

void HUD::Editor::Panel::selectType(const int& x, const int& y) {
    m_selectedType     = getType(x, y);
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
    case 0:  return Cave::Entity::Type::Space;
    case 1:  return Cave::Entity::Type::Dirt;
    case 2:  return Cave::Entity::Type::Boulder;
    case 3:  return Cave::Entity::Type::Diamond;
    case 4:  return Cave::Entity::Type::FragileDiamond;
    case 5:  return Cave::Entity::Type::Ore;
    case 6:  return Cave::Entity::Type::Wall;
    case 7:  return Cave::Entity::Type::SolidWall;
    case 8:  return Cave::Entity::Type::MagicWallActive;
    case 9:  return Cave::Entity::Type::HorizontalWallPlaceholder;
    case 10: return Cave::Entity::Type::VerticalWallPlaceholder;
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
    case Cave::Entity::Type::Space:           return Cave::Entity::Space();
    case Cave::Entity::Type::Dirt:            return Cave::Entity::Dirt();
    case Cave::Entity::Type::Boulder:         return Cave::Entity::Boulder();
    case Cave::Entity::Type::Diamond:         return Cave::Entity::Diamond();
    case Cave::Entity::Type::FragileDiamond:  return Cave::Entity::FragileDiamond();
    case Cave::Entity::Type::Ore:             return Cave::Entity::Ore();
    case Cave::Entity::Type::Wall:            return Cave::Entity::Wall();
    case Cave::Entity::Type::SolidWall:       return Cave::Entity::SolidWall();
    case Cave::Entity::Type::MagicWallActive: return Cave::Entity::MagicWallActive();
    case Cave::Entity::Type::HorizontalWallPlaceholder: return Cave::Entity::HorizontalWallPlaceholder();
    case Cave::Entity::Type::VerticalWallPlaceholder:   return Cave::Entity::VerticalWallPlaceholder();
    case Cave::Entity::Type::Plasma:          return Cave::Entity::Plasma();
    case Cave::Entity::Type::Protozo:         return Cave::Entity::Protozo();
    case Cave::Entity::Type::CaveGull:        return Cave::Entity::CaveGull();
    case Cave::Entity::Type::Eater:           return Cave::Entity::Eater();
    case Cave::Entity::Type::Aggressor:       return Cave::Entity::Aggressor();
    case Cave::Entity::Type::Cilia:           return Cave::Entity::Cilia();
    case Cave::Entity::Type::Amoeba:          return Cave::Entity::Amoeba();
    case Cave::Entity::Type::Detonator:       return Cave::Entity::Detonator();
    case Cave::Entity::Type::TNT:             return Cave::Entity::TNT();
    case Cave::Entity::Type::Bomb:            return Cave::Entity::Bomb();
    case Cave::Entity::Type::TubeHorizontal:  return Cave::Entity::TubeHorizontal();
    case Cave::Entity::Type::TubeVertical:    return Cave::Entity::TubeVertical();
    case Cave::Entity::Type::TubeCross:       return Cave::Entity::TubeCross();
    case Cave::Entity::Type::TubeRight:       return Cave::Entity::TubeRight();
    case Cave::Entity::Type::TubeLeft:        return Cave::Entity::TubeLeft();
    case Cave::Entity::Type::TubeUp:          return Cave::Entity::TubeUp();
    case Cave::Entity::Type::TubeDown:        return Cave::Entity::TubeDown();
    case Cave::Entity::Type::StartDoor:       return Cave::Entity::StartDoor();
    case Cave::Entity::Type::ExitDoor:        return Cave::Entity::ExitDoor();
    default:                                  return Cave::Entity::Space();
    }
}
