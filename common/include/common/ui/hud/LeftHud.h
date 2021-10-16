//
// Created by jul3x on 25.03.20.
//

#ifndef RAG3_GAME_INCLUDE_UI_LEFTHUD_H
#define RAG3_GAME_INCLUDE_UI_LEFTHUD_H


#include <R3E/objects/AbstractDrawableObject.h>


using namespace r3e;

class LeftHud : public AbstractDrawableObject {
public:
    explicit LeftHud(const sf::Vector2f& position);

private:

    static constexpr float SIZE_X_ = 590.0f;
    static constexpr float SIZE_Y_ = 250.0f;

};

#endif //RAG3_GAME_INCLUDE_UI_LEFTHUD_H
