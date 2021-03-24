//
// Created by jul3x on 24.03.21.
//

#ifndef RAG3_COMMON_INCLUDE_MISC_ABSTRACTSTATS_H
#define RAG3_COMMON_INCLUDE_MISC_ABSTRACTSTATS_H

#include <SFML/System/Vector2.hpp>


class AbstractStats {
public:
    explicit AbstractStats();
    AbstractStats(int kills, int crystals, int explosions, int exp, int level);

    virtual void killEnemy(const sf::Vector2f& pos);
    virtual void pickCrystal(const sf::Vector2f& pos);
    virtual void explode(const sf::Vector2f& pos);
    virtual void addExp(int exp, const sf::Vector2f& pos, bool bonus_text = true);
    virtual void checkLevel();

    [[nodiscard]] int getEnemiesKilled() const;
    [[nodiscard]] int getCrystalsPicked() const;
    [[nodiscard]] int getExplosions() const;
    [[nodiscard]] int getExp() const;
    [[nodiscard]] int getLevel() const;

protected:
    int enemies_killed_;
    int crystals_picked_;
    int explosions_;

    int exp_;
    int level_;

};

#endif //RAG3_COMMON_INCLUDE_MISC_ABSTRACTSTATS_H
