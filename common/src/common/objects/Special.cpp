//
// Created by jul3x on 19.03.20.
//

#include <common/ResourceManager.h>
#include <common/objects/Special.h>


Special::Special(const sf::Vector2f& position, const std::string& id, int u_id) :
        Special(position, id,
                RMGET<std::string>("specials", id, "default_activation"),
                RMGET<j3x::List>("specials", id, "default_functions"),
                RMGET<j3x::List>("specials", id, "default_datas"),
                RMGET<bool>("specials", id, "default_active"), u_id)
{

}


Special::Special(const sf::Vector2f& position, const std::string& id,
                 const std::string& activation, const j3x::List& functions,
                 const j3x::List& datas, bool is_active, int u_id) :
        HoveringObject(position, {},
                       RMGET<sf::Vector2f>("specials", id, "size"),
                       collision::None(),
                       &RM.getTexture(
                               "specials/" + (RMGET<bool>("specials", id, "is_drawable") ? id : "special_event")),
                       RMGET<int>("specials", id, "z_index"),
                       RMGET<int>("specials", id, "frames_number"),
                       RMGET<float>("specials", id, "frame_duration"),
                       0.0f),
        Functional(activation, functions, datas, id, u_id),
        is_drawable_(RMGET<bool>("specials", id, "is_drawable")),
        additional_boolean_data_(false)
{
    this->changeOrigin(
            RMGET<sf::Vector2f>("specials", id, "size") / 2.0f + RMGET<sf::Vector2f>("specials", id, "map_offset"));
    if (RMHAS<float>("specials", id, "collision_radius"))
        this->changeCollisionArea(collision::Circle(RMGET<float>("specials", id, "collision_radius"),
                                                    RMGET<sf::Vector2f>("specials", id, "collision_offset")));
    else
        this->changeCollisionArea(collision::Box(RMGET<float>("specials", id, "collision_size_x"),
                                                 RMGET<float>("specials", id, "collision_size_y"),
                                                 RMGET<sf::Vector2f>("specials", id, "collision_offset")));


    if (is_active)
        this->activate();
    else
        this->deactivate();

    this->makeLightPoint(this->getPosition(),
                         CONF<float>("graphics/specials_light_point_size") * CONF<float>("graphics/global_zoom"),
                         &RM.getTexture("lightpoint"), RMGET<std::string>("specials", id, "light_point"),
                         RMGET<float>("specials", id, "light_point_data", true));

    auto shadow_pos = this->getPosition() - RMGET<sf::Vector2f>("specials", id, "map_offset");

    if (RMGET<bool>("specials", id, "shadow"))
    {
        static_shadow_ = std::make_unique<graphics::TransformedTextureShadow>(
                shadow_pos, this->getSize(), CONF<float>("graphics/shadow_direction"),
                CONF<float>("graphics/shadow_length_factor"),
                &RM.getTexture("specials/" + id), sf::Color(CONF<int>("graphics/shadow_color")),
                z_index_,
                RMGET<int>("specials", id, "frames_number"),
                RMGET<float>("specials", id, "frame_duration"));
    }
}

bool Special::isDrawable() const
{
    return is_drawable_;
}

bool Special::getAdditionalBooleanData() const
{
    return additional_boolean_data_;
}

void Special::setAdditionalBooleanData(bool data)
{
    additional_boolean_data_ = data;
}

graphics::StaticShadow* Special::getShadow() const
{
    return static_shadow_.get();
}

bool Special::updateAnimation(float time_elapsed, float animation_speed_factor)
{
    if (static_shadow_ != nullptr)
        static_shadow_->updateAnimation(time_elapsed, animation_speed_factor);
    return AbstractDrawableObject::updateAnimation(time_elapsed, animation_speed_factor);
}

void Special::setCurrentFrame(short int frame)
{
    AbstractDrawableObject::setCurrentFrame(frame);
    if (static_shadow_ != nullptr)
        static_shadow_->setCurrentFrame(frame);
}

void Special::removeShadow()
{
    static_shadow_.reset();
}

void Special::changeTexture(sf::Texture* texture, bool reset)
{
    AbstractDrawableObject::changeTexture(texture, reset);
    if (static_shadow_ != nullptr)
        static_shadow_->changeTexture(texture);
}


void Special::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (static_shadow_ != nullptr)
        target.draw(*static_shadow_, states);
    target.draw(shape_, states);
}

void Special::setPosition(const sf::Vector2f& pos)
{
    AbstractDrawableObject::setPosition(pos);

    if (light_ != nullptr)
        light_->setPosition(pos);
    if (static_shadow_ != nullptr)
        static_shadow_->setPosition(pos - RMGET<sf::Vector2f>("specials", this->getId(), "map_offset") +
                                    RMGET<sf::Vector2f>("specials", this->getId(), "shadow_offset", true));
}
