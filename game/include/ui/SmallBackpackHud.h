//
// Created by jul3x on 06.10.20.
//

#ifndef RAG3_GAME_INCLUDE_UI_SMALLBACKPACKHUD_H
#define RAG3_GAME_INCLUDE_UI_SMALLBACKPACKHUD_H

#include <SFML/Graphics/Text.hpp>

#include <R3E/objects/AbstractDrawableObject.h>


using namespace r3e;

class SmallBackpackHud : public AbstractDrawableObject {
public:
    explicit SmallBackpackHud(const sf::Vector2f& position);

    void update(float time_elapsed);

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    static constexpr float SIZE_X_ = 540.0f;
    static constexpr float SIZE_Y_ = 200.0f;

    std::vector<Special> objects_;
    std::vector<sf::Text> numbers_;

};

#endif //RAG3_GAME_INCLUDE_UI_STATSHUD_H
