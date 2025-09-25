#pragma once
#include <map>
#include <SFML/Graphics.hpp>
#include "Cave/Entity/Entity.h"
#include "Renderer/TileRenderer.h"
#include "Utils/Counter.h"
#include "Game/Game.h"
#include "Camera/Camera.h"

namespace HUD::Editor {

    /**
     * @brief Editor panel for selecting and placing cave entities on the map.
     */ 
    class Panel : public sf::Drawable, public sf::Transformable {
    public:
        /**
         * @brief Constructs the editor panel with a reference to the game.
         * @param game Pointer to the Game object.
         */
        Panel(Game* game);

        /**
         * @brief Load or initialize panel resources.
         */
        void load();

        /**
         * @brief Select the entity type at a given grid position.
         * @param x Grid X coordinate.
         * @param y Grid Y coordinate.
         */
        void selectType(const int& x, const int& y);

        /**
         * @brief Returns the currently selected entity instance.
         * @return The currently selected Cave::Entity::Base.
         */
        Cave::Entity::Base getSelectedEntity();

        /**
         * @brief Returns the currently selected entity type.
         * @return The currently selected Cave::Entity::Type.
         */
        Cave::Entity::Type getSelectedType() const;

        /**
         * @brief Updates the panel, e.g., for highlighting and rendering.
         * @param camera Current camera view.
         */
        void update(const Camera& camera);


    private:
        /// @brief Draws the panel and selection rectangle.
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

        /// @brief Get the entity type at a specific grid coordinate.
        Cave::Entity::Type getType(const int& x, const int& y);

        /// @brief Create a new entity instance of a given type.
        Cave::Entity::Base getNewEntity(Cave::Entity::Type type);

        /// @brief Tile renderer used to draw the entity tiles in the panel.
        Renderer::TileRenderer m_tileRenderer;

        /// @brief Rectangle highlighting the currently selected tile.
        sf::RectangleShape m_selectedBox;

        /// @brief Grid position of the currently selected tile.
        sf::Vector2i m_selectedPosition = { 0, 0 };

        /// @brief Map of entity types to prototype entity instances.
        std::map<Cave::Entity::Type, Cave::Entity::Base> m_entities;

        /// @brief Currently selected entity type.
        Cave::Entity::Type m_selectedType = Cave::Entity::Type::Space;

        /// @brief Tick counter for panel updates.
        Utils::TickCounter m_TickCounter = Utils::TickCounter();
    };
}
