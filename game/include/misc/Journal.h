//
// Created by jul3x on 24.03.2020.
//

#ifndef RAG3_GAME_MISC_JOURNAL_H
#define RAG3_GAME_MISC_JOURNAL_H

#include <deque>
#include <vector>
#include <memory>
#include <unordered_map>

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
    float life_;
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

class Journal {
public:
    explicit Journal(float max_time_back = 1.0f, float sampling_rate = 1.0f);

    void clear();

    void eventEnemyDestroyed(Enemy* enemy);

    void eventBulletDestroyed(Bullet* bullet);

    void eventBulletSpawned(Bullet* bullet);

    void update(float time_elapsed);

    bool executeTimeReversal(float time_elapsed);

    Enemy* getUpdatedPtr(Enemy* ptr);

    void setUpdatedPtr(Enemy* ptr, Enemy* new_ptr);

    Bullet* getUpdatedPtr(Bullet* ptr);

    void setUpdatedPtr(Bullet* ptr, Bullet* new_ptr);

private:
    size_t journal_max_size_;

    std::deque<std::vector<std::unique_ptr<JournalEntry>>> journal_;

    std::unordered_map<Enemy*, Enemy*> enemy_ptr_map_;

    std::unordered_map<Bullet*, Bullet*> bullet_ptr_map_;

    float time_elapsed_;
    float frame_time_;
};

#endif //RAG3_GAME_MISC_JOURNAL_H
