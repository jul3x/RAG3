//
// Created by jul3x on 23.02.2020.
//

#include <iostream>
#include <experimental/filesystem>

#include <R3E/audio/MusicManager.h>
#include <R3E/system/Logger.h>


namespace r3e {
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

        void MusicManager::addToQueue(const std::string& name)
        {
            if (music_owned_.find(name) != music_owned_.end())
            {
                addToQueue(music_owned_[name].get());
                return;
            }

            music_owned_[name] = std::make_unique<sf::Music>();
            if (!music_owned_[name]->openFromFile(name))
            {
                LOG.error("[MusicManager] " + name + " music file not successfully loaded.");
            }
            else
            {
                addToQueue(music_owned_[name].get());
                LOG.info("[MusicManager] Music " + name + " is loaded!");
            }
        }

        void MusicManager::addDirectoryToQueue(const std::string& dir)
        {
            std::experimental::filesystem::path path(dir);
            for (const auto& f : std::experimental::filesystem::recursive_directory_iterator(path))
            {
                if (!std::experimental::filesystem::is_directory(f))
                {
                    this->addToQueue(f.path().string());
                }
            }
        }

        void MusicManager::play()
        {
            if (status_ == Status::Paused)
            {
                if (music_list_.empty()) return;

                if (current_song_ == music_list_.begin()) current_song_ = music_list_.end();
                --current_song_;
            }
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

        void MusicManager::clearQueue()
        {
            if (!music_list_.empty())
                (*current_song_)->stop();

            music_list_.clear();
            status_ = Status::Stopped;
        }

        void MusicManager::pause()
        {
            if (music_list_.empty()) return;

            (*current_song_)->pause();
            status_ = Status::Paused;
        }

    } // namespace audio
} // namespace r3e