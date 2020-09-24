//
// Created by jul3x on 30.09.19.
//

#include <common/ResourceManager.h>

#include <events/Event.h>


Event::Event(const sf::Vector2f& position, const std::string& id) :
        Event(position, id, 0.0f, 1.0f)
{

}

Event::Event(const sf::Vector2f& position, const std::string& id,
             float direction,
             float radius) :
        AnimationEvent(&RM.getTexture("animations/" + id),
                       static_cast<sf::Vector2i>(RMGET<sf::Vector2f>("animations", id, "size")),
                       position,
                       RMGET<int>("animations", id, "z_index"),
                       RMGET<float>("animations", id, "frame_duration"),
                       RMGET<int>("animations", id, "frames_number"),
                       RMGET<bool>("animations", id, "animation_type_quadratic") ?
                       AnimationType::Quadratic : AnimationType::Linear)
{
    shape_.setRotation(direction);

    if (RMGET<bool>("animations", id, "scale"))
        shape_.setScale(radius / shape_.getSize().x, radius / shape_.getSize().x);

    this->changeOrigin(RMGET<sf::Vector2f>("animations", id, "size") / 2.0f +
                               RMGET<sf::Vector2f>("animations", id, "map_offset"));

    if (RMGET<bool>("animations", id, "light_point"))
    {
        light_ = std::make_unique<graphics::LightPoint>(this->getPosition(),
                                                        sf::Vector2f{CONF<float>("graphics/animations_light_point_size"),
                                                                     CONF<float>("graphics/animations_light_point_size")},
                                                        &RM.getTexture("lightpoint"));
    }
}
