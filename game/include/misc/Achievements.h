//
// Created by jul3x on 08.04.20.
//

#ifndef RAG3_GAME_MISC_ACHIEVEMENTS_H
#define RAG3_GAME_MISC_ACHIEVEMENTS_H

#include <unordered_map>
#include <string>


class Achievements {
public:
    enum class Type {
        FirstEnemyKill,
        FirstCrystalPick,
        FirstBarrelDestroyed
    };

    Achievements();

    const std::string& getAchievementText(Achievements::Type type) const;
    const std::string& getAchievementTitle(Achievements::Type type) const;
    const std::string& getAchievementTexture(Achievements::Type type) const;

private:
    std::unordered_map<Achievements::Type, std::string> achievements_texts_;
    std::unordered_map<Achievements::Type, std::string> achievements_titles_;
    std::unordered_map<Achievements::Type, std::string> achievements_textures_;

};

#endif //RAG3_GAME_MISC_ACHIEVEMENTS_H
