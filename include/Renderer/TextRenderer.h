#pragma once
#include <SFML/Graphics.hpp>
#include "Game/Game.h"
#include <vector>

namespace Renderer {

    /**
     * @brief
     */
    struct NumberItem {
        sf::Vector2i position;
        int value;
        int digits;
        int highlight;
    };

    /**
     * @class TextRenderer
     * 
     * @brief Render text onto the screen.
     */
    class TextRenderer : public sf::Drawable, public sf::Transformable {
    public:
        /**
         * @brief Initialise with game instance and text buffer length.
         */
        TextRenderer(Game* game, const size_t length);

        /**
         * @brief Load the given texture, and set the tile size.
         */
        bool load(const Image::Texture& texture, const sf::Vector2u& tileSize);

        /**
         * @brief Use the NumberItem to prepare rendering number on the next render.
         */
        void updateNumbers(const std::vector<NumberItem>& items);

        /**
         * @brief At position (x, y), render the given text.
         */
        void updateText(const int& x, const int& y, const std::string& text);

        /**
         * @brief At position (x, y), render the given text, with varying y offset.
         */
        void updateMovingText(const int& x, const int& y, const std::string& text);

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

        /// @brief Buffer size, for text characters.
        size_t m_length;

        /// @brief Texture of the tile set, for the text.
        sf::Texture m_tileset;

        /// @brief The tile size, for eahc character in the tile set.
        sf::Vector2u m_tilesize = { 0, 0 };

        /// @brief The game instance.
        Game* m_game;
    };
}