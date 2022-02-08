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
    Thought(AbstractPhysicalObject* father, std::string text, float duration);

    bool update(float time_elapsed) override;
    static inline std::string wrapText(std::string text);

    AbstractPhysicalObject* getFather() const;

private:

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    float time_elapsed_;
    sf::Text text_;

    AbstractPhysicalObject* father_;
    AbstractDrawableObject top_;
    std::vector<AbstractDrawableObject> center_;

};

#endif //RAG3_GAME_INCLUDE_MISC_THOUGHT_H
