//
// Created by jul3x on 10.03.20.
//

#include <R3E/utils/Geometry.h>
#include <R3E/utils/Numeric.h>
#include <R3E/system/Config.h>

#include <common/ResourceManager.h>
#include <common/Character.h>
#include <common/NoWeapon.h>
#include <common/ShootingWeapon.h>


Character::Character(const sf::Vector2f& position, const std::string& id, int u_id) :
        Character(position, id,
                  utils::j3x::get<std::string>(RM.getObjectParams("characters", id), "default_activation"),
                  utils::j3x::get<std::vector<std::string>>(RM.getObjectParams("characters", id), "default_functions"),
                  utils::j3x::get<std::vector<std::string>>(RM.getObjectParams("characters", id), "default_datas"), u_id)
{

}

Character::Character(const sf::Vector2f& position, const std::string& id,
                     const std::string& activation, const std::vector<std::string>& functions,
                     const std::vector<std::string>& datas, int u_id) :
        Functional(activation, functions, datas, id, u_id),
        DynamicObject(position, {},
                      {utils::j3x::get<float>(RM.getObjectParams("characters", id), "size_x"),
                       utils::j3x::get<float>(RM.getObjectParams("characters", id), "size_y")},
                      Collision::Box({utils::j3x::get<float>(RM.getObjectParams("characters", id), "collision_size_x"),
                                      utils::j3x::get<float>(RM.getObjectParams("characters", id), "collision_size_y")}),
                      &RM.getTexture("characters/" + id),
                      utils::j3x::get<int>(RM.getObjectParams("characters", id), "z_index"),
                      utils::j3x::get<int>(RM.getObjectParams("characters", id), "frames_number"),
                      utils::j3x::get<float>(RM.getObjectParams("characters", id), "frame_duration"),
                      CFG.get<float>("characters/max_acceleration")),
        global_state_(GlobalState::Normal),
        max_life_(utils::j3x::get<float>(RM.getObjectParams("characters", id), "max_health")),
        ammo_state_(AmmoState::High),
        life_state_(LifeState::High),
        gun_offset_({utils::j3x::get<float>(RM.getObjectParams("characters", id), "gun_offset_x"),
                     utils::j3x::get<float>(RM.getObjectParams("characters", id), "gun_offset_y")}),
        current_rotation_quarter_(1),
        speed_factor_(1.0f),
        rotate_to_(0.0f),
        current_special_object_(nullptr),
        Shootable(utils::j3x::get<float>(RM.getObjectParams("characters", id), "max_health"))
{
    this->changeOrigin(sf::Vector2f(utils::j3x::get<float>(RM.getObjectParams("characters", id), "size_x"),
                                    utils::j3x::get<float>(RM.getObjectParams("characters", id), "size_y")) / 2.0f +
                       sf::Vector2f(utils::j3x::get<float>(RM.getObjectParams("characters", id), "map_offset_x"),
                                    utils::j3x::get<float>(RM.getObjectParams("characters", id), "map_offset_y")));

    for (const auto& weapon :
            utils::j3x::get<std::vector<std::string>>(RM.getObjectParams("characters", id), "weapons"))
    {
        if (weapon == "Null")
            weapons_in_backpack_.push_back(std::make_shared<NoWeapon>());
        else
            weapons_in_backpack_.push_back(std::make_shared<ShootingWeapon>(weapon));
    }

    current_weapon_ = weapons_in_backpack_.begin();
}

bool Character::shot()
{
    auto new_velocity = (*current_weapon_)->use();

    if (!utils::num::isNearlyEqual(new_velocity, {0.0f, 0.0f}))
    {
        this->setForcedVelocity(new_velocity);
        return true;
    }

    return false;
}

void Character::getShot(const Bullet& bullet)
{
    Shootable::getShot(bullet);
    //Engine::spawnBloodAnimation();
    this->setForcedVelocity(this->getVelocity() +
                            utils::geo::getNormalized(bullet.getVelocity()) *
                            static_cast<float>(bullet.getDeadlyFactor()) *
                            CFG.get<float>("get_shot_factor"));
}

int Character::getCurrentWeapon() const
{
    return static_cast<int>(std::distance<std::vector<std::shared_ptr<AbstractWeapon>>::const_iterator>(
            this->weapons_in_backpack_.begin(), this->current_weapon_));
}

void Character::makeOnlyOneWeapon(const std::string& id, float state)
{
    weapons_in_backpack_.clear();
    weapons_in_backpack_.emplace_back(std::make_shared<ShootingWeapon>(id));
    current_weapon_ = weapons_in_backpack_.begin();

    (*current_weapon_)->setState(state);
}

void Character::addWeaponToBackpack(const std::shared_ptr<AbstractWeapon>& ptr)
{
    // If weapon exists
    for (auto& weapon : weapons_in_backpack_)
    {
        if (weapon->getName() == ptr->getName())
        {
            weapon->setState(1.0f);
            return;
        }
    }

    // If there is less than 4 weapons in backpack
    for (auto &weapon : weapons_in_backpack_)
    {
        if (weapon->getName().empty())
        {
            weapon = ptr;
            return;
        }
    }

    weapons_in_backpack_.emplace_back(ptr);
}

void Character::addAmmoToWeapon(const std::string& id)
{
    for (auto& weapon : weapons_in_backpack_)
    {
        if (weapon->getName() == id)
        {
            weapon->setState(std::min(1.0f, weapon->getState() +
                                            static_cast<float>(utils::j3x::get<int>(RM.getObjectParams("weapons", id), "ammo_portion")) /
                                            static_cast<float>(utils::j3x::get<int>(RM.getObjectParams("weapons", id), "max_ammo"))));

            return;
        }
    }
}

const std::vector<std::shared_ptr<AbstractWeapon>>& Character::getWeapons() const
{
    return this->weapons_in_backpack_;
}

void Character::setMaxHealth(float health)
{
    max_life_ = health;
}

float Character::getMaxHealth() const
{
    return this->max_life_;
}

Character::LifeState Character::getLifeState() const
{
    return this->life_state_;
}


void Character::switchWeapon(int relative_position_backpack)
{
    current_weapon_ = current_weapon_ + relative_position_backpack;

    if (current_weapon_ >= weapons_in_backpack_.end())
    {
        current_weapon_ = weapons_in_backpack_.begin();
    }

    if (current_weapon_ < weapons_in_backpack_.begin())
    {
        current_weapon_ = weapons_in_backpack_.end() - 1;
    }
}

bool Character::update(float time_elapsed)
{
    bool is_alive = life_ > 0;
    DynamicObject::update(time_elapsed);
    (*current_weapon_)->update(time_elapsed);

    if (decorator_ != nullptr)
        decorator_->updateAnimation(time_elapsed);

    auto vel = std::get<0>(utils::geo::cartesianToPolar(this->getVelocity()));
    this->updateAnimation(time_elapsed,
                          vel / utils::j3x::get<float>(RM.getObjectParams("characters", this->getId()), "max_speed"));

    handleAmmoState();
    handleLifeState();
    handleGlobalState(time_elapsed);

    auto rotation_diff = utils::geo::getAngleBetweenDegree(this->getRotation(), rotate_to_);
    auto is_negative = std::signbit(rotation_diff);
    auto rotation_sqrt = std::sqrt(std::abs(rotation_diff)) * (is_negative ? -1.0f : 1.0f);
    this->setRotation(this->getRotation() -
                      rotation_sqrt * CFG.get<float>("characters/mouse_reaction_speed") * speed_factor_ * time_elapsed);

    return is_alive;
}

void Character::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(shape_, states);

    if (decorator_ != nullptr)
        target.draw(*decorator_, states);

    if (!weapons_in_backpack_.empty())
        target.draw(**current_weapon_, states);
}

void Character::setRotation(float theta)
{
    (*current_weapon_)->setRotation(theta);

    auto getQuarter = [](float theta) {
        if (theta >= 0.0f && theta < 90.0f)
            return 1;
        else if (theta >= 90.0f && theta < 180.0f)
            return 2;
        else if (theta >= 180.0f && theta < 270.0f)
            return 3;
        else
            return 4;
    };

    short int new_quarter_ = getQuarter(theta);

    switch (current_rotation_quarter_)
    {
        case 1:
        {
            shape_.setTexture(&RM.getTexture("characters/" + this->getId()));

            gun_offset_.x = utils::j3x::get<float>(RM.getObjectParams("characters", this->getId()), "gun_offset_x");
            gun_offset_.y = utils::j3x::get<float>(RM.getObjectParams("characters", this->getId()), "gun_offset_y");

            if (new_quarter_ == 2 && theta >= 90.0f + Character::ROTATING_HYSTERESIS_)
                current_rotation_quarter_ = 2;
            else if (new_quarter_ != 2)
                current_rotation_quarter_ = new_quarter_;
            break;
        }
        case 2:
        {
            shape_.setTexture(&RM.getTexture("characters/" + this->getId() + "_2"));
            gun_offset_.x = -utils::j3x::get<float>(RM.getObjectParams("characters", this->getId()), "gun_offset_x");
            gun_offset_.y = utils::j3x::get<float>(RM.getObjectParams("characters", this->getId()), "gun_offset_y");

            if (new_quarter_ == 1 && theta < 90.0f - Character::ROTATING_HYSTERESIS_)
                current_rotation_quarter_ = 1;
            else if (new_quarter_ != 1)
                current_rotation_quarter_ = new_quarter_;
            break;
        }
        case 3:
        {
            shape_.setTexture(&RM.getTexture("characters/" + this->getId() + "_3"));
            gun_offset_.x = -utils::j3x::get<float>(RM.getObjectParams("characters", this->getId()), "gun_offset_x");
            gun_offset_.y = utils::j3x::get<float>(RM.getObjectParams("characters", this->getId()), "gun_offset_y");

            if (new_quarter_ == 4 && theta >= 270.0f + Character::ROTATING_HYSTERESIS_)
                current_rotation_quarter_ = 4;
            else if (new_quarter_ != 4)
                current_rotation_quarter_ = new_quarter_;
            break;
        }
        case 4:
        {
            shape_.setTexture(&RM.getTexture("characters/" + this->getId() + "_4"));
            gun_offset_.x = utils::j3x::get<float>(RM.getObjectParams("characters", this->getId()), "gun_offset_x");
            gun_offset_.y = utils::j3x::get<float>(RM.getObjectParams("characters", this->getId()), "gun_offset_y");

            if (new_quarter_ == 3 && theta < 270.0f - Character::ROTATING_HYSTERESIS_)
                current_rotation_quarter_ = 3;
            else if (new_quarter_ != 3)
                current_rotation_quarter_ = new_quarter_;
            break;
        }
        default:
            throw std::runtime_error("[Character] Invalid rotation quarter!");
    }
}

void Character::setPosition(const sf::Vector2f& pos)
{
    AbstractDrawableObject::setPosition(pos);
    (*current_weapon_)->setPosition(pos + sf::Vector2f{gun_offset_.x, gun_offset_.y});

    if (decorator_ != nullptr)
        decorator_->setPosition(pos);
}

void Character::setPosition(float x, float y)
{
    AbstractDrawableObject::setPosition(x, y);
    (*current_weapon_)->setPosition(x + gun_offset_.x, y + gun_offset_.y);

    if (decorator_ != nullptr)
        decorator_->setPosition(x, y);
}

void Character::setPositionX(float x)
{
    AbstractDrawableObject::setPositionX(x);
    (*current_weapon_)->setPositionX(x + gun_offset_.x);

    if (decorator_ != nullptr)
        decorator_->setPositionX(x);
}

void Character::setPositionY(float y)
{
    AbstractDrawableObject::setPositionY(y);
    (*current_weapon_)->setPositionY(y + gun_offset_.y);

    if (decorator_ != nullptr)
        decorator_->setPositionY(y);
}

void Character::setWeaponPointing(const sf::Vector2f& point)
{
    float angle = std::atan2(point.y - this->getPosition().y - gun_offset_.y,
                             point.x - this->getPosition().x - gun_offset_.x);

    rotate_to_ = angle * 180.0f / static_cast<float>(M_PI);
}

bool Character::isAlreadyRotated() const
{
    static constexpr float ERROR = 2.0f;

    return utils::num::isNearlyEqual(utils::geo::getAngleBetweenDegree(this->getRotation(), rotate_to_), 0.0f, ERROR);
}

void Character::setSpeedFactor(float factor)
{
    speed_factor_ = factor;
    this->setAcceleration(speed_factor_ * CFG.get<float>("characters/max_acceleration"));
}

float Character::getSpeedFactor() const
{
    return speed_factor_;
}

void Character::setCurrentSpecialObject(Special* obj)
{
    current_special_object_ = obj;
}

Special* Character::getCurrentSpecialObject() const
{
    return current_special_object_;
}

void Character::handleLifeState()
{
    if (life_ > 0.67f * max_life_)
        life_state_ = LifeState::High;
    else if (life_ > 0.2f * max_life_)
        life_state_ = LifeState::Low;
    else if (life_ > 0.0f)
        life_state_ = LifeState::Critical;
    else
        life_state_ = LifeState::Dead;
}

void Character::handleAmmoState()
{
    if ((*current_weapon_)->getState() > 0.7)
        ammo_state_ = AmmoState::High;
    else if ((*current_weapon_)->getState() > 0.0)
        ammo_state_ = AmmoState::Low;
    else
        ammo_state_ = AmmoState::Zero;
}

void Character::handleGlobalState(float time_elapsed)
{
    switch (global_state_)
    {
        case GlobalState::Normal:
            break;

        case GlobalState::OnFire:
            life_ -= time_elapsed * CFG.get<float>("on_fire_hurt_speed");
            on_fire_time_ -= time_elapsed;

            if (on_fire_time_ <= 0.0f)
                setGlobalState(GlobalState::Normal);
            break;
    }
}

float Character::getRotation() const
{
    return (*current_weapon_)->getRotation();
}

Character::GlobalState Character::getGlobalState() const
{
    return global_state_;
}

void Character::setGlobalState(Character::GlobalState state)
{
    switch (global_state_)
    {
        case GlobalState::Normal:
            switch (state)
            {
                case GlobalState::Normal:
                    break;
                case GlobalState::OnFire:
                    decorator_ = std::make_unique<Decoration>(this->getPosition(), "character_on_flames");
                    global_state_ = state;
                    on_fire_time_ = CFG.get<float>("on_fire_time");
                    break;
            }

            break;

        case GlobalState::OnFire:
            switch (state)
            {
                case GlobalState::Normal:
                    decorator_.reset(nullptr);
                    global_state_ = state;
                    break;
                case GlobalState::OnFire:
                    on_fire_time_ = CFG.get<float>("on_fire_time");
                    break;
            }
            break;
    }
}
