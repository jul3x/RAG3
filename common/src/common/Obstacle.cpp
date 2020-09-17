//
// Created by jul3x on 23.03.20.
//

#include <R3E/system/Config.h>

#include <common/Obstacle.h>
#include <common/ResourceManager.h>


Obstacle::Obstacle(const sf::Vector2f& position, const std::string& id, int u_id) :
    Obstacle(position, id,
             RMGET<std::string>("obstacles", id, "default_activation"),
             RMGET<j3x::List>("obstacles", id, "default_functions"),
             RMGET<j3x::List>("obstacles", id, "default_datas"), u_id)
{

}

Obstacle::Obstacle(const sf::Vector2f& position, const std::string& id, const std::string& activation,
                   const j3x::List& functions, const j3x::List& datas, int u_id) :
        Functional(activation, functions, datas, id, u_id),
        StaticObject(position,
                     RMGET<sf::Vector2f>("obstacles", id, "size"),
                     collision::Box(RMGET<sf::Vector2f>("obstacles", id, "collision_size").x,
                                    RMGET<sf::Vector2f>("obstacles", id, "collision_size").y,
                                    RMGET<sf::Vector2f>("obstacles", id, "collision_offset")),
                     &RM.getTexture("obstacles/" + id),
                     RMGET<int>("obstacles", id, "z_index"),
                     RMGET<int>("obstacles", id, "frames_number"),
                     RMGET<float>("obstacles", id, "frame_duration")),
        Shootable(RMGET<float>("obstacles", id, "endurance") *
                  CONF<float>("obstacles_endurance_factor"))
{
    this->changeOrigin(RMGET<sf::Vector2f>("obstacles", id, "size") / 2.0f +
                               RMGET<sf::Vector2f>("obstacles", id, "map_offset"));

    if (RMGET<bool>("obstacles", id, "light_point"))
    {
        float light_size = CONF<float>("graphics/obstacles_light_point_size") * CONF<float>("graphics/global_zoom");
        light_ = std::make_unique<graphics::LightPoint>(this->getPosition(),
                                                        sf::Vector2f{light_size, light_size},
                                                        &RM.getTexture("lightpoint"));
    }

    auto shadow_pos = this->getPosition() - RMGET<sf::Vector2f>("obstacles", id, "map_offset");

    if (RMGET<bool>("obstacles", id, "shadow"))
    {
        auto shadow_texture_suffix = RMGET<std::string>("obstacles", id, "shadow_texture_suffix", true);
        if (shadow_texture_suffix.empty())
        {
            static_shadow_ = std::make_unique<graphics::TransformedTextureShadow>(
                    shadow_pos, this->getSize(), CONF<float>("graphics/shadow_direction"),
                    CONF<float>("graphics/shadow_length_factor"),
                    &RM.getTexture("obstacles/" + id),
                    sf::Color(CONF<int>("graphics/shadow_color")), z_index_,
                    RMGET<int>("obstacles", id, "frames_number"),
                    RMGET<float>("obstacles", id, "frame_duration"));
        }
        else
        {
            auto& shadow_offset = RMGET<sf::Vector2f>("obstacles", id, "shadow_offset");
            auto& shadow_size = RMGET<sf::Vector2f>("obstacles", id, "shadow_size");
            static_shadow_ = std::make_unique<graphics::StaticTextureShadow>(
                    shadow_pos + shadow_offset, shadow_size,
                    &RM.getTexture("obstacles/" + id + shadow_texture_suffix),
                    sf::Color(CONF<int>("graphics/shadow_color")), z_index_,
                    RMGET<int>("obstacles", id, "frames_number"),
                    RMGET<float>("obstacles", id, "frame_duration"));
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

