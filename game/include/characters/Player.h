//
// Created by jul3x on 03.11.19.
//

#ifndef RAG3_GAME_INCLUDE_CHARACTERS_PLAYER_H
#define RAG3_GAME_INCLUDE_CHARACTERS_PLAYER_H

#include <common/Character.h>


class Player : public Character {
public:
    enum class SideStepDir {
        Left = -1,
        Right = 1
    };

    explicit Player(const sf::Vector2f& position);

    void setDead();
    void setHealth(float life) override;

    bool isAlive() const;
    void getShot(const Bullet& bullet) override;
    void getCut(const MeleeWeapon& weapon) override;
    float getMaxTimeManipulation() const;
    std::list<std::pair<Special, int>>& getBackpack();

    void addSpecialToBackpack(Special* special);
    void useItem(const std::string& name);
    bool sideStep(Player::SideStepDir dir);
    bool update(float time_elapsed) override;

protected:
    inline void handleGlobalState(float time_elapsed) override;

private:
    float side_stepping_freeze_time_;

    bool is_alive_;

    std::list<std::pair<Special, int>> backpack_;

};

#endif //RAG3_GAME_INCLUDE_CHARACTERS_PLAYER_H
