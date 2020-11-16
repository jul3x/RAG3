//
// Created by jul3x on 08.04.20.
//

#include <misc/Stats.h>
#include <Game.h>


Stats::Stats() :
    enemies_killed_(0), crystals_picked_(0), explosions_(0), exp_(0), level_(0)
{
}

Stats::Stats(int kills, int crystals, int explosions, int exp, int level) :
        enemies_killed_(kills), crystals_picked_(crystals), explosions_(explosions), exp_(exp), level_(level)
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

int Stats::getExp() const
{
    return exp_;
}

int Stats::getLevel() const
{
    return level_;
}

void Stats::killEnemy(const sf::Vector2f& pos)
{
    ++enemies_killed_;
    this->addExp(CONF<int>("enemy_kill_exp"), pos);
}

void Stats::pickCrystal(const sf::Vector2f& pos)
{
    ++crystals_picked_;
    this->addExp(CONF<int>("crystal_pick_exp"), pos);
}

void Stats::explode(const sf::Vector2f& pos)
{
    ++explosions_;
}

void Stats::addExp(int exp, const sf::Vector2f& info_pos, bool bonus_text)
{
    if (exp > 0)
    {
        exp_ += exp;

        this->checkLevel();

        if (bonus_text)
            Game::get().spawnBonusText(info_pos, std::to_string(exp));
    }
}

void Stats::checkLevel()
{
    static auto& experience_list = CONF<j3x::List>("characters/levels_experience");

    if (level_ < experience_list.size() && exp_ >= j3x::getObj<int>(experience_list, static_cast<size_t>(level_), false))
    {
        ++level_;

        Game::get().getPlayer().addSkillPoints(CONF<int>("characters/skill_points_per_level"));
    }
}

Stats Stats::operator-(const Stats &stats) const {
    return Stats(
            this->getEnemiesKilled() - stats.getEnemiesKilled(),
            this->getCrystalsPicked() - stats.getCrystalsPicked(),
            this->getExplosions() - stats.getExplosions(),
            this->getExp() - stats.getExp(),
            this->getLevel() - stats.getLevel()
            );
}
