//
// Created by jul3x on 01.04.20.
//

#ifndef RAG3_GAME_INCLUDE_MISC_THOUGHT_H
#define RAG3_GAME_INCLUDE_MISC_THOUGHT_H

#include <list>

#include <SFML/Graphics/Text.hpp>

#include <R3E/objects/AbstractPhysicalObject.h>


using namespace r3e;

class Thought : public AbstractPhysicalObject {
public:
    Thought(AbstractPhysicalObject* father, const std::string& text, float duration);

    bool update(float time_elapsed) override;

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    static constexpr float SIZE_X_ = 64.0f;
    static constexpr float SIZE_Y_BOTTOM_ = 16.0f;
    static constexpr float SIZE_Y_CENTER_ = 8.0f;
    static constexpr float SIZE_Y_TOP_ = 8.0f;
    static constexpr float GLOBAL_OFFSET_X_ = 16.0f;
    static constexpr float GLOBAL_OFFSET_Y_ = -32.0f;
    static constexpr float TEXT_MARGIN_ = 4.0f;
    static constexpr int TEXT_SIZE_ = 8;

    float time_elapsed_;

    sf::Text text_;

    AbstractPhysicalObject* father_;
    AbstractDrawableObject top_;
    std::vector<AbstractDrawableObject> center_;

};

#endif //RAG3_GAME_INCLUDE_MISC_THOUGHT_H
