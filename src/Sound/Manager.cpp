#include "Sound/Manager.h"
#include <algorithm>
#include <iostream>

Sound::Manager::Manager() {}

void Sound::Manager::loadAllSounds() {
    // Load sound effects
    loadSound(Sound::Effect::Amoeba, ".\\assets\\sounds\\Effect\\amoeba.wav");
    loadSound(Sound::Effect::Break, ".\\assets\\sounds\\Effect\\break.wav");
    loadSound(Sound::Effect::CaveGullExplosion, ".\\assets\\sounds\\Effect\\cave_gull_explosion.wav");
    loadSound(Sound::Effect::Collect, ".\\assets\\sounds\\Effect\\collect.wav");
    loadSound(Sound::Effect::DiamondDrop, ".\\assets\\sounds\\Effect\\diamond_drop.wav");
    loadSound(Sound::Effect::DiamondLand, ".\\assets\\sounds\\Effect\\diamond_land.wav");
    loadSound(Sound::Effect::Dig, ".\\assets\\sounds\\Effect\\dig.wav");
    loadSound(Sound::Effect::Drop, ".\\assets\\sounds\\Effect\\drop.wav");
    loadSound(Sound::Effect::Explosion, ".\\assets\\sounds\\Effect\\explosion.wav");
    loadSound(Sound::Effect::Land, ".\\assets\\sounds\\Effect\\land.wav");
    loadSound(Sound::Effect::MagicWall, ".\\assets\\sounds\\Effect\\magic_wall.wav");
    loadSound(Sound::Effect::Open, ".\\assets\\sounds\\Effect\\open.wav");
    loadSound(Sound::Effect::Plasma, ".\\assets\\sounds\\Effect\\plasma.wav");
    loadSound(Sound::Effect::Tube, ".\\assets\\sounds\\Effect\\tube.wav");
    loadSound(Sound::Effect::Unlock, ".\\assets\\sounds\\Effect\\unlock.wav");
    loadSound(Sound::Effect::Yahoo, ".\\assets\\sounds\\Effect\\yahoo.wav");
    loadSound(Sound::Effect::Yippee, ".\\assets\\sounds\\Effect\\yippee.wav");

    // Load music
    loadMusic(Sound::Music::MainMenu, ".\\assets\\sounds\\Music\\main_menu.wav");
}

void Sound::Manager::loadSound(const Sound::Effect& effect, const std::string& filename) {
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile(filename)) {
        throw std::runtime_error("Error: Unable to load sound: " + filename + "\n");
    }

    m_buffers[effect] = buffer;

    auto sound = std::make_unique<sf::Sound>(buffer);
    sound->setBuffer(m_buffers[effect]);

    m_sounds[effect] = std::move(sound);
}

void Sound::Manager::loadMusic(const Sound::Music& music, const std::string& filename) {
    auto track = std::make_unique<sf::Music>();
    if (!track->openFromFile(filename)) {
        throw std::runtime_error("Error: Unable to load music: " + filename + "\n");
    }

    m_music[music] = std::move(track);
}

void Sound::Manager::play(const Sound::Effect& effect) {
    auto bufIt = m_buffers.find(effect);
    if (bufIt == m_buffers.end()) return;

    auto sound = std::make_unique<sf::Sound>(bufIt->second);
    sound->setLooping(false);
    sound->setVolume(m_volume/2);
    sound->play();

    m_activeSounds.push_back(std::move(sound));
}

void Sound::Manager::loop(const Sound::Effect& effect) {
    auto it = m_sounds.find(effect);
    if (it != m_sounds.end()) {
        it->second->setLooping(true);
        it->second->setVolume(m_volume/2);
        it->second->play();
    }
}

void Sound::Manager::stop(const Sound::Effect& effect) {
    auto it = m_sounds.find(effect);
    if (it != m_sounds.end()) {
        it->second->stop();
    }
}

bool Sound::Manager::isPlaying(const Sound::Effect& effect) const {
    auto it = m_sounds.find(effect);
    if (it != m_sounds.end()) {
        if (it->second->getStatus() == sf::Sound::Status::Playing)
            return true;
    }
    return false;
}

void Sound::Manager::playMusic(const Sound::Music& music) {
    auto it = m_music.find(music);
    if (it == m_music.end()) {
        return;
    }

    // Stop currently playing track, if any
    if (m_currentMusic && m_currentMusic->getStatus() == sf::Music::Status::Playing) {
        m_currentMusic->stop();
    }

    m_currentMusic = it->second.get();
    m_currentMusic->play();
}

void Sound::Manager::stopMusic() {
    if (m_currentMusic && m_currentMusic->getStatus() == sf::Music::Status::Playing) {
        m_currentMusic->stop();
    }
    m_currentMusic = nullptr;
}

void Sound::Manager::setVolume(const float& volume) {
    m_volume = volume;
}

void Sound::Manager::update() {
    m_activeSounds.erase(
        std::remove_if(
            m_activeSounds.begin(),
            m_activeSounds.end(),
            [](const std::unique_ptr<sf::Sound>& s) {
        return s->getStatus() == sf::Sound::Status::Stopped;
    }),
        m_activeSounds.end()
    );
}