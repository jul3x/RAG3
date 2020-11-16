//
// Created by jul3x on 08.04.20.
//

#ifndef RAG3_GAME_INCLUDE_MISC_STATS_H
#define RAG3_GAME_INCLUDE_MISC_STATS_H

#include <SFML/System/Vector2.hpp>


class Stats {
public:
    explicit Stats();
    Stats(int kills, int crystals, int explosions, int exp, int level);

    void killEnemy(const sf::Vector2f& pos);
    void pickCrystal(const sf::Vector2f& pos);
    void explode(const sf::Vector2f& pos);
    void addExp(int exp, const sf::Vector2f& pos, bool bonus_text = true);
    void checkLevel();

    [[nodiscard]] int getEnemiesKilled() const;
    [[nodiscard]] int getCrystalsPicked() const;
    [[nodiscard]] int getExplosions() const;
    [[nodiscard]] int getExp() const;
    [[nodiscard]] int getLevel() const;

    Stats operator-(const Stats& stats) const;

private:
    int enemies_killed_;
    int crystals_picked_;
    int explosions_;

    int exp_;
    int level_;

};

#endif //RAG3_GAME_INCLUDE_MISC_STATS_H
