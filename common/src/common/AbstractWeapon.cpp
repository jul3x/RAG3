//
// Created by jul3x on 16.10.19.
//

#include <common/AbstractWeapon.h>
#include <common/ResourceManager.h>


AbstractWeapon::AbstractWeapon(const sf::Vector2f& size,
                               const sf::Vector2f& weapon_offset,
                               const std::string& name) :
        name_(name),
        weapon_offset_(weapon_offset),
        time_elapsed_(0.0f),
        AbstractDrawableObject({}, size,
                               name.empty() ? nullptr : &RM.getTexture("weapons/" + name),
                               name.empty() ? 1 : utils::j3x::get<int>(RM.getObjectParams("weapons", name), "frames_number"),
                               name.empty() ? 0.0f :utils::j3x::get<float>(RM.getObjectParams("weapons", name), "frame_duration"))
{
}


void AbstractWeapon::registerSpawningFunction(std::function<void(const std::string&, const sf::Vector2f&, float)> func)
{
    spawning_function_ = func;
}

const std::string& AbstractWeapon::getName() const
{
    return name_;
}

const sf::Vector2f& AbstractWeapon::getWeaponOffset() const
{
    return weapon_offset_;
}

void AbstractWeapon::update(float time_elapsed)
{
    time_elapsed_ -= time_elapsed;
}