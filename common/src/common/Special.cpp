//
// Created by jul3x on 19.03.20.
//

#include <common/ResourceManager.h>
#include <common/Special.h>


Special::Special(const sf::Vector2f& position, const std::string& id, int u_id) :
        Special(position, id,
                utils::j3x::get<std::string>(RM.getObjectParams("specials", id), "default_activation"),
                utils::j3x::get<std::vector<std::string>>(RM.getObjectParams("specials", id), "default_functions"),
                utils::j3x::get<std::vector<std::string>>(RM.getObjectParams("specials", id), "default_datas"),
                utils::j3x::get<int>(RM.getObjectParams("specials", id), "default_active"), u_id)
{

}


Special::Special(const sf::Vector2f& position, const std::string& id,
                 const std::string& activation, const std::vector<std::string>& functions,
                 const std::vector<std::string>& datas, bool is_active, int u_id) :
        HoveringObject(position, {},
                       {utils::j3x::get<float>(RM.getObjectParams("specials", id), "size_x"),
                        utils::j3x::get<float>(RM.getObjectParams("specials", id), "size_y")},
                       collision::Circle(utils::j3x::get<float>(RM.getObjectParams("specials", id), "collision_size_x"),
                                         {utils::j3x::get<float>(RM.getObjectParams("specials", id), "collision_offset_x"),
                                          utils::j3x::get<float>(RM.getObjectParams("specials", id), "collision_offset_y")}),
                       &RM.getTexture("specials/" + id),
                       utils::j3x::get<int>(RM.getObjectParams("specials", id), "z_index"),
                       utils::j3x::get<int>(RM.getObjectParams("specials", id), "frames_number"),
                       utils::j3x::get<float>(RM.getObjectParams("specials", id), "frame_duration"),
                       0.0f),
        Functional(activation, functions, datas, id, u_id),
        is_drawable_(utils::j3x::get<int>(RM.getObjectParams("specials", id), "is_drawable")),
        additional_boolean_data_(false)
{
    this->changeOrigin(sf::Vector2f(utils::j3x::get<float>(RM.getObjectParams("specials", id), "size_x"),
                                    utils::j3x::get<float>(RM.getObjectParams("specials", id), "size_y")) / 2.0f +
                       sf::Vector2f(utils::j3x::get<float>(RM.getObjectParams("specials", id), "map_offset_x"),
                                    utils::j3x::get<float>(RM.getObjectParams("specials", id), "map_offset_y")));

    if (is_active)
        this->activate();
    else
        this->deactivate();

    if (utils::j3x::get<int>(RM.getObjectParams("specials", id), "light_point"))
    {
        float light_size = CFG.get<float>("graphics/specials_light_point_size") * CFG.get<float>("graphics/global_zoom");
        light_ = std::make_unique<graphics::LightPoint>(this->getPosition(),
                                                        sf::Vector2f{light_size, light_size},
                                                        &RM.getTexture("lightpoint"));
    }

    auto shadow_pos = this->getPosition() -
                      sf::Vector2f{utils::j3x::get<float>(RM.getObjectParams("specials", id), "map_offset_x"),
                                   utils::j3x::get<float>(RM.getObjectParams("specials", id), "map_offset_y")};

    if (utils::j3x::get<int>(RM.getObjectParams("specials", id), "shadow"))
    {
        static_shadow_ = std::make_unique<graphics::StaticTextureShadow>(
                shadow_pos, this->getSize(), CFG.get<float>("graphics/shadow_direction"),
                CFG.get<float>("graphics/shadow_length_factor"),
                &RM.getTexture("specials/" + id), sf::Color(CFG.get<int>("graphics/shadow_color")),
                z_index_,
                utils::j3x::get<int>(RM.getObjectParams("specials", id), "frames_number"),
                utils::j3x::get<float>(RM.getObjectParams("specials", id), "frame_duration"));
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

graphics::LightPoint* Special::getLightPoint() const
{
    return light_.get();
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
