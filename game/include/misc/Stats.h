//
// Created by jul3x on 08.04.20.
//

#ifndef RAG3_GAME_INCLUDE_MISC_STATS_H
#define RAG3_GAME_INCLUDE_MISC_STATS_H

#include <SFML/System/Vector2.hpp>

#include <common/misc/AbstractStats.h>


class Game;

class Stats : public AbstractStats {
public:
    explicit Stats();
    explicit Stats(Game* game);
    Stats(int kills, int crystals, int explosions, int exp, int level);

    void killEnemy(const std::string& id, const sf::Vector2f& pos) override;
    void pickCrystal(const sf::Vector2f& pos) override;
    void explode(const sf::Vector2f& pos) override;
    void addExp(int exp, const sf::Vector2f& pos, bool bonus_text = true) override;
    void checkLevel() override;

    [[nodiscard]] Game* getGame() const;

    Stats operator-(const Stats& stats) const;

private:
    Game* game_;

};

#endif //RAG3_GAME_INCLUDE_MISC_STATS_H
