//
// Created by jul3x on 24.03.2020.
//

#ifndef RAG3_GAME_INCLUDE_MISC_JOURNAL_H
#define RAG3_GAME_INCLUDE_MISC_JOURNAL_H

#include <deque>
#include <vector>
#include <memory>
#include <unordered_map>

#include <common/Map.h>

#include <misc/JournalEntries.h>

using namespace r3e;


class Journal {
public:
    explicit Journal(float max_time_back = 1.0f, float sampling_rate = 1.0f);

    void clear();

    void eventTimeReversal();
    void eventCharacterDestroyed(Character* character);
    void eventBulletDestroyed(Bullet* bullet);
    void eventBulletSpawned(Bullet* bullet);
    void eventObstacleDestroyed(Obstacle* ptr);
    void eventObstacleShot(Obstacle* ptr);
    void eventObstacleTileDestroyed(ObstacleTile* ptr);
    void eventObstacleTileShot(ObstacleTile* ptr);
    void eventDecorationSpawned(Decoration* ptr);

    void update(float time_elapsed);
    bool executeTimeReversal(float time_elapsed);

    float getDurationSaved() const;
    Character* getUpdatedPtr(Character* ptr);
    Bullet* getUpdatedPtr(Bullet* ptr);
    Obstacle* getUpdatedPtr(Obstacle* ptr);
    ObstacleTile* getUpdatedPtr(ObstacleTile* ptr);

    void setUpdatedPtr(Character* ptr, Character* new_ptr);
    void setUpdatedPtr(Bullet* ptr, Bullet* new_ptr);
    void setUpdatedPtr(Obstacle* ptr, Obstacle* new_ptr);
    void setUpdatedPtr(ObstacleTile* ptr, ObstacleTile* new_ptr);

private:
    static constexpr size_t MIN_JOURNAL_SIZE_ = 10;
    size_t journal_max_size_;

    std::deque<std::vector<std::unique_ptr<JournalEntry>>> journal_;

    std::unordered_map<Character*, Character*> character_ptr_map_;
    std::unordered_map<Bullet*, Bullet*> bullet_ptr_map_;
    std::unordered_map<ObstacleTile*, ObstacleTile*> obstacle_tile_ptr_map_;
    std::unordered_map<Obstacle*, Obstacle*> obstacle_ptr_map_;

    float time_elapsed_;
    float frame_time_;
};


#endif //RAG3_GAME_INCLUDE_MISC_JOURNAL_H
