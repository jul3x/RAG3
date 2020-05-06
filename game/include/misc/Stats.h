//
// Created by jul3x on 08.04.20.
//

#ifndef RAG3_GAME_INCLUDE_MISC_STATS_H
#define RAG3_GAME_INCLUDE_MISC_STATS_H

#include <vector>

#include <misc/Achievements.h>


class Stats {
public:
    Stats();

    void killEnemy();

    void pickCrystal();

    void explode();

    [[nodiscard]] int getEnemiesKilled() const;

    [[nodiscard]] int getCrystalsPicked() const;

private:
    std::vector<Achievements::Type> achievements_;
    int enemies_killed_;
    int crystals_picked_;
    int explosions_;

};

#endif //RAG3_GAME_INCLUDE_MISC_STATS_H
