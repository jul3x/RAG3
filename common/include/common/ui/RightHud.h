//
// Created by jul3x on 25.03.20.
//

#ifndef RAG3_GAME_INCLUDE_UI_RIGHTHUD_H
#define RAG3_GAME_INCLUDE_UI_RIGHTHUD_H


#include <R3E/objects/AbstractDrawableObject.h>


using namespace r3e;

class RightHud : public AbstractDrawableObject {
public:
    explicit RightHud(const sf::Vector2f& position);

    void update(int level, int exp, float time_elapsed);

    void setName(const std::string& name);

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    static constexpr float SIZE_X_ = 940.0f;
    static constexpr float SIZE_Y_ = 250.0f;

    static constexpr float FACE_OFFSET_X_ = -71.0f;
    static constexpr float FACE_OFFSET_Y_ = -64.0f;

    static constexpr float FACE_SIZE_X_ = 109.0f;
    static constexpr float FACE_SIZE_Y_ = 99.0f;

    static constexpr float LEVEL_X_ = -145.0f;
    static constexpr float LEVEL_Y_ = -66.0f;

    static constexpr float NAME_X_ = -145.0f;
    static constexpr float NAME_Y_ = -110.0f;

    StateWithInertia<float> level_, exp_;
    sf::Text name_, level_text_;
    AbstractDrawableObject face_;

};

#endif //RAG3_GAME_INCLUDE_UI_RIGHTHUD_H
