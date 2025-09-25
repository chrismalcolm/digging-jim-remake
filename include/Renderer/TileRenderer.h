#pragma once
#include "Game/Game.h"
#include <SFML/Graphics.hpp>
#include "Cave/Entity/Base.h"
#include "Image/Texture.h"

namespace Renderer {
    class TileRenderer : public sf::Drawable, public sf::Transformable {
    public:   
        /**
         * @brief Initialise with game instance.
         */
        TileRenderer(Game* game);
        
        /**
         * @brief Load the given texture, and set the tile size.
         */
        bool load(const Image::Texture& texture, const sf::Vector2u& tileSize);
        
        /**
         * @brief Prepare the texture for drawing by updating the positions and textures for each tile drawn.
         */
        void updateTexture(const std::vector<Cave::Entity::Base>& entities, const sf::Vector2i& position, const sf::IntRect& gridRange);
        
        /**
         * @brief Prepare the locations of the loading tiles to be drawn,
         */
        void updateLoadingTexture(const std::vector<bool>& entities, const sf::Vector2i& position, const sf::IntRect& gridRange);
        
        /**
         * @brief Render the text onto the screen.
         */
        void render(sf::RenderTarget& target, sf::RenderStates states) const;

    private:
        /**
         * @brief Override to draw text to screen.
         */
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

        /// @brief Vertex array for draing text.
        sf::VertexArray m_vertices;

        /// @brief Texture of the tile set, for the text.
        sf::Texture m_tileset;

        /// @brief The tile size, for eahc character in the tile set.
        sf::Vector2u m_tilesize = { 0, 0 };

        /// @brief The number of colums for the tileset.
        int m_tilesetCols = 0;

        /// @brief The game instance.
        Game* m_game;
    };
}