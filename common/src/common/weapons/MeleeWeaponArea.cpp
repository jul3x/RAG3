//
// Created by jul3x on 12.05.2020.
//

#include <SFML/Graphics/CircleShape.hpp>

#include <common/weapons/MeleeWeaponArea.h>
#include <common/weapons/MeleeWeapon.h>


using namespace r3e;

MeleeWeaponArea::MeleeWeaponArea(MeleeWeapon* father, float r) :
            father_(father),
            r_(r),
            HoveringObject({}, {}, {r, r}, collision::None(), nullptr, 0, 0, 0.0f, 0.0f)
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
    static sf::CircleShape shape;

    shape.setFillColor(sf::Color::Red);
    shape.setPosition(this->getPosition());
    shape.setOrigin(r_/2.0, r_/2.0);
    shape.setRadius(r_);

    target.draw(shape);
}

bool MeleeWeaponArea::isActive() const
{
    return this->getCollisionArea().getType() != collision::Area::Type::None;
}
