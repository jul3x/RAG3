//
// Created by jul3x on 02.03.2022.
//

#ifndef RAG3_COMMON_INCLUDE_COMMON_ACTIVATIONAREA_H
#define RAG3_COMMON_INCLUDE_COMMON_ACTIVATIONAREA_H

#include <R3E/objects/AbstractPhysicalObject.h>


using namespace r3e;

class PlacedWeapon;

class ActivationArea : public HoveringObject {
public:
    ActivationArea(PlacedWeapon* father, float r) :
            father_(father),
            HoveringObject({}, {}, {r, r}, collision::Circle(r), nullptr, 0, 0, 0.0f, 0.0f),
            is_active_(false)
    {

    }

    PlacedWeapon* getFather() const
    {
        return father_;
    }

    bool isActive() const
    {
        return is_active_;
    }

    void setActive(bool active)
    {
        is_active_ = active;
    }

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override
    {

    }

    PlacedWeapon* father_;
    bool is_active_;

};

#endif //RAG3_COMMON_INCLUDE_COMMON_ACTIVATIONAREA_H
