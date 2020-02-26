//
// Created by jul3x on 23.02.2020.
//

#ifndef RAG3_ENGINE_AUDIO_MUSICMANAGER_H
#define RAG3_ENGINE_AUDIO_MUSICMANAGER_H

#include <list>

#include <SFML/Audio/Music.hpp>


namespace r3e {
    namespace audio {

        class MusicManager {
        public:
            enum class Status {
                Playing,
                Stopped
            };

            MusicManager();

            void addToQueue(sf::Music *music);

            void addToQueue(const std::string &name);

            void addDirectoryToQueue(const std::string &dir);

            void setVolume(float volume);

            void play();

            void update(float time_elapsed);

            void stop();

            void setPlaybackPitch(float pitch);

        private:
            std::list<sf::Music *> music_list_;
            std::list<sf::Music *>::iterator current_song_;

            std::list<sf::Music> music_owned_;

            Status status_;
            float current_pitch_, current_volume_;

        };
    } // namespace audio
} // namespace r3e

#endif //RAG3_ENGINE_AUDIO_MUSICMANAGER_H
