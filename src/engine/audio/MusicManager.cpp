//
// Created by jul3x on 23.02.2020.
//

#include <iostream>

#include <engine/audio/MusicManager.h>


namespace audio {

    MusicManager::MusicManager() : current_pitch_(1.0f), current_volume_(100.0f),
                                   current_song_(music_list_.begin()), status_(Status::Stopped)
    {

    }

    void MusicManager::addToQueue(sf::Music* music)
    {
        music_list_.push_back(music);

        if (music_list_.size() == 1) current_song_ = music_list_.begin();
    }

    void MusicManager::play()
    {
        status_ = Status::Playing;
    }

    void MusicManager::update(float time_elapsed)
    {
        if (music_list_.empty()) return;

        if (status_ == Status::Playing)
        {
            if ((*current_song_)->getStatus() != sf::Music::Status::Playing)
            {
                (*current_song_)->stop();
                ++current_song_;

                if (current_song_ == music_list_.end())
                {
                    current_song_ = music_list_.begin();
                }

                (*current_song_)->setPitch(current_pitch_);
                (*current_song_)->setVolume(current_volume_);
                (*current_song_)->play();
            }
        }
    }

    void MusicManager::stop()
    {
        status_ = Status::Stopped;

        if (music_list_.empty()) return;

        (*current_song_)->stop();
    }

    void MusicManager::setPlaybackPitch(float pitch)
    {
        current_pitch_ = pitch;

        if (music_list_.empty()) return;

        (*current_song_)->setPitch(current_pitch_);
    }

    void MusicManager::setVolume(float volume)
    {
        current_volume_ = volume;

        if (music_list_.empty()) return;

        (*current_song_)->setVolume(current_volume_);
    }

} // namespace audio