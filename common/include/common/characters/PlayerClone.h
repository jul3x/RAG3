//
// Created by jul3x on 27.03.2020.
//

#ifndef RAG3_GAME_INCLUDE_CHARACTERS_PLAYERCLONE_H
#define RAG3_GAME_INCLUDE_CHARACTERS_PLAYERCLONE_H

#include <common/characters/NPC.h>

#include <common/characters/Player.h>


using namespace r3e;

class PlayerClone : public NPC {
public:
    PlayerClone(const sf::Vector2f& position, const Player* player, float life_time);

    bool update(float time_elapsed) override;

    void updateLifeTimeDependingOnPrevious(float life_time);

    bool isLifeTime() const;

private:
    float life_time_;

    const Player* player_;

};

#endif //RAG3_GAME_INCLUDE_CHARACTERS_PLAYERCLONE_H
