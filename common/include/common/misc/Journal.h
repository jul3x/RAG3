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
#include <common/Framework.h>


using namespace r3e;

class JournalEntry {
public:
    JournalEntry() : father_(nullptr) {}

    explicit JournalEntry(Journal* father) : father_(father) {}

    virtual void executeEntryReversal() = 0;

protected:
    Journal* father_;

};


class Journal {
public:
    explicit Journal(Framework* framework, float max_time_back = 1.0f, float sampling_rate = 1.0f);

    void clear();

    template<class T, class K>
    void event(K* ptr = nullptr)
    {
        journal_.back().emplace_back(std::make_unique<T>(this, ptr));
    }

    void update(float time_elapsed);
    bool executeTimeReversal(float time_elapsed);
    void removeSavedTime(float time_to_remove);
    float getTimeReversed() const;
    Framework* getFramework();

    float getDurationSaved() const;
    Character* getUpdatedPtr(Character* ptr);
    Bullet* getUpdatedPtr(Bullet* ptr);
    Fire* getUpdatedPtr(Fire* ptr);
    Decoration* getUpdatedPtr(Decoration* ptr);
    Obstacle* getUpdatedPtr(Obstacle* ptr);
    ObstacleTile* getUpdatedPtr(ObstacleTile* ptr);
    Special* getUpdatedPtr(Special* ptr);
    DestructionSystem* getUpdatedPtr(DestructionSystem* ptr);

    void setUpdatedPtr(Character* ptr, Character* new_ptr);
    void setUpdatedPtr(Bullet* ptr, Bullet* new_ptr);
    void setUpdatedPtr(Obstacle* ptr, Obstacle* new_ptr);
    void setUpdatedPtr(ObstacleTile* ptr, ObstacleTile* new_ptr);
    void setUpdatedPtr(Fire* ptr, Fire* new_ptr);
    void setUpdatedPtr(Decoration* ptr, Decoration* new_ptr);
    void setUpdatedPtr(Special* ptr, Special* new_ptr);
    void setUpdatedPtr(DestructionSystem* ptr, DestructionSystem* new_ptr);

private:
    static constexpr size_t MIN_JOURNAL_SIZE_ = 10;
    size_t journal_max_size_;

    std::deque<std::vector<std::unique_ptr<JournalEntry>>> journal_;

    std::unordered_map<Character*, Character*> character_ptr_map_;
    std::unordered_map<Bullet*, Bullet*> bullet_ptr_map_;
    std::unordered_map<Fire*, Fire*> fire_ptr_map_;
    std::unordered_map<ObstacleTile*, ObstacleTile*> obstacle_tile_ptr_map_;
    std::unordered_map<Obstacle*, Obstacle*> obstacle_ptr_map_;
    std::unordered_map<Decoration*, Decoration*> decoration_ptr_map_;
    std::unordered_map<Special*, Special*> special_ptr_map_;
    std::unordered_map<DestructionSystem*, DestructionSystem*> destruction_ptr_map_;

    float time_elapsed_;
    float frame_time_;
    float time_reversed_;

    Framework* framework_;
};


#endif //RAG3_GAME_INCLUDE_MISC_JOURNAL_H
