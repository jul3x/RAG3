//
// Created by jul3x on 21.02.20.
//

#ifndef RAG3_GAME_INCLUDE_UI_BLOODSPLASH_H
#define RAG3_GAME_INCLUDE_UI_BLOODSPLASH_H

#include <R3E/objects/AbstractDrawableObject.h>

#include <common/characters/Player.h>


using namespace r3e;

class Framework;

class BloodSplash : public AbstractDrawableObject {
public:
    explicit BloodSplash(Framework* framework, const sf::Vector2f& size);

    void updateLifeState(Player::LifeState state);

    void resizeWindow(const sf::Vector2f& new_size);

    bool update(float time_elapsed);

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    AbstractDrawableObject dead_, low_, critical_;

    sf::Uint8 transparency_;
    float time_elapsed_, heartbeat_time_elapsed_;

    Player::LifeState player_life_state_;

    Framework* framework_;

};

#endif //RAG3_GAME_INCLUDE_UI_BLOODSPLASH_H
