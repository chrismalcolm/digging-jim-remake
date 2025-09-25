#pragma once
#include <SFML/Graphics.hpp>
#include "Game/Game.h"
#include "Cave/Properties/Properties.h"
#include "Renderer/TextRenderer.h"
#include "Utils/Counter.h"
#include "Camera/Camera.h"

namespace HUD::Level {

    /// @brief HUD panel for displaying game information such as diamonds, lives, score, and cave number.
    class Panel : public sf::Drawable, public sf::Transformable {
    public:
        /**
         * @brief Constructs the HUD panel with a reference to the Game.
         * 
         * @param game Pointer to the Game instance.
         */
        Panel(Game* game);

        /**
         * @brief Load textures, fonts, and initialize internal resources.
         */
        void load();

        /**
         * @brief Reveals the panel (slides it into view).
         */
        void reveal();

        /**
         * @brief Hides the panel (slides it out of view).
         */
        void hide();

        /**
         * @brief Immediately hides the panel and resets its height.
         */
        void disappear();

        /**
         * @brief Updates the panel contents, highlighting, and animation.
         * 
         * @param camera Current camera view used for HUD placement.
         */
        void update(const Camera& camera);

    private:
        /**
         * @brief Draws the panel, including the HUD texture and text renderers.
         * 
         * @param target Render target to draw to.
         * @param states Current render states.
         */
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

        /// @brief Pointer to the main Game instance.
        Game* m_game;

        /// @brief Renderer for numeric HUD values (diamonds, score, etc.).
        Renderer::TextRenderer m_textRenderer;

        /// @brief Renderer for the "Paused" text.
        Renderer::TextRenderer m_pauseTextRenderer;

        /// @brief Tick counter used for animation highlights.
        Utils::TickCounter m_TickCounter = Utils::TickCounter();

        /// @brief HUD background texture.
        sf::Texture m_caveHUD;

        /// @brief Font texture for HUD text.
        sf::Texture m_caveFont;

        /// @brief Vertex array used to draw the HUD background.
        sf::VertexArray m_vertices;

        /// @brief Highlight for diamond changes (frames remaining).
        int m_diamondHighlight = 0;

        /// @brief Previous collected diamond count for comparison.
        int m_previousCollected = 0;

        /// @brief Highlight for lives changes (frames remaining).
        int m_livesHighlight = 0;

        /// @brief Previous number of lives for comparison.
        int m_previousLives = 0;

        /// @brief Previous points/score for comparison.
        int m_previousPoints = 0;

        /// @brief Highlight for points changes (frames remaining).
        int m_pointsHighlight = 0;

        /// @brief Highlight for time warning (frames remaining).
        int m_timeHighlight = 0;

        /// @brief Direction of time highlight increment (for pulsing effect).
        int m_timeHighlightIncrease = 0;

        /// @brief Current player score.
        int m_score = 0;

        /// @brief Current height of the panel (for slide animation).
        int m_panelHeight;

        /// @brief Whether the panel is currently revealed.
        bool m_panelRevealed;

        /// @brief Cave number currently being displayed.
        int m_displayCaveNumber;
    };
}
