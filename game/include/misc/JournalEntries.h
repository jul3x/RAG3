//
// Created by jul3x on 24.03.2020.
//

#ifndef RAG3_GAME_INCLUDE_MISC_JOURNALENTRIES_H
#define RAG3_GAME_INCLUDE_MISC_JOURNALENTRIES_H

#include <common/Map.h>


using namespace r3e;

class Journal;

class JournalEntry {
public:
    JournalEntry();

    explicit JournalEntry(Journal* father);

    virtual void executeEntryReversal() = 0;

protected:
    Journal* father_;

};

class TimeReversalEntry : public JournalEntry {
public:
    explicit TimeReversalEntry(Journal* father);

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

};


class DestroyCharacterEntry : public JournalEntry {
public:
    DestroyCharacterEntry(Journal* father, Character* ptr);

    void executeEntryReversal() override;

private:
    Character* ptr_;

    std::string id_;

};


class BulletEntry : public JournalEntry {
public:
    BulletEntry(Journal* father, Bullet* bullet);

    void executeEntryReversal() override;

private:
    Bullet* ptr_;

    sf::Vector2f pos_;

};

class DestroyBulletEntry : public JournalEntry {
public:
    DestroyBulletEntry(Journal* father, Bullet* bullet);

    void executeEntryReversal() override;

private:
    Bullet* ptr_;

    std::string id_;
    sf::Vector2f pos_;
    float direction_;
    Character* user_;

};

class SpawnBulletEntry : public JournalEntry {
public:
    SpawnBulletEntry(Journal* father, Bullet* bullet);

    void executeEntryReversal() override;

private:
    Bullet* ptr_;

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

class DestroyFireEntry : public JournalEntry {
public:
    DestroyFireEntry(Journal* father, Fire* fire);

    void executeEntryReversal() override;

private:
    Fire* ptr_;

    sf::Vector2f pos_;
    float direction_;
    Character* user_;

};

class SpawnFireEntry : public JournalEntry {
public:
    SpawnFireEntry(Journal* father, Fire* fire);

    void executeEntryReversal() override;

private:
    Fire* ptr_;

};

class DestroyObstacleEntry : public JournalEntry {
public:
    DestroyObstacleEntry(Journal* father, Obstacle* ptr);

    void executeEntryReversal() override;

private:
    Obstacle* ptr_;

    std::string id_;
    sf::Vector2f pos_;

};

class ShotObstacleEntry : public JournalEntry {
public:
    ShotObstacleEntry(Journal* father, Obstacle* obstacle);

    void executeEntryReversal() override;

private:
    Obstacle* ptr_;

    float life_;

};

class DestroyObstacleTileEntry : public JournalEntry {
public:
    DestroyObstacleTileEntry(Journal* father, ObstacleTile* ptr);

    void executeEntryReversal() override;

private:
    ObstacleTile* ptr_;

    std::string id_;
    sf::Vector2f pos_;

};

class ShotObstacleTileEntry : public JournalEntry {
public:
    ShotObstacleTileEntry(Journal* father, ObstacleTile* obstacle);

    void executeEntryReversal() override;

private:
    ObstacleTile* ptr_;

    float life_;

};


class SpawnDecorationEntry : public JournalEntry {
public:
    SpawnDecorationEntry(Journal* father, Decoration* ptr);

    void executeEntryReversal() override;

private:
    Decoration* ptr_;

};


#endif //RAG3_GAME_INCLUDE_MISC_JOURNALENTRIES_H
