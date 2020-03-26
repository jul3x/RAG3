//
// Created by jul3x on 24.03.2020.
//

#include <Game.h>

#include <misc/Journal.h>
#include <misc/JournalEntries.h>




JournalEntry::JournalEntry(Journal* father) : father_(father)
{
}

EnemyEntry::EnemyEntry(Journal* father, Enemy* enemy) : JournalEntry(father), ptr_(enemy)
{
    pos_ = enemy->getPosition();
    rotation_ = enemy->getRotation();
    life_ = enemy->getHealth();
    ammo_state_ = enemy->getWeapons().at(enemy->getCurrentWeapon())->getState();

    std::cout << "ZAPISUJE NA TYLE" << ammo_state_ << "Entry no. " << this << std::endl;
}

void EnemyEntry::executeEntryReversal()
{
    auto new_ptr = father_->getUpdatedPtr(ptr_);
    new_ptr->setPosition(pos_);
    new_ptr->setRotation(rotation_);
    new_ptr->setHealth(life_);
    new_ptr->getWeapons().at(new_ptr->getCurrentWeapon())->setState(ammo_state_);
    std::cout << "USTAWIAM NA : " << new_ptr->getWeapons().at(new_ptr->getCurrentWeapon())->getState() << "Entry no. " << this << std::endl;
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

DestroyObstacleEntry::DestroyObstacleEntry(Journal* father, Obstacle* ptr) : JournalEntry(father), ptr_(ptr)
{
    id_ = ptr->getId();
    pos_ = ptr->getPosition();
}

void DestroyObstacleEntry::executeEntryReversal()
{
    auto new_ptr = Game::get().spawnNewObstacle(id_, pos_);

    father_->setUpdatedPtr(ptr_, new_ptr);
}

ShotObstacleEntry::ShotObstacleEntry(Journal* father, Obstacle* obstacle) : JournalEntry(father), ptr_(obstacle)
{
    life_ = obstacle->getHealth();
}

void ShotObstacleEntry::executeEntryReversal()
{
    auto new_ptr = father_->getUpdatedPtr(ptr_);
    new_ptr->setHealth(life_);
}


DestroyObstacleTileEntry::DestroyObstacleTileEntry(Journal* father, ObstacleTile* ptr) : JournalEntry(father), ptr_(ptr)
{
    id_ = ptr->getId();
    pos_ = ptr->getPosition();
}

void DestroyObstacleTileEntry::executeEntryReversal()
{
    auto new_ptr = Game::get().spawnNewObstacleTile(id_, pos_);

    father_->setUpdatedPtr(ptr_, new_ptr);
}

ShotObstacleTileEntry::ShotObstacleTileEntry(Journal* father, ObstacleTile* obstacle) : JournalEntry(father), ptr_(obstacle)
{
    life_ = obstacle->getHealth();
}

void ShotObstacleTileEntry::executeEntryReversal()
{
    auto new_ptr = father_->getUpdatedPtr(ptr_);
    new_ptr->setHealth(life_);
}
