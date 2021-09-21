//
// Created by jul3x on 08.04.20.
//

#ifndef RAG3_GAME_INCLUDE_MISC_ACHIEVEMENTS_H
#define RAG3_GAME_INCLUDE_MISC_ACHIEVEMENTS_H

#include <unordered_set>
#include <unordered_map>
#include <string>

#include <common/ResourceManager.h>

#include <misc/Stats.h>


using namespace r3e;

class Achievements {
public:
    explicit Achievements(Stats* stats);
    void load(const std::string& path);
    void rotate();
    void update(float time_elapsed);
    void setAchievementsUnlocked(const j3x::List& achievements);

private:
    void check(const std::string& condition, int data, int delta_data);

    Stats* stats_;
    Stats saved_stats_;

    float time_elapsed_{};

    std::unordered_map<std::string, std::vector<std::pair<std::string, j3x::Parameters>>> achievements_by_condition_;
    std::unordered_set<std::string> achievements_unlocked_;

};

#endif //RAG3_GAME_INCLUDE_MISC_ACHIEVEMENTS_H
