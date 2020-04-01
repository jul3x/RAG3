//
// Created by jul3x on 01.04.20.
//

#ifndef RAG3_GAME_MISC_THOUGHT_H
#define RAG3_GAME_MISC_THOUGHT_H

#include <SFML/Graphics/Text.hpp>

#include <R3E/objects/AbstractPhysicalObject.h>


using namespace r3e;

class Thought : public AbstractPhysicalObject {
public:
    Thought(AbstractPhysicalObject* father, const std::string& text, float duration);

    bool update(float time_elapsed) override;
private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    float time_elapsed_;

    sf::Text text_;

    AbstractPhysicalObject* father_;

};

#endif //RAG3_GAME_MISC_THOUGHT_H
