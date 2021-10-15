//
// Created by jul3x on 24.03.2020.
//

#ifndef RAG3_GAME_INCLUDE_MISC_JOURNALENTRIES_H
#define RAG3_GAME_INCLUDE_MISC_JOURNALENTRIES_H

#include <common/Map.h>
#include <common/weapons/Fire.h>
#include <common/misc/Destruction.h>
#include <common/misc/Journal.h>

using namespace r3e;


class TimeReversal : public JournalEntry {
public:
    explicit TimeReversal(Journal* father, void* placeholder);

    void executeEntryReversal() override;
private:
    std::string picked_weapon_;

};


class CharacterEntry : public JournalEntry {
public:
    CharacterEntry(Journal* father, Character* ptr);

    void executeEntryReversal() override;

protected:
    Character* ptr_;

    sf::Vector2f pos_;

    float rotation_;
    float life_;
    float ammo_state_;

    Character::GlobalState state_;

};


class DestroyCharacter : public JournalEntry {
public:
    DestroyCharacter(Journal* father, Character* ptr);

    void executeEntryReversal() override;

private:
    Character* ptr_;

    int u_id_;
    std::string id_;
    Functional::Activation activation_;
    j3x::List funcs_;
    j3x::List datas_;

    j3x::List talk_scenario_;

};

class BulletEntry : public JournalEntry {
public:
    BulletEntry(Journal* father, Bullet* bullet);

    void executeEntryReversal() override;

private:
    Bullet* ptr_;

    sf::Vector2f pos_;

};

class DestroyBullet : public JournalEntry {
public:
    DestroyBullet(Journal* father, Bullet* bullet);

    void executeEntryReversal() override;

private:
    Bullet* ptr_;

    std::string id_;
    sf::Vector2f pos_;
    float direction_;
    Character* user_;

};

template<class T>
class SpawnEntry : public JournalEntry {
public:
    SpawnEntry(Journal* father, T* ptr) : JournalEntry(father), ptr_(ptr)
    {
    }

    void executeEntryReversal() override
    {
        auto new_ptr = father_->getUpdatedPtr(ptr_);
        father_->getFramework()->findAndDelete<T>(new_ptr);
    }

private:
    T* ptr_;

};

class FireEntry : public JournalEntry {
public:
    FireEntry(Journal* father, Fire* fire);

    void executeEntryReversal() override;

private:
    Fire* ptr_;

    sf::Vector2f pos_;
    float r_;
    float alpha_;

};

class DestroyFire : public JournalEntry {
public:
    DestroyFire(Journal* father, Fire* fire);

    void executeEntryReversal() override;

private:
    Fire* ptr_;

    sf::Vector2f pos_;
    float direction_;
    Character* user_;

};

class DestroyObstacle : public JournalEntry {
public:
    DestroyObstacle(Journal* father, Obstacle* ptr);

    void executeEntryReversal() override;

private:
    Obstacle* ptr_;

    int u_id_;
    std::string id_;
    sf::Vector2f pos_;
    Functional::Activation activation_;
    j3x::List funcs_;
    j3x::List datas_;

};

class ShotObstacle : public JournalEntry {
public:
    ShotObstacle(Journal* father, Obstacle* obstacle);

    void executeEntryReversal() override;

private:
    Obstacle* ptr_;

    float life_;

};

class DestroyDecoration : public JournalEntry {
public:
    DestroyDecoration(Journal* father, Decoration* ptr);

    void executeEntryReversal() override;

private:
    Decoration* ptr_;

    int u_id_;
    std::string id_;
    sf::Vector2f pos_;

};

class DestroySpecial : public JournalEntry {
public:
    DestroySpecial(Journal* father, Special* ptr);

    void executeEntryReversal() override;

private:
    Special* ptr_;

    int u_id_;
    std::string id_;
    sf::Vector2f pos_;
    Functional::Activation activation_;
    j3x::List funcs_;
    j3x::List datas_;

};

class DestroyDestructionSystem : public JournalEntry {
public:
    DestroyDestructionSystem(Journal* father, DestructionSystem* ptr);

    void executeEntryReversal() override;

private:
    DestructionSystem* ptr_;

    sf::Vector2f pos_;
    float dir_;
    float quantity_factor_;
    const DestructionParams& params_;

};

class WeaponActivation : public JournalEntry {
public:
    WeaponActivation(Journal* father, PlacedWeapon* ptr);

    void executeEntryReversal() override;

private:
    PlacedWeapon* ptr_;

    bool activate_;

};

class ObjectDeactivated : public JournalEntry {
public:
    ObjectDeactivated(Journal* father, Special* ptr);

    void executeEntryReversal() override;

private:
    Special* ptr_;

};

class ChangeOpenState : public JournalEntry {
public:
    ChangeOpenState(Journal* father, Special* ptr);

    void executeEntryReversal() override;

private:
    Special* ptr_;

    bool open_state_;

};

class DoorOpen : public JournalEntry {
public:
    DoorOpen(Journal* father, Obstacle* ptr);

    void executeEntryReversal() override;

private:
    Obstacle* ptr_;

};


#endif //RAG3_GAME_INCLUDE_MISC_JOURNALENTRIES_H
