#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include "Camera/Camera.h"

namespace HUD::Developer {

    /**
     * @brief Displays a label and position coordinates for an object in developer mode.
     *
     * Can be used to track entities, the player, or other points of interest
     * on the HUD relative to the camera.
     */
    class PositionDisplay : public sf::Drawable, public sf::Transformable {
    public:
        /**
         * @brief Constructs the position display.
         */
        PositionDisplay();

        /**
         * @brief Updates the display with a name and position.
         *
         * @param camera Current camera view (used for HUD placement)
         * @param name Label/name for the object being tracked
         * @param position Position to display (x, y)
         * @param yOffset Vertical offset for display positioning
         */
        void update(const Camera& camera, const std::string& name, const sf::Vector2f& position, const float& yOffset);

    private:
        /**
         * @brief Draws the position display and background rectangle.
         */
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

        /// @brief Text showing the object's name and coordinates.
        sf::Text m_positionText;

        /// @brief Background rectangle behind the text.
        sf::RectangleShape m_background;
    };
}
