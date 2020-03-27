//
// Created by jul3x on 24.03.2020.
//

#include <Game.h>

#include <misc/JournalEntries.h>
#include <misc/Journal.h>


Journal::Journal(float max_time_back, float sampling_rate) : time_elapsed_(0.0f)
{
    frame_time_ = 1.0f / sampling_rate;
    journal_max_size_ = static_cast<size_t>(max_time_back * sampling_rate);

    journal_.emplace_back();
}

void Journal::clear()
{
    journal_.clear();
    character_ptr_map_.clear();
    bullet_ptr_map_.clear();
    obstacle_tile_ptr_map_.clear();
    obstacle_ptr_map_.clear();
    journal_.emplace_back();

    time_elapsed_ = 0.0f;
}

void Journal::eventTimeReversal()
{
    journal_.back().emplace_back(std::make_unique<TimeReversalEntry>(this));
}

void Journal::eventCharacterDestroyed(Character* ptr)
{
    journal_.back().emplace_back(std::make_unique<DestroyCharacterEntry>(this, ptr));
}

void Journal::eventBulletDestroyed(Bullet* bullet)
{
    journal_.back().emplace_back(std::make_unique<DestroyBulletEntry>(this, bullet));
}

void Journal::eventBulletSpawned(Bullet* bullet)
{
    journal_.back().emplace_back(std::make_unique<SpawnBulletEntry>(this, bullet));
}

void Journal::eventObstacleDestroyed(Obstacle* ptr)
{
    journal_.back().emplace_back(
            std::make_unique<DestroyObstacleEntry>(this, ptr));
}

void Journal::eventObstacleShot(Obstacle* ptr)
{
    journal_.back().emplace_back(
            std::make_unique<ShotObstacleEntry>(this, ptr));
}

void Journal::eventObstacleTileDestroyed(ObstacleTile* ptr)
{
    journal_.back().emplace_back(
            std::make_unique<DestroyObstacleTileEntry>(this, ptr));
}

void Journal::eventObstacleTileShot(ObstacleTile* ptr)
{
    journal_.back().emplace_back(
            std::make_unique<ShotObstacleTileEntry>(this, ptr));
}

void Journal::eventDecorationSpawned(Decoration* ptr)
{
    journal_.back().emplace_back(
            std::make_unique<SpawnDecorationEntry>(this, ptr));
}

float Journal::getDurationSaved() const
{
    return journal_.size() * frame_time_;
}

void Journal::update(float time_elapsed)
{
    time_elapsed_ += time_elapsed;

    if (time_elapsed_ > frame_time_)
    {
        time_elapsed_ -= frame_time_;
        journal_.emplace_back();

        if (journal_.size() > journal_max_size_)
        {
            journal_.pop_front();
        }

        auto& enemies = Game::get().getMap().getNPCs();
        auto& bullets = Game::get().getBullets();

        auto& journal_back = journal_.back();
        for (const auto& enemy : enemies)
        {
            journal_back.emplace_back(std::make_unique<CharacterEntry>(this, enemy.get()));
        }

        for (const auto& bullet : bullets)
        {
            journal_back.emplace_back(std::make_unique<BulletEntry>(this, bullet.get()));
        }

        // player clone
        journal_back.emplace_back(std::make_unique<CharacterEntry>(this, &Game::get().getPlayer()));
    }
}

bool Journal::executeTimeReversal(float time_elapsed)
{
    if (journal_.size() < MIN_JOURNAL_SIZE_)
    {
        std::cout << "[Journal] Warning - could not execute time reversal" << std::endl;
        return false;
    }

    time_elapsed_ += time_elapsed;

    auto frames_elapsed = static_cast<short int>(time_elapsed_ / frame_time_);
    time_elapsed_ -= frames_elapsed * frame_time_;

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