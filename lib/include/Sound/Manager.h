#pragma once

#include <SFML/Audio.hpp>
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include "Sound/Effect.h"
#include "Sound/Music.h"

namespace Sound {

    /**
     * @class Manager
     * @brief Handles loading, playback, and management of all sound effects in the game.
     *
     * Sounds are identified using the Sound::Effect enum.
     */
    class Manager {
    public:
        /// @brief Construct a new Sound Manager instance.
        Manager();

        /// @brief Load all game sound effects into memory.
        void loadAllSounds();

        /**
         * @brief Play a sound effect once.
         *
         * Creates a temporary sound instance for one-shot playback,
         * stored in @ref m_activeSounds to ensure it remains alive until finished.
         *
         * @param effect Enum identifier of the sound effect to play.
         */
        void play(const Sound::Effect& effect);

        /**
         * @brief Play a sound effect in a continuous loop.
         * @param effect Enum identifier of the sound effect to loop.
         */
        void loop(const Sound::Effect& effect);

        /**
         * @brief Stop playback of a specific sound effect.
         * @param effect Enum identifier of the sound effect to stop.
         */
        void stop(const Sound::Effect& effect);

        /**
         * @brief Check whether a sound effect is currently playing.
         * 
         * @param effect Enum identifier of the sound effect.
         * 
         * @return true if the sound is playing, false otherwise.
         */
        bool isPlaying(const Sound::Effect& effect) const;

        /**
         * @brief Play a music track; only one track can play at a time.
         * 
         * @param music Enum identifier for the music.
         */
        void playMusic(const Sound::Music& music);

        /**
         * @brief Stop playback of the currently playing music.
         */
        void stopMusic();

        /**
         * @brief Set the volume.
         */
        void setVolume(const float& volume);

        /**
         * @brief Update the sound manager (removes finished sounds from active list).
         */
        void update();

    private:
        /**
         * @brief Load a single sound effect from file.
         * 
         * @param effect Enum identifier for the sound effect.
         * @param filename Path to the sound file.
         */
        void loadSound(const Sound::Effect& effect, const std::string& filename);

        /**
         * @brief Load a single music track from file.
         * 
         * @param music Enum identifier for the music.
         * @param filename Path to the music file.
         */
        void loadMusic(const Sound::Music& music, const std::string& filename);

        /// @brief Map of sound effect identifiers to their loaded buffers.
        std::unordered_map<Sound::Effect, sf::SoundBuffer> m_buffers;

        /// @brief Map of sound effect identifiers to their active sound objects.
        std::unordered_map<Sound::Effect, std::unique_ptr<sf::Sound>> m_sounds;

        /// @brief List of all currently active (playing) sounds.
        std::vector<std::unique_ptr<sf::Sound>> m_activeSounds;

        /// @brief Map of music identifiers to their loaded tracks.
        std::unordered_map<Sound::Music, std::unique_ptr<sf::Music>> m_music;

        /// @brief Current volume for audio
        float m_volume = 100.0;

        /// @brief Pointer to the currently playing music track, if any.
        sf::Music* m_currentMusic = nullptr;
    };

}
