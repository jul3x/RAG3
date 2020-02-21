//
// Created by jul3x on 21.02.20.
//

#ifndef RAG3_GAME_UI_BLOODSPLASH_H
#define RAG3_GAME_UI_BLOODSPLASH_H

#include <engine/objects/AbstractDrawableObject.h>
#include <game/characters/Player.h>


class BloodSplash : public AbstractDrawableObject {
public:
    explicit BloodSplash(const sf::Vector2f& size);

    void updateLifeState(Player::LifeState state);

    void resizeWindow(const sf::Vector2f& new_size);
private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    AbstractDrawableObject high_, low_, critical_;

    Player::LifeState player_life_state_;
};

#endif // RAG3_GAME_UI_BLOODSPLASH_H
