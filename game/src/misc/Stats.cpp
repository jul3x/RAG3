//
// Created by jul3x on 08.04.20.
//

#include <misc/Stats.h>
#include <Game.h>


Stats::Stats() :
    enemies_killed_(0), crystals_picked_(0), explosions_(0)
{
}

Stats::Stats(int kills, int crystals, int explosions) :
        enemies_killed_(kills), crystals_picked_(crystals), explosions_(explosions)
{
}

int Stats::getEnemiesKilled() const
{
    return enemies_killed_;
}

int Stats::getCrystalsPicked() const
{
    return crystals_picked_;
}

int Stats::getExplosions() const
{
    return explosions_;
}

void Stats::killEnemy()
{
    ++enemies_killed_;
}

void Stats::pickCrystal()
{
    ++crystals_picked_;
}

void Stats::explode()
{
    ++explosions_;
}

Stats Stats::operator-(const Stats &stats) const {
    return Stats(
            this->getEnemiesKilled() - stats.getEnemiesKilled(),
            this->getCrystalsPicked() - stats.getCrystalsPicked(),
            this->getExplosions() - stats.getExplosions()
            );
}
