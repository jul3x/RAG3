//
// Created by jul3x on 08.04.20.
//

#ifndef RAG3_GAME_INCLUDE_MISC_STATS_H
#define RAG3_GAME_INCLUDE_MISC_STATS_H


class Stats {
public:
    explicit Stats();
    Stats(int kills, int crystals, int explosions);

    void killEnemy();
    void pickCrystal();
    void explode();

    [[nodiscard]] int getEnemiesKilled() const;
    [[nodiscard]] int getCrystalsPicked() const;
    [[nodiscard]] int getExplosions() const;

    Stats operator-(const Stats& stats) const;

private:
    int enemies_killed_;
    int crystals_picked_;
    int explosions_;

};

#endif //RAG3_GAME_INCLUDE_MISC_STATS_H
