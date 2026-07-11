#pragma once
#include <map>
#include <optional>
#include <SFML/Graphics.hpp>
#include "Cave/Entity/Entity.h"
#include "Cave/Entity/Type.h"
#include "Cave/Settings/Settings.h"
#include "Renderer/TileRenderer.h"
#include "Utils/Counter.h"
#include "Camera/Camera.h"
#include "Cave/Map/Map.h"

class Editor;

namespace HUD::Editor {

    /**
     * @class Panel
     * @brief The right-hand side panel of the editor, containing the entity picker,
     *        minimap, coordinate display, fill mode buttons, and test button.
     *
     * Panel is an SFML Drawable that renders directly into the editor's render texture.
     * It is updated once per frame via update(), which repositions all elements relative
     * to the panel camera and rebuilds the minimap vertex array from the live cave map.
     * Mouse interaction is forwarded from the Editor's event loop via handleClick() and
     * handleRelease().
     */
    class Panel : public sf::Drawable, public sf::Transformable {
    public:

        // ----------------------------------------------------------------------------------
        // Construction
        // ----------------------------------------------------------------------------------

        /// @brief Construct the panel with a pointer to the owning Editor.
        /// @param editor Pointer to the Editor instance that owns this panel.
        Panel(::Editor* editor);

        // ----------------------------------------------------------------------------------
        // Initialisation
        // ----------------------------------------------------------------------------------

        /// @brief Load all textures and initialise sprites used by the panel.
        void load();

        // ----------------------------------------------------------------------------------
        // Entity picker
        // ----------------------------------------------------------------------------------

        /**
         * @brief Select the entity at grid position (x, y) in the entity palette.
         * @param x Column index (0–2).
         * @param y Row index (0–9).
         */
        void selectType(const int& x, const int& y);

        /// @brief Return a fresh instance of the currently selected entity.
        Cave::Entity::Base getSelectedEntity();

        /// @brief Return the type of the currently selected entity.
        Cave::Entity::Type getSelectedType() const;

        /// @brief Map a palette grid position to the corresponding entity type.
        /// @param x Column (0–2). @param y Row (0–9).
        Cave::Entity::Type getType(const int& x, const int& y);

        /// @brief Construct a fresh entity instance for the given type.
        Cave::Entity::Base getNewEntity(Cave::Entity::Type type);

        // ----------------------------------------------------------------------------------
        // Update & input
        // ----------------------------------------------------------------------------------

        /**
         * @brief Update panel layout, animations, coordinate display, and minimap.
         *
         * @param panelCamera  Camera used to position all panel elements on screen.
         * @param caveCamera   Optional cave camera; used to draw the minimap viewport rect.
         * @param map          Optional live cave map; used to rebuild the minimap tiles.
         * @param animate      When false, entity animations in the palette are paused.
         */
        void update(const Camera& panelCamera, const Camera* caveCamera = nullptr, const Cave::Map* map = nullptr, bool animate = true, Cave::FillMode fillMode = Cave::FillMode::Rectangle);

        /**
         * @brief Handle a left mouse button press inside the panel area.
         *
         * Dispatches to entity selection, fill mode buttons, or the test button
         * depending on where the click landed.
         *
         * @param vp      Mouse position in virtual screen coordinates.
         * @param panelX  X offset of the panel within the virtual screen.
         * @param toolbarH Height of the toolbar strip above the panel.
         */
        void handleClick(sf::Vector2f vp, float panelX, float toolbarH);

        /// @brief Handle a left mouse button release (resets the test button state).
        void handleRelease();

        /**
         * @brief Test whether a virtual-screen position falls inside the minimap,
         *        and if so return the corresponding cave-world coordinate.
         *
         * @param vp         Mouse position in virtual screen coordinates.
         * @param panelX     X offset of the panel within the virtual screen.
         * @param toolbarH   Height of the toolbar strip above the panel.
         * @param outCavePos Receives the cave-world position if the hit test passes.
         * @return true if vp is inside the minimap bounds, false otherwise.
         */
        bool getMinimapCavePos(sf::Vector2f vp, float panelX, float toolbarH, sf::Vector2f& outCavePos) const;

        // ----------------------------------------------------------------------------------
        // Coordinate display
        // ----------------------------------------------------------------------------------

        /**
         * @brief Set the tile coordinate shown in the coordinate display.
         * @param x Tile column under the mouse cursor.
         * @param y Tile row under the mouse cursor.
         */
        void setMouseTile(int x, int y);

        // ----------------------------------------------------------------------------------
        // Getters
        // ----------------------------------------------------------------------------------

        /// @brief Return the active fill mode index (0 = single, 1 = row, 2 = rect).
        int getFillSelected() const { return m_fillSelected; }

    private:

        // ----------------------------------------------------------------------------------
        // Drawing
        // ----------------------------------------------------------------------------------

        /// @brief Draw all panel elements to the render target.
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

        // ----------------------------------------------------------------------------------
        // Minimap helpers
        // ----------------------------------------------------------------------------------

        /// @brief Rebuild the minimap vertex array from the current cave map entities.
        /// @param map The live cave map to read entity types from.
        void rebuildMinimap(const Cave::Map& map);

        /// @brief Return the minimap tile sheet index for the given entity type.
        /// @param type The entity type to look up.
        static int miniTileIndex(Cave::Entity::Type type);

        // ----------------------------------------------------------------------------------
        // Entity picker members
        // ----------------------------------------------------------------------------------

        /// @brief Tile renderer used to draw the 3×10 entity palette.
        Renderer::TileRenderer    m_tileRenderer;

        /// @brief Grey background rectangle behind the entity palette.
        sf::RectangleShape        m_background;

        /// @brief Yellow outline box drawn over the currently selected palette cell.
        sf::RectangleShape        m_selectedBox;

        /// @brief Grid position of the currently selected palette cell.
        sf::Vector2i              m_selectedPosition = { 0, 0 };

        /// @brief Live entity instances for each palette cell, used for animation.
        std::map<Cave::Entity::Type, Cave::Entity::Base> m_entities;

        /// @brief The currently selected entity type.
        Cave::Entity::Type        m_selectedType = Cave::Entity::Type::Space;

        /// @brief Tick counter that drives palette entity animation.
        Utils::TickCounter        m_TickCounter  = Utils::TickCounter();

        // ----------------------------------------------------------------------------------
        // Builder menu area
        // ----------------------------------------------------------------------------------

        /// @brief Grey background rectangle for the area below the palette.
        sf::RectangleShape        m_builderMenu;

        // ----------------------------------------------------------------------------------
        // Minimap members
        // ----------------------------------------------------------------------------------

        /// @brief Vertex array of coloured 2×2 quads representing the cave tiles.
        sf::VertexArray           m_minimapVerts;

        /// @brief Top-left position of the viewport rectangle drawn over the minimap.
        sf::Vector2f              m_minimapViewPos;

        /// @brief Size of the viewport rectangle drawn over the minimap.
        sf::Vector2f              m_minimapViewSize;

        /// @brief Dimensions of the cave currently shown in the minimap.
        int m_minimapMapW = 50;
        int m_minimapMapH = 30;

        // ----------------------------------------------------------------------------------
        // Coordinate display members
        // ----------------------------------------------------------------------------------

        /// @brief Unified white background behind the entire coordinate display.
        std::optional<sf::Sprite> m_cordsBackground;

        /// @brief "X:" label sprite (shifts left when X coordinate is 3 digits).
        std::optional<sf::Sprite> m_cordsLabelX;

        /// @brief "Y:" label sprite (always at a fixed position).
        std::optional<sf::Sprite> m_cordsLabelY;

        /// @brief Digit sprites for the X coordinate (hundreds, tens, units).
        std::optional<sf::Sprite> m_digitX0, m_digitX1, m_digitX2;

        /// @brief Digit sprites for the Y coordinate (tens, units, hundreds).
        std::optional<sf::Sprite> m_digitY0, m_digitY1, m_digitY2;

        /// @brief Tile column currently under the mouse cursor.
        int m_mouseTileX = 0;

        /// @brief Tile row currently under the mouse cursor.
        int m_mouseTileY = 0;

        // ----------------------------------------------------------------------------------
        // Button members
        // ----------------------------------------------------------------------------------

        /// @brief Sprite for the Test button (normal and pressed frames).
        std::optional<sf::Sprite> m_testBtn;

        /// @brief Sprites for the three fill mode buttons (single / row / rect).
        std::optional<sf::Sprite> m_fillBtnL, m_fillBtnC, m_fillBtnR;

        /// @brief Active fill mode: 0 = single tile, 1 = row, 2 = rectangle.
        int  m_fillSelected = 0;

        /// @brief True while the test button is held down (shows pressed frame).
        bool m_testPressed  = false;

        // ----------------------------------------------------------------------------------
        // Owner
        // ----------------------------------------------------------------------------------

        /// @brief Pointer to the owning Editor instance.
        ::Editor* m_editor;
    };
}
