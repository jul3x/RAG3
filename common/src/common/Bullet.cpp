//
// Created by jul3x on 01.10.19.
//

#include <R3E/system/Config.h>

#include <common/Bullet.h>
#include <common/ResourceManager.h>


Bullet::Bullet(const sf::Vector2f& position,
               const std::string& id,
               const float direction) :
        Identifiable(id),
        HoveringObject(position,
                       utils::getFloat(RM.getObjectParams("bullets", id), "speed") *
                               sf::Vector2f(std::cos(direction), std::sin(direction)),
                       {utils::getFloat(RM.getObjectParams("bullets", id), "size_x"),
                        utils::getFloat(RM.getObjectParams("bullets", id), "size_y")},
                       Collision::Box(utils::getFloat(RM.getObjectParams("bullets", id), "size_x"),
                                      utils::getFloat(RM.getObjectParams("bullets", id), "size_y")),
                       &RM.getTexture("bullets/" + id),
                       0, 0.0f,
                       sf::Color(CFG.getInt("graphics/trail_color")),
                       0.0f)
{
    this->setRotation(direction * 180.0f / static_cast<float>(M_PI));
    life_ = utils::getFloat(RM.getObjectParams("bullets", id), "life");
    deadly_factor_ = utils::getInt(RM.getObjectParams("bullets", id), "deadly_factor");
}

int Bullet::getDeadlyFactor() const
{
    return deadly_factor_;
}

void Bullet::setDead()
{
    life_ = -1.0f;
}

bool Bullet::update(float time_elapsed)
{
    DynamicObject::update(time_elapsed);

    life_ -= time_elapsed * 1000.0f;

    return life_ > 0.0f;
}