//
// Created by jul3x on 10.03.20.
//

#include <editor/ResourceManager.h>
#include <editor/Character.h>

using namespace editor;

Character::Character(const sf::Vector2f& position, const std::string& id) :
        AbstractDrawableObject(position,
                               RM.getObjectSize("characters", id),
                               &RM.getTexture("characters/" + id))
{
    this->changeOrigin(RM.getObjectSize("characters", id) / 2.0f + RM.getObjectOffset("characters", id));
}
