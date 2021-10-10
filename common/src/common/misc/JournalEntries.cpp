//
// Created by jul3x on 24.03.2020.
//

#include <common/ResourceManager.h>

#include <common/misc/Journal.h>
#include <common/misc/JournalEntries.h>


JournalEntry::JournalEntry() : father_(nullptr)
{
}

JournalEntry::JournalEntry(Journal* father) : father_(father)
{
}

TimeReversal::TimeReversal(Journal* father, void* placeholder) : JournalEntry(father)
{
    auto player = father_->getFramework()->getPlayer();
    picked_weapon_ = player->getWeapons().at(player->getCurrentWeapon())->getId();
}

void TimeReversal::executeEntryReversal()
{
    auto new_ptr = father_->getFramework()->spawnNewPlayerClone(picked_weapon_);
    father_->setUpdatedPtr(father_->getFramework()->getPlayer(), new_ptr);
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

SpawnCharacter::SpawnCharacter(Journal* father, Character* character) : JournalEntry(father), ptr_(character)
{
}

void SpawnCharacter::executeEntryReversal()
{
    auto new_ptr = father_->getUpdatedPtr(ptr_);
    father_->getFramework()->findAndDeleteCharacter(new_ptr);
}

DestroyCharacter::DestroyCharacter(Journal* father, Character* ptr) : JournalEntry(father), ptr_(ptr)
{
    u_id_ = ptr->getUniqueId();
    id_ = ptr->getId();
    activation_ = ptr->getActivation();
    funcs_ = ptr->getFunctions();
    datas_ = ptr->getDatas();
    talk_scenario_ = ptr->getTalkScenario();
}

void DestroyCharacter::executeEntryReversal()
{
    auto new_ptr = father_->getFramework()->spawnNewNPC(id_, u_id_, activation_, funcs_, datas_);
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

DestroyBullet::DestroyBullet(Journal* father, Bullet* bullet) :
        JournalEntry(father), ptr_(bullet), user_(bullet->getUser()), pos_(bullet->getPosition())
{
    id_ = bullet->getId();
    direction_ = bullet->getRotation();
}

void DestroyBullet::executeEntryReversal()
{
    auto new_user = father_->getUpdatedPtr(user_);
    auto new_ptr = father_->getFramework()->spawnNewBullet(new_user, id_, pos_, direction_ * M_PI / 180.0f);

    new_ptr->setRotation(direction_);
    father_->setUpdatedPtr(ptr_, new_ptr);
}

SpawnBullet::SpawnBullet(Journal* father, Bullet* bullet) : JournalEntry(father), ptr_(bullet)
{
}

void SpawnBullet::executeEntryReversal()
{
    auto new_ptr = father_->getUpdatedPtr(ptr_);
    father_->getFramework()->findAndDeleteBullet(new_ptr);
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

DestroyFire::DestroyFire(Journal* father, Fire* fire) :
        JournalEntry(father), ptr_(fire), user_(fire->getUser()), pos_(fire->getPosition())
{
    direction_ = fire->getRotation();
}

void DestroyFire::executeEntryReversal()
{
    auto new_user = father_->getUpdatedPtr(user_);
    auto new_ptr = father_->getFramework()->spawnNewFire(new_user, pos_, direction_ * M_PI / 180.0f);

    new_ptr->setRotation(direction_);
    father_->setUpdatedPtr(ptr_, new_ptr);
}

SpawnFire::SpawnFire(Journal* father, Fire* fire) : JournalEntry(father), ptr_(fire)
{
}

void SpawnFire::executeEntryReversal()
{
    auto new_ptr = father_->getUpdatedPtr(ptr_);
    father_->getFramework()->findAndDeleteFire(new_ptr);
}

DestroyObstacle::DestroyObstacle(Journal* father, Obstacle* ptr) : JournalEntry(father), ptr_(ptr)
{
    id_ = ptr->getId();
    u_id_ = ptr->getUniqueId();
    pos_ = ptr->getPosition();
    activation_ = ptr->getActivation();
    funcs_ = ptr->getFunctions();
    datas_ = ptr->getDatas();
}

void DestroyObstacle::executeEntryReversal()
{
    auto new_ptr = father_->getFramework()->spawnNewObstacle(id_, u_id_, pos_, activation_, funcs_, datas_);
    father_->setUpdatedPtr(ptr_, new_ptr);
}

ShotObstacle::ShotObstacle(Journal* father, Obstacle* obstacle) : JournalEntry(father), ptr_(obstacle)
{
    life_ = obstacle->getHealth();
}

void ShotObstacle::executeEntryReversal()
{
    auto new_ptr = father_->getUpdatedPtr(ptr_);
    new_ptr->setHealth(life_);
}

SpawnDecoration::SpawnDecoration(Journal* father, Decoration* ptr) : JournalEntry(father), ptr_(ptr)
{
}

void SpawnDecoration::executeEntryReversal()
{
    auto new_ptr = father_->getUpdatedPtr(ptr_);
    father_->getFramework()->findAndDeleteDecoration(new_ptr);
}

DestroyDecoration::DestroyDecoration(Journal* father, Decoration* ptr) : JournalEntry(father), ptr_(ptr)
{
    u_id_ = ptr->getUniqueId();
    id_ = ptr->getId();
    pos_ = ptr->getPosition();
}

void DestroyDecoration::executeEntryReversal()
{
    auto new_ptr = father_->getFramework()->spawnNewDecoration(id_, u_id_, pos_);
    father_->setUpdatedPtr(ptr_, new_ptr);
}

SpawnSpecial::SpawnSpecial(Journal* father, Special* ptr) : JournalEntry(father), ptr_(ptr)
{

}

void SpawnSpecial::executeEntryReversal()
{
    auto new_ptr = father_->getUpdatedPtr(ptr_);
    father_->getFramework()->findAndDeleteSpecial(new_ptr);
}

DestroySpecial::DestroySpecial(Journal* father, Special* ptr) : JournalEntry(father), ptr_(ptr)
{
    u_id_ = ptr->getUniqueId();
    id_ = ptr->getId();
    pos_ = ptr->getPosition();
    activation_ = ptr->getActivation();
    funcs_ = ptr->getFunctions();
    datas_ = ptr->getDatas();
}

void DestroySpecial::executeEntryReversal()
{
    auto new_ptr = father_->getFramework()->spawnNewSpecial(id_, u_id_, pos_, activation_, funcs_, datas_);
    father_->setUpdatedPtr(ptr_, new_ptr);
}

SpawnDestructionSystem::SpawnDestructionSystem(Journal* father, DestructionSystem* ptr) :
        JournalEntry(father), ptr_(ptr)
{

}

void SpawnDestructionSystem::executeEntryReversal()
{
    auto new_ptr = father_->getUpdatedPtr(ptr_);
    father_->getFramework()->findAndDeleteDestructionSystem(new_ptr);
}

DestroyDestructionSystem::DestroyDestructionSystem(Journal* father, DestructionSystem* ptr) :
        JournalEntry(father), ptr_(ptr), params_(ptr->getDestructionParams()), dir_(ptr->getDestructionDir()),
        pos_(ptr->getPosition()), quantity_factor_(ptr->getQuantityFactor())
{
}

void DestroyDestructionSystem::executeEntryReversal()
{
    auto new_ptr = father_->getFramework()->spawnNewDestructionSystem(pos_, dir_, params_, quantity_factor_);

    // simulate blood
    for (int i = 0; i < 30; ++i)
    {
        new_ptr->update(1.0f / 30.0f);
    }

    father_->setUpdatedPtr(ptr_, new_ptr);
}

WeaponActivation::WeaponActivation(Journal* father, PlacedWeapon* ptr) : JournalEntry(father), ptr_(ptr)
{
    activate_ = ptr->getActive();
}

void WeaponActivation::executeEntryReversal()
{
    ptr_->setActive(!activate_);
}

ChangeOpenState::ChangeOpenState(Journal* father, Special* ptr) : JournalEntry(father), ptr_(ptr)
{
    open_state_ = ptr->getAdditionalBooleanData();
}

void ChangeOpenState::executeEntryReversal()
{
    auto new_ptr = father_->getUpdatedPtr(ptr_);
    father_->getFramework()->getSpecialFunctions()->changeOpenState(new_ptr, "", nullptr);
}

DoorOpen::DoorOpen(Journal* father, Obstacle* ptr) : JournalEntry(father), ptr_(ptr)
{
}

void DoorOpen::executeEntryReversal()
{
    auto new_ptr = father_->getUpdatedPtr(ptr_);
    father_->getFramework()->getSpecialFunctions()->openDoor(nullptr, new_ptr->getUniqueId(), nullptr);
}

ObjectDeactivated::ObjectDeactivated(Journal* father, Special* ptr) : JournalEntry(father), ptr_(ptr)
{
}

void ObjectDeactivated::executeEntryReversal()
{
    auto new_ptr = father_->getUpdatedPtr(ptr_);
    new_ptr->activate();
}
