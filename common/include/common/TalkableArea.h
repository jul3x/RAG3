//
// Created by jul3x on 09.05.2020.
//

#ifndef RAG3_COMMON_INCLUDE_COMMON_TALKABLEAREA_H
#define RAG3_COMMON_INCLUDE_COMMON_TALKABLEAREA_H

#include <R3E/objects/AbstractPhysicalObject.h>

using namespace r3e;

class Character;

class TalkableArea : public HoveringObject {
public:
    TalkableArea(Character* father, float r) :
        father_(father),
        HoveringObject({}, {}, {r, r}, collision::Circle(r), nullptr, 0, 0, 0.0f, 0.0f)
    {

    }

    Character* getFather() const
    {
        return father_;
    }

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override
    {

    }

    Character* father_ ;

};

#endif //RAG3_COMMON_INCLUDE_COMMON_TALKABLEAREA_H
