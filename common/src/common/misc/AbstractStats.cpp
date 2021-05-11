//
// Created by jul3x on 24.03.2021.
//

#include <common/misc/AbstractStats.h>


int AbstractStats::getEnemiesKilled() const
{
    return enemies_killed_;
}

int AbstractStats::getCrystalsPicked() const
{
    return crystals_picked_;
}

int AbstractStats::getExplosions() const
{
    return explosions_;
}

int AbstractStats::getExp() const
{
    return exp_;
}

int AbstractStats::getLevel() const
{
    return level_;
}

AbstractStats::AbstractStats() :
        enemies_killed_(0), crystals_picked_(0), explosions_(0), exp_(0), level_(0)
{

}

AbstractStats::AbstractStats(int kills, int crystals, int explosions, int exp, int level) :
        enemies_killed_(kills), crystals_picked_(crystals), explosions_(explosions), exp_(exp), level_(level)
{

}

void AbstractStats::killEnemy(const std::string& id, const sf::Vector2f& pos)
{
    ++enemies_killed_;
}

void AbstractStats::pickCrystal(const sf::Vector2f& pos)
{
    ++crystals_picked_;
}

void AbstractStats::explode(const sf::Vector2f& pos)
{
    ++explosions_;
}

void AbstractStats::addExp(int exp, const sf::Vector2f& info_pos, bool bonus_text)
{
}

void AbstractStats::checkLevel()
{
}

void AbstractStats::setEnemiesKilled(int count)
{
    enemies_killed_ = count;
}

void AbstractStats::setCrystalsPicked(int count)
{
    crystals_picked_ = count;
}

void AbstractStats::setExplosions(int count)
{
    explosions_ = count;
}

void AbstractStats::setExp(int count)
{
    exp_ = count;
}

void AbstractStats::setLevel(int count)
{
    level_ = count;
}
