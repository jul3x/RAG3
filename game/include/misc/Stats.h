//
// Created by jul3x on 08.04.20.
//

#ifndef RAG3_GAME_MISC_STATS_H
#define RAG3_GAME_MISC_STATS_H

#include <vector>

#include <misc/Achievements.h>


class Stats {
public:
    Stats();

    void killEnemy();

    void pickCrystal();

    int getEnemiesKilled() const;

    int getCrystalsPicked() const;

private:
    std::vector<Achievements::Type> achievements_;
    int enemies_killed_;
    int crystals_picked_;

};

#endif //RAG3_GAME_MISC_STATS_H
