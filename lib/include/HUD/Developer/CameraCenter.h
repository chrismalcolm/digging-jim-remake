#pragma once

#include <SFML/Graphics.hpp>
#include "Camera/Camera.h"

namespace HUD::Developer {

    /**
     * @brief Visual marker for the camera center in developer mode.
     *
     * Renders a simple shape (outline/box) to indicate the current
     * center of the camera, primarily for debugging purposes.
     */
    class CameraCenter : public sf::Drawable, public sf::Transformable {
    public:
        /**
         * @brief Constructs the camera center marker.
         */
        CameraCenter();

        /**
         * @brief Updates the marker position relative to the camera.
         * 
         * @param camera The current camera.
         * @param position The position to update the marker to.
         */
        void update(Camera camera, sf::Vector2f& position);

    private:
        /**
         * @brief Draws the camera center marker to the render target.
         */
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

        /// @brief Shape used to render the marker.
        sf::RectangleShape m_marker;
    };
}
