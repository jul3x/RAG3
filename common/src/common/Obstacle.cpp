//
// Created by jul3x on 23.03.20.
//

#include <R3E/system/Config.h>

#include <common/Obstacle.h>
#include <common/ResourceManager.h>


Obstacle::Obstacle(const sf::Vector2f& position, const std::string& id, int u_id) :
    Obstacle(position, id,
             utils::j3x::get<std::string>(RM.getObjectParams("obstacles", id), "default_activation"),
             utils::j3x::get<std::vector<std::string>>(RM.getObjectParams("obstacles", id), "default_functions"),
             utils::j3x::get<std::vector<std::string>>(RM.getObjectParams("obstacles", id), "default_datas"), u_id)
{

}

Obstacle::Obstacle(const sf::Vector2f& position, const std::string& id, const std::string& activation,
                   const std::vector<std::string>& functions, const std::vector<std::string>& datas, int u_id) :
        Functional(activation, functions, datas, id, u_id),
        StaticObject(position,
                     {utils::j3x::get<float>(RM.getObjectParams("obstacles", id), "size_x"),
                      utils::j3x::get<float>(RM.getObjectParams("obstacles", id), "size_y")},
                     collision::Box(utils::j3x::get<float>(RM.getObjectParams("obstacles", id), "collision_size_x"),
                                    utils::j3x::get<float>(RM.getObjectParams("obstacles", id), "collision_size_y"),
                                    {utils::j3x::get<float>(RM.getObjectParams("obstacles", id), "collision_offset_x"),
                                     utils::j3x::get<float>(RM.getObjectParams("obstacles", id), "collision_offset_y")}),
                     &RM.getTexture("obstacles/" + id),
                     utils::j3x::get<int>(RM.getObjectParams("obstacles", id), "z_index"),
                     utils::j3x::get<int>(RM.getObjectParams("obstacles", id), "frames_number"),
                     utils::j3x::get<float>(RM.getObjectParams("obstacles", id), "frame_duration")),
        Shootable(utils::j3x::get<float>(RM.getObjectParams("obstacles", id), "endurance") *
                  CFG.get<float>("obstacles_endurance_factor"))
{
    this->changeOrigin(sf::Vector2f(utils::j3x::get<float>(RM.getObjectParams("obstacles", id), "size_x"),
                                    utils::j3x::get<float>(RM.getObjectParams("obstacles", id), "size_y")) / 2.0f +
                       sf::Vector2f(utils::j3x::get<float>(RM.getObjectParams("obstacles", id), "map_offset_x"),
                                    utils::j3x::get<float>(RM.getObjectParams("obstacles", id), "map_offset_y")));

    if (utils::j3x::get<int>(RM.getObjectParams("obstacles", id), "light_point"))
    {
        float light_size = CFG.get<float>("graphics/obstacles_light_point_size") * CFG.get<float>("graphics/global_zoom");
        light_ = std::make_unique<graphics::LightPoint>(this->getPosition(),
                                                        sf::Vector2f{light_size, light_size},
                                                        &RM.getTexture("lightpoint"));
    }

    auto shadow_pos = this->getPosition() -
                      sf::Vector2f{utils::j3x::get<float>(RM.getObjectParams("obstacles", id), "map_offset_x"),
                                   utils::j3x::get<float>(RM.getObjectParams("obstacles", id), "map_offset_y")};

    if (utils::j3x::get<int>(RM.getObjectParams("obstacles", id), "shadow"))
    {
        auto shadow_texture_suffix =
                utils::j3x::get<std::string>(RM.getObjectParams("obstacles", id), "shadow_texture_suffix", true);
        if (shadow_texture_suffix.empty())
        {
            static_shadow_ = std::make_unique<graphics::TransformedTextureShadow>(
                    shadow_pos, this->getSize(), CFG.get<float>("graphics/shadow_direction"),
                    CFG.get<float>("graphics/shadow_length_factor"),
                    &RM.getTexture("obstacles/" + id),
                    sf::Color(CFG.get<int>("graphics/shadow_color")), z_index_,
                    utils::j3x::get<int>(RM.getObjectParams("obstacles", id), "frames_number"),
                    utils::j3x::get<float>(RM.getObjectParams("obstacles", id), "frame_duration"));
        }
        else
        {
            auto shadow_offset =  sf::Vector2f(utils::j3x::get<float>(RM.getObjectParams("obstacles", id), "shadow_offset_x"),
                                               utils::j3x::get<float>(RM.getObjectParams("obstacles", id), "shadow_offset_y"));
            auto shadow_size = sf::Vector2f(utils::j3x::get<float>(RM.getObjectParams("obstacles", id), "shadow_size_x"),
                                            utils::j3x::get<float>(RM.getObjectParams("obstacles", id), "shadow_size_y"));
            static_shadow_ = std::make_unique<graphics::StaticTextureShadow>(
                    shadow_pos + shadow_offset, shadow_size,
                    &RM.getTexture("obstacles/" + id + shadow_texture_suffix),
                    sf::Color(CFG.get<int>("graphics/shadow_color")), z_index_,
                    utils::j3x::get<int>(RM.getObjectParams("obstacles", id), "frames_number"),
                    utils::j3x::get<float>(RM.getObjectParams("obstacles", id), "frame_duration"));
        }
    }
}

bool Obstacle::update(float time_elapsed)
{
    return life_ > 0;
}

graphics::StaticShadow* Obstacle::getShadow() const
{
    return static_shadow_.get();
}

bool Obstacle::updateAnimation(float time_elapsed, float animation_speed_factor)
{
    if (static_shadow_ != nullptr)
        static_shadow_->updateAnimation(time_elapsed, animation_speed_factor);
    return AbstractDrawableObject::updateAnimation(time_elapsed, animation_speed_factor);
}

void Obstacle::setCurrentFrame(short int frame)
{
    AbstractDrawableObject::setCurrentFrame(frame);
    if (static_shadow_ != nullptr)
        static_shadow_->setCurrentFrame(frame);
}

void Obstacle::changeTexture(sf::Texture* texture, bool reset)
{
    AbstractDrawableObject::changeTexture(texture, reset);
    if (static_shadow_ != nullptr)
        static_shadow_->changeTexture(texture);
}

void Obstacle::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (static_shadow_ != nullptr)
        target.draw(*static_shadow_, states);
    target.draw(shape_, states);
}

