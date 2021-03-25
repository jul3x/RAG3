//
// Created by jul3x on 08.04.20.
//

#include <misc/Stats.h>

#include <Game.h>


Stats::Stats() : Stats(nullptr)
{

}

Stats::Stats(Game* game) :
        game_(game), AbstractStats()
{
}

Stats::Stats(int kills, int crystals, int explosions, int exp, int level) :
        AbstractStats(kills, crystals, explosions, exp, level)
{
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
            game_->spawnBonusText(info_pos, std::to_string(exp));
    }
}

void Stats::checkLevel()
{
    static auto& experience_list = CONF<j3x::List>("characters/levels_experience");

    if (level_ < experience_list.size() &&
        exp_ >= j3x::getObj<int>(experience_list, static_cast<size_t>(level_), false))
    {
        ++level_;

        game_->getPlayer()->addSkillPoints(CONF<int>("characters/skill_points_per_level"));
    }
}

Stats Stats::operator-(const Stats& stats) const
{
    return Stats(
            this->getEnemiesKilled() - stats.getEnemiesKilled(),
            this->getCrystalsPicked() - stats.getCrystalsPicked(),
            this->getExplosions() - stats.getExplosions(),
            this->getExp() - stats.getExp(),
            this->getLevel() - stats.getLevel()
    );
}

Game* Stats::getGame() const
{
    return game_;
}
