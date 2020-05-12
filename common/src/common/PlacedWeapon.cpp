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
    ShootingWeapon(nullptr, id),
    Unique(u_id),
    data_(data),
    usage_str_(usage),
    active_(false),
    time_elapsed_(0.0f)
{
    // TODO Change origin?

    this->setPosition(position);
    this->setRotation(direction);

    if (usage == "Const")
    {
        usage_func_ = std::bind(&PlacedWeapon::constUse, this, std::placeholders::_1);
    }
    else if (usage == "Single")
    {
        usage_func_ = std::bind(&PlacedWeapon::singleUse, this, std::placeholders::_1);
    }
    else if (usage == "Interrupted")
    {
        usage_func_ = std::bind(&PlacedWeapon::interruptedUse, this, std::placeholders::_1);
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

    if (active_)
        usage_func_(time_elapsed);
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

void PlacedWeapon::setActive(bool active)
{
    active_ = active;
}

bool PlacedWeapon::getActive() const
{
    return active_;
}

void PlacedWeapon::constUse(float time_elapsed)
{
    use();
}

void PlacedWeapon::singleUse(float time_elapsed)
{
    time_elapsed_ += time_elapsed;

    if (time_elapsed_ >= data_)
        time_elapsed_ = data_;
    else
        use();
}

void PlacedWeapon::interruptedUse(float time_elapsed)
{
    time_elapsed_ += time_elapsed;

    if (time_elapsed_ >= 2 * data_)
        time_elapsed_ = 0.0f;
    else if (time_elapsed_ >= data_)
    {
        // nothing
    }
    else
        use();
}

