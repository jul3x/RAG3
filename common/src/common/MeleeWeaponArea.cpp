//
// Created by jul3x on 12.05.2020.
//

#include <SFML/Graphics/CircleShape.hpp>

#include <common/MeleeWeaponArea.h>
#include <common/MeleeWeapon.h>


using namespace r3e;

MeleeWeaponArea::MeleeWeaponArea(MeleeWeapon* father, float r) :
            father_(father),
            r_(r),
            HoveringObject({}, {}, {r, r}, collision::Circle(r), nullptr, 0, 0, 0.0f, 0.0f)
{

}

MeleeWeapon* MeleeWeaponArea::getFather() const
{
    return father_;
}

void MeleeWeaponArea::setActive(bool active)
{
    if (active)
    {
        this->changeCollisionArea(collision::Circle(r_));
    }
    else
    {
        this->changeCollisionArea(collision::None());
    }
}

void MeleeWeaponArea::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
}
