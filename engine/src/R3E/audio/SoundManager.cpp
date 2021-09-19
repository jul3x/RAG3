//
// Created by jul3x on 23.02.2020.
//

#include <R3E/audio/SoundManager.h>


namespace r3e {
    namespace audio {
        SoundManager::SoundManager(float attenuation) : current_pitch_(1.0f), attenuation_(attenuation)
        {

        }

        void SoundManager::playSound(const sf::SoundBuffer& buffer, const sf::Vector2f& position, float volume,
                                     bool force_pitch)
        {
            played_sounds_.emplace_back(buffer);
            if (!force_pitch)
                played_sounds_.back().setPitch(current_pitch_);
            played_sounds_.back().setPosition(position.x, 0.0f, position.y);
            played_sounds_.back().setAttenuation(attenuation_);
            played_sounds_.back().setVolume(volume);
            played_sounds_.back().play();
        }

        void SoundManager::update(float time_elapsed)
        {
            for (auto it = played_sounds_.begin(); it != played_sounds_.end();)
            {
                bool do_increment = true;
                if (it->getStatus() != sf::Sound::Status::Playing)
                {
                    auto next_it = std::next(it);

                    played_sounds_.erase(it);
                    it = next_it;
                    do_increment = false;
                }

                if (do_increment)
                    ++it;
            }
        }

        void SoundManager::changePitch(float pitch)
        {
            current_pitch_ = pitch;
        }

    } // namespace audio
} // namespace r3e