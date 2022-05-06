//
// Created by jul3x on 08.05.20.
//

#include <common/ResourceManager.h>

#include <common/objects/PlacedWeapon.h>


PlacedWeapon::PlacedWeapon(const sf::Vector2f& position, float direction, const std::string& id, int u_id) :
        PlacedWeapon(position, direction, id,
                     RMGET<std::string>("weapons", id, "default_usage"),
                     RMGET<float>("weapons", id, "default_data"), u_id)
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

    this->setPosition(position, {});
    this->setRotation(direction);

    auto function_index = std::find(ACTIVATIONS.begin(), ACTIVATIONS.end(), usage);

    if (function_index != ACTIVATIONS.end())
    {
        usage_func_ = std::bind(FUNCTIONS.at(std::distance(ACTIVATIONS.begin(), function_index)), this,
                                std::placeholders::_1);

        if (utils::startsWith(usage, "Near"))
        {
            activation_area_ =
                    std::make_unique<ActivationArea>(this, RMGET<float>("weapons", id, "activation_area_radius"));
            activation_area_->setPosition(this->getPosition());
        }
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

    if (activation_area_ != nullptr)
    {
        activation_area_->setActive(false);
        active_ = false;
    }
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
    if (active_)
        use();
}

void PlacedWeapon::singleUse(float time_elapsed)
{
    if (active_)
    {
        time_elapsed_ += time_elapsed;

        if (time_elapsed_ >= data_)
            time_elapsed_ = data_;
        else
            use();
    }
}

void PlacedWeapon::interruptedUse(float time_elapsed)
{
    if (active_)
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
}

void PlacedWeapon::nearConstUse(float time_elapsed)
{
    if (activation_area_->isActive())
        constUse(time_elapsed);
}

void PlacedWeapon::nearSingleUse(float time_elapsed)
{
    if (activation_area_->isActive())
        singleUse(time_elapsed);
    else
        time_elapsed_ = 0.0f;
}

void PlacedWeapon::nearInterruptedUse(float time_elapsed)
{
    if (activation_area_->isActive())
        interruptedUse(time_elapsed);
    else
        time_elapsed_ = 0.0f;
}

ActivationArea* PlacedWeapon::getActivationArea() const
{
    return activation_area_.get();
}

