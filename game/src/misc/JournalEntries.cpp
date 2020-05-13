//
// Created by jul3x on 24.03.2020.
//

#include <common/ResourceManager.h>

#include <Game.h>

#include <misc/Journal.h>
#include <misc/JournalEntries.h>


JournalEntry::JournalEntry() : father_(nullptr)
{
}

JournalEntry::JournalEntry(Journal* father) : father_(father)
{
}

TimeReversalEntry::TimeReversalEntry(Journal* father) : JournalEntry(father)
{
    auto& player = Game::get().getPlayer();
    picked_weapon_ = player.getWeapons().at(player.getCurrentWeapon())->getId();
}

void TimeReversalEntry::executeEntryReversal()
{
    auto new_ptr = Game::get().spawnNewPlayerClone(picked_weapon_);
    father_->setUpdatedPtr(&Game::get().getPlayer(), new_ptr);
}

CharacterEntry::CharacterEntry(Journal* father, Character* ptr) : JournalEntry(father), ptr_(ptr)
{
    pos_ = ptr->getPosition();
    rotation_ = ptr->getRotation();
    life_ = ptr->getHealth();
    ammo_state_ = ptr->getWeapons().at(ptr->getCurrentWeapon())->getState();
    state_ = ptr->getGlobalState();
}

void CharacterEntry::executeEntryReversal()
{
    auto new_ptr = father_->getUpdatedPtr(ptr_);
    new_ptr->setPosition(pos_);
    new_ptr->setRotation(rotation_);
    new_ptr->setHealth(life_);
    new_ptr->getWeapons().at(new_ptr->getCurrentWeapon())->setState(ammo_state_);
    new_ptr->setGlobalState(state_);
}

DestroyCharacterEntry::DestroyCharacterEntry(Journal* father, Character* ptr) : JournalEntry(father), ptr_(ptr)
{
    id_ = ptr->getId();
    activation_ = ptr->getActivation();
    funcs_ = ptr->getFunctions();
    datas_ = ptr->getDatas();
    talk_scenario_ = ptr->getTalkScenario();
}

void DestroyCharacterEntry::executeEntryReversal()
{
    auto new_ptr = Game::get().spawnNewNPC(id_, activation_, funcs_, datas_);
    father_->setUpdatedPtr(ptr_, new_ptr);

    new_ptr->setTalkScenario(talk_scenario_);
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

DestroyBulletEntry::DestroyBulletEntry(Journal* father, Bullet* bullet) :
        JournalEntry(father), ptr_(bullet), user_(bullet->getUser()), pos_(bullet->getPosition())
{
    id_ = bullet->getId();
    direction_ = bullet->getRotation() ;
}

void DestroyBulletEntry::executeEntryReversal()
{
    auto new_user = father_->getUpdatedPtr(user_);
    auto new_ptr = Game::get().spawnNewBullet(new_user, id_, pos_, direction_* M_PI / 180.0f);

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

FireEntry::FireEntry(Journal* father, Fire* fire) : JournalEntry(father), ptr_(fire)
{
    pos_ = fire->getPosition();
    alpha_ = fire->getAlpha();
    r_ = fire->getR();
}

void FireEntry::executeEntryReversal()
{
    auto new_ptr = father_->getUpdatedPtr(ptr_);
    new_ptr->setPosition(pos_);
    new_ptr->setR(r_);
    new_ptr->setAlpha(alpha_);
}

DestroyFireEntry::DestroyFireEntry(Journal* father, Fire* fire) :
        JournalEntry(father), ptr_(fire), user_(fire->getUser()), pos_(fire->getPosition())
{
    direction_ = fire->getRotation() ;
}

void DestroyFireEntry::executeEntryReversal()
{
    auto new_user = father_->getUpdatedPtr(user_);
    auto new_ptr = Game::get().spawnNewFire(new_user, pos_, direction_* M_PI / 180.0f);

    new_ptr->setRotation(direction_);
    father_->setUpdatedPtr(ptr_, new_ptr);
}

SpawnFireEntry::SpawnFireEntry(Journal* father, Fire* fire) : JournalEntry(father), ptr_(fire)
{
}

void SpawnFireEntry::executeEntryReversal()
{
    auto new_ptr = father_->getUpdatedPtr(ptr_);
    Game::get().findAndDeleteFire(new_ptr);
}

DestroyObstacleEntry::DestroyObstacleEntry(Journal* father, Obstacle* ptr) : JournalEntry(father), ptr_(ptr)
{
    id_ = ptr->getId();
    pos_ = ptr->getPosition();
    activation_ = ptr->getActivation();
    funcs_ = ptr->getFunctions();
    datas_ = ptr->getDatas();
}

void DestroyObstacleEntry::executeEntryReversal()
{
    auto new_ptr = Game::get().spawnNewObstacle(id_, pos_, activation_, funcs_, datas_);
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

SpawnDecorationEntry::SpawnDecorationEntry(Journal* father, Decoration* ptr) : JournalEntry(father), ptr_(ptr)
{
}

void SpawnDecorationEntry::executeEntryReversal()
{
    auto new_ptr = father_->getUpdatedPtr(ptr_);
    Game::get().findAndDeleteDecoration(ptr_);
}

DestroyDecorationEntry::DestroyDecorationEntry(Journal* father, Decoration* ptr) : JournalEntry(father), ptr_(ptr)
{
    id_ = ptr->getId();
    pos_ = ptr->getPosition();
}

void DestroyDecorationEntry::executeEntryReversal()
{
    auto new_ptr = Game::get().spawnNewDecoration(id_, pos_);

    father_->setUpdatedPtr(ptr_, new_ptr);
}
