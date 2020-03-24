//
// Created by jul3x on 24.03.2020.
//

#include <misc/Journal.h>
#include <Game.h>


JournalEntry::JournalEntry(Journal* father) : father_(father)
{
}

EnemyEntry::EnemyEntry(Journal* father, Enemy* enemy) : JournalEntry(father), ptr_(enemy)
{
    pos_ = enemy->getPosition();
    life_ = enemy->getHealth();
    ammo_state_ = enemy->getWeapons().at(enemy->getCurrentWeapon())->getState();
}

void EnemyEntry::executeEntryReversal()
{
    auto new_ptr = father_->getUpdatedPtr(ptr_);
    new_ptr->setPosition(pos_);
    //        new_ptr->setLife(life_);
    //        new_ptr->setAmmoState(ammo_state_);
}

DestroyEnemyEntry::DestroyEnemyEntry(Journal* father, Enemy* enemy) : JournalEntry(father), ptr_(enemy)
{
    id_ = enemy->getId();
}

void DestroyEnemyEntry::executeEntryReversal()
{
    auto new_ptr = Game::get().spawnNewEnemy(id_);
    father_->setUpdatedPtr(ptr_, new_ptr);
}

Journal::Journal(float max_time_back, float sampling_rate) : time_elapsed_(0.0f)
{
    frame_time_ = 1.0f / sampling_rate;
    journal_max_size_ = static_cast<size_t>(max_time_back * sampling_rate);
    std::cout << journal_max_size_ << std::endl;

    journal_.emplace_back();
}

void Journal::clear()
{
    journal_.clear();
    enemy_ptr_map_.clear();
    journal_.emplace_back();

    time_elapsed_ = 0.0f;
}

void Journal::eventEnemyDestroyed(Enemy* enemy)
{
    journal_.back().emplace_back(std::make_unique<DestroyEnemyEntry>(this, enemy));
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

        auto& enemies = Game::get().getMap().getCharacters();

        auto& journal_back = journal_.back();
        for (const auto& enemy : enemies)
        {
            journal_back.emplace_back(std::make_unique<EnemyEntry>(this, enemy.get()));
        }
    }
}

bool Journal::executeTimeReversal(float time_elapsed)
{
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

Enemy* Journal::getUpdatedPtr(Enemy* ptr)
{
    auto it = enemy_ptr_map_.find(ptr);

    return it != enemy_ptr_map_.end() ? it->second : ptr;
}

void Journal::setUpdatedPtr(Enemy* ptr, Enemy* new_ptr)
{
    enemy_ptr_map_[ptr] = new_ptr;
}