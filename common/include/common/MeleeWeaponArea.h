//
// Created by jul3x on 09.05.2020.
//

#ifndef RAG3_COMMON_INCLUDE_COMMON_MELEEWEAPONAREA_H
#define RAG3_COMMON_INCLUDE_COMMON_MELEEWEAPONAREA_H

#include <R3E/objects/AbstractPhysicalObject.h>

using namespace r3e;

class MeleeWeapon;

class MeleeWeaponArea : public HoveringObject {
public:
    MeleeWeaponArea(MeleeWeapon* father, float r);

    MeleeWeapon* getFather() const;

    void setActive(bool active);

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    MeleeWeapon* father_;
    float r_;

};

#endif //RAG3_COMMON_INCLUDE_COMMON_MELEEWEAPONAREA_H
