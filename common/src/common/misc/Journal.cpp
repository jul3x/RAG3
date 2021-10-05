//
// Created by jul3x on 24.03.2020.
//

#include <R3E/system/Logger.h>

#include <common/misc/Journal.h>
#include <common/misc/JournalEntries.h>


Journal::Journal(Framework* framework, float max_time_back, float sampling_rate) :
        framework_(framework), time_elapsed_(0.0f), time_reversed_(0.0f)
{
    frame_time_ = 1.0f / sampling_rate;
    journal_max_size_ = static_cast<size_t>(max_time_back * sampling_rate);

    journal_.emplace_back();
}

void Journal::clear()
{
    journal_.clear();
    character_ptr_map_.clear();
    fire_ptr_map_.clear();
    bullet_ptr_map_.clear();
    decoration_ptr_map_.clear();
    special_ptr_map_.clear();
    obstacle_tile_ptr_map_.clear();
    obstacle_ptr_map_.clear();
    journal_.emplace_back();

    time_elapsed_ = 0.0f;
    time_reversed_ = 0.0f;
}

float Journal::getDurationSaved() const
{
    return journal_.size() * frame_time_;
}

void Journal::update(float time_elapsed)
{
    time_elapsed_ += time_elapsed;
    time_reversed_ = 0.0f;
    if (time_elapsed_ > frame_time_)
    {
        time_elapsed_ -= frame_time_;
        journal_.emplace_back();

        if (journal_.size() > journal_max_size_)
        {
            journal_.pop_front();
        }

        auto& enemies = framework_->getMap()->getList<NPC>();
        auto& bullets = framework_->getBullets();
        auto& fires = framework_->getFires();

        auto& journal_back = journal_.back();
        for (const auto& enemy : enemies)
        {
            journal_back.emplace_back(std::make_unique<CharacterEntry>(this, enemy.get()));
        }

        for (const auto& bullet : bullets)
        {
            journal_back.emplace_back(std::make_unique<BulletEntry>(this, bullet.get()));
        }

        for (const auto& fire : fires)
        {
            journal_back.emplace_back(std::make_unique<FireEntry>(this, fire.get()));
        }

        // player clone
        journal_back.emplace_back(std::make_unique<CharacterEntry>(this, framework_->getPlayer()));
    }
}

bool Journal::executeTimeReversal(float time_elapsed)
{
    if (journal_.size() < MIN_JOURNAL_SIZE_)
    {
        LOG.info("[Journal] Warning - could not execute time reversal!");
        return false;
    }

    time_elapsed_ += time_elapsed;

    auto frames_elapsed = static_cast<short int>(time_elapsed_ / frame_time_);
    time_elapsed_ -= static_cast<float>(frames_elapsed) * frame_time_;

    while (frames_elapsed > 0)
    {
        if (journal_.empty())
        {
            return false;
        }

        auto& frame = journal_.back();

        for (auto it = frame.rbegin(); it != frame.rend(); ++it)
        {
            (*it)->executeEntryReversal();
        }

        journal_.pop_back();

        --frames_elapsed;
        time_reversed_ += frame_time_;
    }

    return true;
}

Character* Journal::getUpdatedPtr(Character* ptr)
{
    auto it = character_ptr_map_.find(ptr);

    return it != character_ptr_map_.end() ? it->second : ptr;
}

void Journal::setUpdatedPtr(Character* ptr, Character* new_ptr)
{
    character_ptr_map_[ptr] = new_ptr;
}

Bullet* Journal::getUpdatedPtr(Bullet* ptr)
{
    auto it = bullet_ptr_map_.find(ptr);

    return it != bullet_ptr_map_.end() ? it->second : ptr;
}

void Journal::setUpdatedPtr(Bullet* ptr, Bullet* new_ptr)
{
    bullet_ptr_map_[ptr] = new_ptr;
}

Obstacle* Journal::getUpdatedPtr(Obstacle* ptr)
{
    auto it = obstacle_ptr_map_.find(ptr);

    return it != obstacle_ptr_map_.end() ? it->second : ptr;
}

void Journal::setUpdatedPtr(Obstacle* ptr, Obstacle* new_ptr)
{
    obstacle_ptr_map_[ptr] = new_ptr;
}

ObstacleTile* Journal::getUpdatedPtr(ObstacleTile* ptr)
{
    auto it = obstacle_tile_ptr_map_.find(ptr);

    return it != obstacle_tile_ptr_map_.end() ? it->second : ptr;
}

void Journal::setUpdatedPtr(ObstacleTile* ptr, ObstacleTile* new_ptr)
{
    obstacle_tile_ptr_map_[ptr] = new_ptr;
}

Fire* Journal::getUpdatedPtr(Fire* ptr)
{
    auto it = fire_ptr_map_.find(ptr);

    return it != fire_ptr_map_.end() ? it->second : ptr;
}

void Journal::setUpdatedPtr(Fire* ptr, Fire* new_ptr)
{
    fire_ptr_map_[ptr] = new_ptr;
}

Decoration* Journal::getUpdatedPtr(Decoration* ptr)
{
    auto it = decoration_ptr_map_.find(ptr);

    return it != decoration_ptr_map_.end() ? it->second : ptr;
}

void Journal::setUpdatedPtr(Decoration* ptr, Decoration* new_ptr)
{
    decoration_ptr_map_[ptr] = new_ptr;
}

Special* Journal::getUpdatedPtr(Special* ptr)
{
    auto it = special_ptr_map_.find(ptr);

    return it != special_ptr_map_.end() ? it->second : ptr;
}

void Journal::setUpdatedPtr(Special* ptr, Special* new_ptr)
{
    special_ptr_map_[ptr] = new_ptr;
}

DestructionSystem* Journal::getUpdatedPtr(DestructionSystem* ptr)
{
    auto it = destruction_ptr_map_.find(ptr);

    return it != destruction_ptr_map_.end() ? it->second : ptr;
}

void Journal::setUpdatedPtr(DestructionSystem* ptr, DestructionSystem* new_ptr)
{
    destruction_ptr_map_[ptr] = new_ptr;
}

float Journal::getTimeReversed() const
{
    return time_reversed_;
}

Framework* Journal::getFramework()
{
    return framework_;
}

void Journal::removeSavedTime(float time_to_remove)
{

}
