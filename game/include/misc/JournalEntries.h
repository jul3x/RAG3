//
// Created by jul3x on 24.03.2020.
//

#ifndef RAG3_GAME_MISC_JOURNALENTRIES_H
#define RAG3_GAME_MISC_JOURNALENTRIES_H

#include <common/Map.h>


using namespace r3e;

class Journal;

class JournalEntry {
public:
    JournalEntry() = default;

    JournalEntry(Journal* father);

    virtual void executeEntryReversal() = 0;

protected:
    Journal* father_;

};


class EnemyEntry : public JournalEntry {
public:
    EnemyEntry(Journal* father, Enemy* enemy);

    void executeEntryReversal() override;

private:
    Enemy* ptr_;

    sf::Vector2f pos_;

    float rotation_;
    int life_;
    float ammo_state_;

};


class DestroyEnemyEntry : public JournalEntry {
public:
    DestroyEnemyEntry(Journal* father, Enemy* enemy);

    void executeEntryReversal() override;

private:
    Enemy* ptr_;

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
    float direction_;

};

class SpawnBulletEntry : public JournalEntry {
public:
    SpawnBulletEntry(Journal* father, Bullet* bullet);

    void executeEntryReversal() override;

private:
    Bullet* ptr_;

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


#endif //RAG3_GAME_MISC_JOURNALENTRIES_H
