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
    rotation_ = enemy->getRotation();
    life_ = enemy->getHealth();
    ammo_state_ = enemy->getWeapons().at(enemy->getCurrentWeapon())->getState();
}

void EnemyEntry::executeEntryReversal()
{
    auto new_ptr = father_->getUpdatedPtr(ptr_);
    new_ptr->setPosition(pos_);
    new_ptr->setRotation(rotation_);
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

BulletEntry::BulletEntry(Journal* father, Bullet* bullet) : JournalEntry(father), ptr_(bullet)
{
    pos_ = bullet->getPosition();
}

void BulletEntry::executeEntryReversal()
{
    auto new_ptr = father_->getUpdatedPtr(ptr_);
    new_ptr->setPosition(pos_);
}

DestroyBulletEntry::DestroyBulletEntry(Journal* father, Bullet* bullet) : JournalEntry(father), ptr_(bullet)
{
    id_ = bullet->getId();
    direction_ = bullet->getRotation();
}

void DestroyBulletEntry::executeEntryReversal()
{
    auto new_ptr = Game::get().spawnNewBullet(id_, {}, direction_);
    new_ptr->setRotation(direction_);
    father_->setUpdatedPtr(ptr_, new_ptr);
}

SpawnBulletEntry::SpawnBulletEntry(Journal* father, Bullet* bullet) : JournalEntry(father), ptr_(bullet)
{
}

void SpawnBulletEntry::executeEntryReversal()
{
    auto new_ptr = father_->getUpdatedPtr(ptr_);
    Game::get().findAndDeleteBullet(new_ptr);
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
    bullet_ptr_map_.clear();
    journal_.emplace_back();

    time_elapsed_ = 0.0f;
}

void Journal::eventEnemyDestroyed(Enemy* enemy)
{
    journal_.back().emplace_back(std::make_unique<DestroyEnemyEntry>(this, enemy));
}

void Journal::eventBulletDestroyed(Bullet* bullet)
{
    journal_.back().emplace_back(std::make_unique<DestroyBulletEntry>(this, bullet));
}

void Journal::eventBulletSpawned(Bullet* bullet)
{
    journal_.back().emplace_back(std::make_unique<SpawnBulletEntry>(this, bullet));
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
        auto& bullets = Game::get().getBullets();

        auto& journal_back = journal_.back();
        for (const auto& enemy : enemies)
        {
            journal_back.emplace_back(std::make_unique<EnemyEntry>(this, enemy.get()));
        }

        for (const auto& bullet : bullets)
        {
            journal_back.emplace_back(std::make_unique<BulletEntry>(this, bullet.get()));
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

Bullet* Journal::getUpdatedPtr(Bullet* ptr)
{
    auto it = bullet_ptr_map_.find(ptr);

    return it != bullet_ptr_map_.end() ? it->second : ptr;
}

void Journal::setUpdatedPtr(Bullet* ptr, Bullet* new_ptr)
{
    bullet_ptr_map_[ptr] = new_ptr;
}