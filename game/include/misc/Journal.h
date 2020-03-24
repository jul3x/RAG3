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

class Journal {
public:
    explicit Journal(float max_time_back = 1.0f, float sampling_rate = 1.0f);

    void clear();

    void eventEnemyDestroyed(Enemy* enemy);

    void update(float time_elapsed);

    bool executeTimeReversal(float time_elapsed);

    Enemy* getUpdatedPtr(Enemy* ptr);

    void setUpdatedPtr(Enemy* ptr, Enemy* new_ptr);

private:
    size_t journal_max_size_;

    std::deque<std::vector<std::unique_ptr<JournalEntry>>> journal_;

    std::unordered_map<Enemy*, Enemy*> enemy_ptr_map_;

    float time_elapsed_;
    float frame_time_;
};

#endif //RAG3_GAME_MISC_JOURNAL_H
