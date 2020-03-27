//
// Created by jul3x on 27.03.2020.
//

#ifndef RAG3_GAME_CHARACTERS_PLAYERCLONE_H
#define RAG3_GAME_CHARACTERS_PLAYERCLONE_H

#include <common/NPC.h>


using namespace r3e;

class PlayerClone : public NPC {
public:
    PlayerClone(const sf::Vector2f& position, float life_time);

    bool update(float time_elapsed, float time_factor) override;

    void updateLifeTimeDependingOnPrevious(float life_time);

    bool isLifeTime() const;

protected:
    void handleActionState() override;

    sf::Vector2f findNearestSafeSpot(const sf::Vector2f& direction) const override;

private:
    float life_time_;

};

#endif // RAG3_GAME_CHARACTERS_PLAYERCLONE_H
