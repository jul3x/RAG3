//
// Created by jul3x on 25.03.20.
//

#ifndef RAG3_GAME_UI_RIGHTHUD_H
#define RAG3_GAME_UI_RIGHTHUD_H


#include <R3E/objects/AbstractDrawableObject.h>


using namespace r3e;

class RightHud : public AbstractDrawableObject {
public:
    explicit RightHud(const sf::Vector2f& position);

    void updateState(float current, float max);

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    static constexpr float SIZE_X_ = 550.0f;
    static constexpr float SIZE_Y_ = 500.0f;

    static constexpr float FACE_OFFSET_X_ = -164.0f;
    static constexpr float FACE_OFFSET_Y_ = -160.0f;

    static constexpr float FACE_SIZE_X_ = 160.0f;
    static constexpr float FACE_SIZE_Y_ = 160.0f;

    AbstractDrawableObject face_;

};

#endif // RAG3_GAME_UI_RIGHTHUD_H
