//
// Created by jul3x on 25.03.20.
//

#ifndef RAG3_GAME_UI_LEFTHUD_H
#define RAG3_GAME_UI_LEFTHUD_H


#include <R3E/objects/AbstractDrawableObject.h>


using namespace r3e;

class LeftHud : public AbstractDrawableObject {
public:
    explicit LeftHud(const sf::Vector2f& position);

    void updateState(float current, float max);

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    static constexpr float SIZE_X_ = 500.0f;
    static constexpr float SIZE_Y_ = 450.0f;

    static constexpr float CRYSTAL_OFFSET_X_ = 169.0f;
    static constexpr float CRYSTAL_OFFSET_Y_ = -160.0f;

    static constexpr float CRYSTAL_SIZE_X_ = 160.0f;
    static constexpr float CRYSTAL_SIZE_Y_ = 160.0f;

    AbstractDrawableObject crystal_;

};

#endif // RAG3_GAME_UI_LEFTHUD_H
