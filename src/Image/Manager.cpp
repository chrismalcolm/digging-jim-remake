#include "Image/Manager.h"
#include <iostream>

Image::Manager::Manager() {};

void Image::Manager::loadAllImages() {
    // Load icons
    loadIcon(Image::Icon::DiggingJimMain, ".\\assets\\icons\\DiggingJim\\main.png");
    
    // Load textures
    loadTexture(Image::Texture::CaveHUD, ".\\assets\\textures\\Cave\\hud.png");
    loadTexture(Image::Texture::CaveLoadingTiles, ".\\assets\\textures\\Cave\\loading_tiles.png");
    loadTexture(Image::Texture::CaveNumbers, ".\\assets\\textures\\Cave\\numbers.png");
    loadTexture(Image::Texture::CaveTiles, ".\\assets\\textures\\Cave\\tiles.png");
    loadTexture(Image::Texture::GameCompleted, ".\\assets\\textures\\Game\\completed.png");
    loadTexture(Image::Texture::GameFont, ".\\assets\\textures\\Game\\font.png");
    loadTexture(Image::Texture::GameOver, ".\\assets\\textures\\Game\\over.png");
    loadTexture(Image::Texture::MainMenuBelow, ".\\assets\\textures\\MainMenu\\below.png");
    loadTexture(Image::Texture::MainMenuDial, ".\\assets\\textures\\MainMenu\\dial.png");
    loadTexture(Image::Texture::MainMenuLoadCaves, ".\\assets\\textures\\MainMenu\\load_caves.png");
    loadTexture(Image::Texture::MainMenuNumbers, ".\\assets\\textures\\MainMenu\\numbers.png");
    loadTexture(Image::Texture::MainMenuOnOff, ".\\assets\\textures\\MainMenu\\on_off.png");
    loadTexture(Image::Texture::MainMenuOptions, ".\\assets\\textures\\MainMenu\\options.png");
    loadTexture(Image::Texture::MainMenuSelectArrow, ".\\assets\\textures\\MainMenu\\select_arrow.png");
    loadTexture(Image::Texture::MainMenuTop, ".\\assets\\textures\\MainMenu\\top.png");
}

void Image::Manager::loadIcon(const Image::Icon& icon, const std::string& filename) {
    sf::Image image;
    if (!image.loadFromFile(filename)) {
        throw std::runtime_error("Error: Unable to load icon: " + filename + "\n");
    }
    m_icons[icon] = std::move(image);
}

void Image::Manager::loadTexture(const Image::Texture& texture, const std::string& filename) {
    sf::Texture tex;
    if (!tex.loadFromFile(filename)) {
        throw std::runtime_error("Error: Unable to load texture: " + filename + "\n");
    }
    m_textures[texture] = std::move(tex);
}

const sf::Image& Image::Manager::getIcon(const Image::Icon& icon) const {
    return m_icons.at(icon);
}

const sf::Texture& Image::Manager::getTexture(const Image::Texture& texture) const {
    return m_textures.at(texture);
}