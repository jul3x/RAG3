//
// Created by jul3x on 17.03.20.
//

#include <editor/ResourceManager.h>
#include <editor/Weapon.h>

using namespace editor;


Weapon::Weapon(const sf::Vector2f& position, const std::string& id) :
        AbstractDrawableObject(position,
                      RM.getObjectSize("weapons", id),
                      &RM.getTexture("weapons/" + id))
{
    this->changeOrigin(RM.getObjectSize("weapons", id) / 2.0f + RM.getObjectOffset("weapons", id));
}
