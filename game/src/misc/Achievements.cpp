//
// Created by jul3x on 08.04.20.
//

#include <misc/Achievements.h>


Achievements::Achievements()
{
    achievements_titles_[Achievements::Type::FirstEnemyKill] = "First enemy killed";
    achievements_titles_[Achievements::Type::FirstCrystalPick] = "Crystal acknowledgement";

    achievements_texts_[Achievements::Type::FirstEnemyKill] = "First blood! Keep it going!";
    achievements_texts_[Achievements::Type::FirstCrystalPick] = "What a strange object...";

    achievements_textures_[Achievements::Type::FirstEnemyKill] = "achievements/blood";
    achievements_textures_[Achievements::Type::FirstCrystalPick] = "achievements/crystal";
}

const std::string& Achievements::getAchievementText(Achievements::Type type) const
{
    return achievements_texts_.at(type);
}

const std::string& Achievements::getAchievementTitle(Achievements::Type type) const
{
    return achievements_titles_.at(type);
}

const std::string& Achievements::getAchievementTexture(Achievements::Type type) const
{
    return achievements_textures_.at(type);
}
