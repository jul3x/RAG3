//
// Created by jul3x on 06.10.20.
//

#ifndef RAG3_GAME_INCLUDE_UI_LEVELHUD_H
#define RAG3_GAME_INCLUDE_UI_LEVELHUD_H

#include <SFML/Graphics/Text.hpp>

#include <R3E/objects/StateWithInertia.h>
#include <R3E/objects/AbstractDrawableObject.h>


using namespace r3e;

class LevelHud : public AbstractDrawableObject {
public:
    explicit LevelHud(const sf::Vector2f& position);

    void update(int level, int exp, float time_elapsed);

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    static constexpr float SIZE_X_ = 540.0f;
    static constexpr float SIZE_Y_ = 200.0f;

    static constexpr float LEVEL_X_ = -80.0f;
    static constexpr float LEVEL_Y_ = 0.0f;

    static constexpr float EXP_X_ = -100.0f;
    static constexpr float EXP_Y_ = 55.0f;

    StateWithInertia<float> level_, exp_;
    sf::Text level_text_, exp_text_;

};

#endif //RAG3_GAME_INCLUDE_UI_LEVELHUD_H
