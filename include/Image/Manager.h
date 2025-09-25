#pragma once

#include <unordered_map>
#include <SFML/Graphics.hpp>

#include "Image/Icon.h"
#include "Image/Texture.h"

namespace Image {

    /**
     * @class Manager
     * @brief Manages loading and retrieval of all images and textures used in the game.
     *
     * Textures are stored as sf::Texture objects.
     * Icons are stored as sf::Image objects.
     */
    class Manager {
    public:
        /// @brief Construct a new Image Manager instance.
        Manager();

        /// @brief Load all game images and textures.
        void loadAllImages();

        /**
         * @brief Retrieve an icon by its identifier.
         * 
         * @param icon Enum value identifying the requested icon.
         * 
         * @return const sf::Image& Reference to the stored image.
         */
        const sf::Image& getIcon(const Image::Icon& icon) const;

        /**
         * @brief Retrieve a texture by its identifier (non-owning reference).
         * 
         * @param texture Enum value identifying the requested texture.
         * 
         * @return const sf::Texture& Reference to the stored texture.
         */
        const sf::Texture& getTexture(const Image::Texture& texture) const;

    private:
        /**
         * @brief Load a single icon from disk into the manager.
         * 
         * @param icon Enum identifier for the icon.
         * @param filename Path to the image file on disk.
         * 
         * @throws std::runtime_error if the file cannot be loaded.
         */
        void loadIcon(const Image::Icon& icon, const std::string& filename);

        /**
         * @brief Load a single texture from disk into the manager.
         * 
         * @param texture Enum identifier for the texture.
         * @param filename Path to the texture file on disk.
         * 
         * @throws std::runtime_error if the file cannot be loaded.
         */
        void loadTexture(const Image::Texture& texture, const std::string& filename);

        /// @brief Map of loaded icons.
        std::unordered_map<Image::Icon, sf::Image> m_icons;

        /// @brief Map of loaded textures.
        std::unordered_map<Image::Texture, sf::Texture> m_textures;
    };
}
