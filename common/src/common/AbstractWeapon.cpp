//
// Created by jul3x on 16.10.19.
//

#include <common/AbstractWeapon.h>
#include <common/ShootingWeapon.h>
#include <common/NoWeapon.h>
#include <common/MeleeWeapon.h>
#include <common/ResourceManager.h>

#include <utility>


AbstractWeapon::AbstractWeapon(Character* user,
                               const sf::Vector2f& size,
                               const sf::Vector2f& weapon_offset,
                               const std::string& name) :
        weapon_offset_(weapon_offset),
        Identifiable(name),
        user_(user),
        AbstractDrawableObject({}, size,
                               name.empty() ? nullptr : &RM.getTexture("weapons/" + name),
                               name.empty() ? 0 : RMGET<int>("weapons", name, "z_index"),
                               name.empty() ? 1 : RMGET<int>("weapons", name, "frames_number"),
                               name.empty() ? 0.0f : RMGET<float>("weapons", name, "frame_duration"))
{
}


void AbstractWeapon::registerSpawningFunction(
        std::function<void(Character*, const std::string&, const sf::Vector2f&, float)> func)
{
    spawning_function_ = std::move(func);
}

const sf::Vector2f& AbstractWeapon::getWeaponOffset() const
{
    return weapon_offset_;
}

void AbstractWeapon::update(float time_elapsed)
{
    time_elapsed_ -= time_elapsed;
}

Character* AbstractWeapon::getUser() const
{
    return user_;
}

void AbstractWeapon::setPosition(const sf::Vector2f& pos, const sf::Vector2f& offset)
{
    AbstractDrawableObject::setPosition(pos + offset);
}

std::shared_ptr<AbstractWeapon> AbstractWeapon::create(Character* user, const std::string& name)
{
    if (name.length() > 5 && name.substr(0, 5) == "melee")
    {
        return std::make_shared<MeleeWeapon>(user, name);
    }
    else if (name != "Null" && !name.empty())
    {
        return std::make_shared<ShootingWeapon>(user, name);
    }

    return std::make_shared<NoWeapon>();
}

void AbstractWeapon::upgrade(const std::string& id)
{
    upgrades_.emplace_back(id);
}