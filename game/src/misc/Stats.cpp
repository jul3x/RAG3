//
// Created by jul3x on 08.04.20.
//

#include <misc/Stats.h>
#include <Game.h>

Stats::Stats() : enemies_killed_(0), crystals_picked_(0), explosions_(0)
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

void Stats::killEnemy()
{
    ++enemies_killed_;

    if (enemies_killed_ == 1)
    {
        achievements_.emplace_back(Achievements::Type::FirstEnemyKill);

        Game::get().spawnAchievement(Achievements::Type::FirstEnemyKill);
    }
}

void Stats::pickCrystal()
{
    ++crystals_picked_;

    if (crystals_picked_ == 1)
    {
        achievements_.emplace_back(Achievements::Type::FirstCrystalPick);

        Game::get().spawnAchievement(Achievements::Type::FirstCrystalPick);
    }
}

void Stats::explode()
{
    ++explosions_;

    if (explosions_ == 1)
    {
        achievements_.emplace_back(Achievements::Type::FirstBarrelDestroyed);

        Game::get().spawnAchievement(Achievements::Type::FirstBarrelDestroyed);
    }
}
