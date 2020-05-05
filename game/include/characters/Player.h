//
// Created by jul3x on 03.11.19.
//


#ifndef RAG3_GAME_CHARACTERS_PLAYER_H
#define RAG3_GAME_CHARACTERS_PLAYER_H

#include <common/Character.h>


class Player : public Character {
public:
    enum class SideStepDir {
        Left = -1,
        Right = 1
    };

    explicit Player(const sf::Vector2f& position);

    void setDead();

    bool isAlive() const;

    float getMaxTimeManipulation() const;

    bool sideStep(Player::SideStepDir dir);

    bool update(float time_elapsed) override;

    void setHealth(float life) override;

    void getShot(const Bullet& bullet) override;

protected:
    inline void handleGlobalState(float time_elapsed) override;

private:
    float side_stepping_freeze_time_;

    bool is_alive_;

};

#endif // RAG3_GAME_CHARACTERS_PLAYER_H
