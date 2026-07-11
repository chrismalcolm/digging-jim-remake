#pragma once

#include "Game/Game.h"
#include <SFML/Graphics.hpp>

namespace Shader {

    /**
     * @class Shader
     * @brief Manages game shaders, including current and default tint shaders.
     *
     * Handles loading shader files and can update hue, saturation and luminance based on game state.
     */
    class Shader {
    public:
        /**
         * @brief Construct a new Shader manager.
         * 
         * @param game Pointer to the current Game instance (used for uniform updates).
         */
        Shader(Game* game);

        /**
         * @brief Load all shader files from disk.
         *
         * Loads the main fragment shader and a default shader.
         * The default shader is initialized with a neutral tint.
         *
         * @throws std::runtime_error If the main or default shader file cannot be loaded.
         */
        void loadAllShaders();

        /**
         * @brief Update the active shader based on the current game state.
         *
         * Sets the shader's "tint" uniform according to the game's hue, saturation, and luminance values.
         * Values are clamped to [0,200] and normalized to [0.0, 1.0] for GLSL.
         */
        void update();

        /**
         * @brief Get the currently active shader.
         * 
         * @return Pointer to the active sf::Shader.
         */
        sf::Shader* currentShader();

        /**
         * @brief Get the default shader.
         * 
         * @return Pointer to the default sf::Shader.
         */
        sf::Shader* defaultShader();

    private:

        /// @brief Pointer to the game instance for uniform updates.
        Game* m_game;

        /// @brief Active shader applied to the game view.
        sf::Shader m_shader;

        /// @brief Default shader (backup or base shader).
        sf::Shader m_default;
    };

}
