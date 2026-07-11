#pragma once

#include <SFML/Graphics.hpp>
#include "Camera/Camera.h"

namespace HUD::Developer {

    /**
     * @brief Displays the current frames per second (FPS) in developer mode.
     *
     * Renders an FPS counter overlay, updating at regular intervals
     * to provide performance feedback.
     */
    class FPSCounter : public sf::Drawable, public sf::Transformable {
    public:
        /**
         * @brief Constructs the FPS counter.
         */
        FPSCounter();

        /**
         * @brief Updates the FPS calculation and display.
         * @param camera The current camera (used for positioning, if needed).
         */
        void update(Camera camera);

    private:
        /**
         * @brief Draws the FPS counter and its background.
         */
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

        /// @brief Text object showing FPS.
        sf::Text m_fpsText;

        /// @brief Background rectangle behind FPS text.
        sf::RectangleShape m_background;

        /// @brief Clock used to measure elapsed time for updates.
        sf::Clock m_clock;

        /// @brief Timer to accumulate time between updates.
        float m_updateTimer = 0.f;

        /// @brief Number of frames counted since last update.
        int m_frameCount = 0;
    };
}
