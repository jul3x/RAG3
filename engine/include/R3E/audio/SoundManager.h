//
// Created by jul3x on 23.02.2020.
//

#ifndef RAG3_ENGINE_INCLUDE_R3E_AUDIO_SOUNDMANAGER_H
#define RAG3_ENGINE_INCLUDE_R3E_AUDIO_SOUNDMANAGER_H

#include <list>

#include <SFML/System/Vector2.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>


namespace r3e::audio {
    class SoundManager {
    public:
        explicit SoundManager(float attenuation);

        void
        playSound(const sf::SoundBuffer& buffer, const sf::Vector2f& position, float volume, bool force_pitch = false);

        void playSound(const sf::SoundBuffer& buffer, float volume);

        void update(float time_elapsed);

        void changePitch(float pitch);

    private:
        std::list<sf::Sound> played_sounds_;

        float current_pitch_, attenuation_;
    };
} // namespace r3e::audio

#endif //RAG3_ENGINE_INCLUDE_R3E_AUDIO_SOUNDMANAGER_H