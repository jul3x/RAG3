//
// Created by jul3x on 08.05.20.
//

#include <common/ResourceManager.h>

#include <common/PlacedWeapon.h>


PlacedWeapon::PlacedWeapon(const sf::Vector2f& position, float direction, const std::string& id, int u_id) :
    PlacedWeapon(position, direction, id,
                 utils::j3x::get<std::string>(RM.getObjectParams("weapons", id), "default_usage"),
                 utils::j3x::get<float>(RM.getObjectParams("weapons", id), "default_data"), u_id)
{

}

PlacedWeapon::PlacedWeapon(const sf::Vector2f& position, float direction, const std::string& id,
                           const std::string& usage, float data, int u_id) :
    ShootingWeapon(id),
    Unique(u_id),
    data_(data),
    usage_str_(usage)
{
    // TODO Change origin?

    this->setPosition(position);
    this->setRotation(direction);

    if (usage == "Const")
    {
        usage_func_ = std::bind(&PlacedWeapon::constantUse, this, std::placeholders::_1);
    }
    else
    {
        throw std::invalid_argument("[PlacedWeapon] Invalid usage function: " + usage);
    }
}

void PlacedWeapon::update(float time_elapsed)
{
    this->setState(1.0f); // never ending ammo
    AbstractWeapon::update(time_elapsed);

    usage_func_(time_elapsed);
}

void PlacedWeapon::constantUse(float time_elapsed)
{
    use();
}

float PlacedWeapon::getData() const
{
    return data_;
}

const std::string& PlacedWeapon::getUsageStr() const
{
    return usage_str_;
}

void PlacedWeapon::setUsageStr(const std::string& usage)
{
    usage_str_ = usage;
}

void PlacedWeapon::setData(float data)
{
    data_ = data;
}

