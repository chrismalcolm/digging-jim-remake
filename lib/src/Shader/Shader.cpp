#include <algorithm>
#include <iostream>
#include <string>
#include "Shader/Shader.h"

Shader::Shader::Shader(Game* game) : m_game(game) {}

void Shader::Shader::loadAllShaders() {
    const std::string& shaderFilename = "./assets/shaders/tint.frag";
    if (!m_shader.loadFromFile(shaderFilename, sf::Shader::Type::Fragment)) {
        throw std::runtime_error("Error: Unable to load shader .frag file: " + shaderFilename + "\n");
    }

    const std::string& defaultFilename = "./assets/shaders/tint.frag";
    if (!m_default.loadFromFile(defaultFilename, sf::Shader::Type::Fragment)) {
        throw std::runtime_error("Error: Unable to load shader .frag file: " + defaultFilename + "\n");
    }

    // Initialize default shader with a neutral tint
    m_default.setUniform("tint", sf::Glsl::Vec3{ 0.5f, 0.5f, 0.5f });
}

void Shader::Shader::update() {
    m_shader.setUniform("tint", sf::Glsl::Vec3{
        static_cast<float>(std::clamp(m_game->getHue(), 0, 200)) / 200.f,
        static_cast<float>(std::clamp(m_game->getSat(), 0, 200)) / 200.f,
        static_cast<float>(std::clamp(m_game->getLum(), 0, 200)) / 200.f,
        });
}

sf::Shader* Shader::Shader::currentShader() {
    if (m_game->getGameOptions().fixedColours) return &m_default;
    return &m_shader;
}

sf::Shader* Shader::Shader::defaultShader() {
    return &m_default;
}
