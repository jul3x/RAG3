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
                       sf::Vector2i(utils::j3x::get<float>(RM.getObjectParams("animations", id), "size_x"),
                                    utils::j3x::get<float>(RM.getObjectParams("animations", id), "size_y")),
                       position,
                       utils::j3x::get<int>(RM.getObjectParams("animations", id), "z_index"),
                       utils::j3x::get<float>(RM.getObjectParams("animations", id), "frame_duration"),
                       utils::j3x::get<int>(RM.getObjectParams("animations", id), "frames_number"),
                       utils::j3x::get<int>(RM.getObjectParams("animations", id), "animation_type_quadratic") ?
                       AnimationType::Quadratic : AnimationType::Linear)
{
    shape_.setRotation(direction);

    if (utils::j3x::get<int>(RM.getObjectParams("animations", id), "scale"))
        shape_.setScale(radius / shape_.getSize().x, radius / shape_.getSize().x);

    this->changeOrigin(sf::Vector2f(utils::j3x::get<float>(RM.getObjectParams("animations", id), "size_x"),
                                    utils::j3x::get<float>(RM.getObjectParams("animations", id), "size_y")) / 2.0f +
                       sf::Vector2f(utils::j3x::get<float>(RM.getObjectParams("animations", id), "map_offset_x"),
                                    utils::j3x::get<float>(RM.getObjectParams("animations", id), "map_offset_y")));

    if (utils::j3x::get<int>(RM.getObjectParams("animations", id), "light_point"))
    {
        light_ = std::make_unique<graphics::LightPoint>(this->getPosition(),
                                                        sf::Vector2f{CFG.get<float>("graphics/animations_light_point_size"),
                                                                     CFG.get<float>("graphics/animations_light_point_size")},
                                                        &RM.getTexture("lightpoint"));
    }
}
