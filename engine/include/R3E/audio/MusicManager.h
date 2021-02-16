//
// Created by jul3x on 23.02.2020.
//

#ifndef RAG3_ENGINE_INCLUDE_R3E_AUDIO_MUSICMANAGER_H
#define RAG3_ENGINE_INCLUDE_R3E_AUDIO_MUSICMANAGER_H

#include <list>
#include <unordered_map>

#include <SFML/Audio/Music.hpp>


namespace r3e::audio {

    class MusicManager {
    public:
        enum class Status {
            Playing,
            Paused,
            Stopped
        };

        MusicManager();

        void clearQueue();
        void addToQueue(sf::Music* music);
        void addToQueue(const std::string& name);
        void addDirectoryToQueue(const std::string& dir);

        void setVolume(float volume);
        void setPlaybackPitch(float pitch);

        void play();
        void pause();
        void stop();

        void update(float time_elapsed);

    private:
        std::list<sf::Music*> music_list_;
        std::list<sf::Music*>::iterator current_song_;

        std::unordered_map<std::string, std::unique_ptr<sf::Music>> music_owned_;

        Status status_;
        float current_pitch_, current_volume_;

    };
} // namespace r3e::audio

#endif //RAG3_ENGINE_INCLUDE_R3E_AUDIO_MUSICMANAGER_H
