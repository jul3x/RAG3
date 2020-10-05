//
// Created by jul3x on 08.04.20.
//

#include <misc/Achievements.h>
#include <Game.h>


Achievements::Achievements(Stats* stats) : stats_(stats)
{
}

void Achievements::load(const std::string &path)
{
    achievements_by_condition_.clear();
    auto& achievements = RM.getListOfObjects(path);

    for (auto& achievement : achievements)
    {
        auto& params = RM.getObjectParams("achievements", achievement);

        achievements_by_condition_[j3x::get<std::string>(params, "condition")].emplace_back(std::make_pair(achievement, params));
    }
}

void Achievements::update(float time_elapsed)
{
    time_elapsed_ += time_elapsed;

    if (time_elapsed_ >= CONF<float>("achievements_update_time"))
    {
        Stats diff_stats = *stats_ - saved_stats_;

        try
        {
            if (diff_stats.getEnemiesKilled())
            {
                check("Kills", stats_->getEnemiesKilled(), diff_stats.getEnemiesKilled());
            }

            if (diff_stats.getCrystalsPicked())
            {
                check("Crystals", stats_->getCrystalsPicked(), diff_stats.getCrystalsPicked());
            }

            if (diff_stats.getExplosions())
            {
                check("BarrelsExplosions", stats_->getExplosions(), diff_stats.getExplosions());
            }
        }
        catch (const std::out_of_range& e)
        {
            LOG.info("[Achievements] No achievements with given condition found.");
        }

        saved_stats_ = *stats_;
        time_elapsed_ -= CONF<float>("achievements_update_time");
    }
}

void Achievements::check(const std::string &condition, int data, int delta_data)
{
    for (const auto& achievement : achievements_by_condition_.at(condition))
    {
        int compare_data = j3x::get<std::string>(achievement.second, "compare") == "InTime" ? delta_data : data;
        if (j3x::get<int>(achievement.second, "data") <= compare_data)
        {
            achievements_unlocked_.emplace(achievement.first);

            if (!achievements_unlocked_.count(achievement.first))
            {
                Game::get().spawnAchievement(achievement.second);
            }

            auto& bonus_text = j3x::get<std::string>(achievement.second, "bonus_text", true);
            auto& exp = j3x::get<int>(achievement.second, "exp", true);

            if (!bonus_text.empty())
                Game::get().spawnBonusText(Game::get().getPlayer().getPosition(), bonus_text);

            stats_->addExp(exp, Game::get().getPlayer().getPosition(), false);
        }
    }
}
