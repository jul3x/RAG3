//
// Created by jul3x on 10.03.20.
//

#include <R3E/utils/Geometry.h>
#include <R3E/utils/Numeric.h>
#include <R3E/system/Config.h>

#include <common/ResourceManager.h>
#include <common/Character.h>
#include <common/NoWeapon.h>


Character::Character(const sf::Vector2f& position, const std::string& id) :
        DynamicObject(position, {},
                      {utils::getFloat(RM.getObjectParams("characters", id), "size_x"),
                       utils::getFloat(RM.getObjectParams("characters", id), "size_y")},
                      Collision::Box({utils::getFloat(RM.getObjectParams("characters", id), "collision_size_x"),
                                      utils::getFloat(RM.getObjectParams("characters", id), "collision_size_y")}),
                      &RM.getTexture("characters/" + id),
                      utils::getInt(RM.getObjectParams("characters", id), "frames_number"),
                      utils::getFloat(RM.getObjectParams("characters", id), "frame_duration"),
                      sf::Color::Transparent,
                      CFG.getFloat("characters/max_acceleration")),
        max_life_(utils::getInt(RM.getObjectParams("characters", id), "max_health")),
        ammo_state_(AmmoState::High),
        life_state_(LifeState::High),
        gun_offset_({utils::getFloat(RM.getObjectParams("characters", id), "gun_offset_x"),
                     utils::getFloat(RM.getObjectParams("characters", id), "gun_offset_y")}),
        current_rotation_quarter_(1),
        Shootable(utils::getInt(RM.getObjectParams("characters", id), "max_health")),
        Identifiable(id)
{
    this->changeOrigin(sf::Vector2f(utils::getFloat(RM.getObjectParams("characters", id), "size_x"),
                                    utils::getFloat(RM.getObjectParams("characters", id), "size_y")) / 2.0f +
                       sf::Vector2f(utils::getFloat(RM.getObjectParams("characters", id), "map_offset_x"),
                                    utils::getFloat(RM.getObjectParams("characters", id), "map_offset_y")));
}

bool Character::shot(float time_factor)
{
    auto new_velocity = (*current_weapon_)->use(time_factor);

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
                            CFG.getFloat("get_shot_factor"));
}

int Character::getCurrentWeapon() const
{
    return static_cast<int>(std::distance<std::vector<std::shared_ptr<AbstractWeapon>>::const_iterator>(
            this->weapons_in_backpack_.begin(), this->current_weapon_));
}

const std::vector<std::shared_ptr<AbstractWeapon>>& Character::getWeapons() const
{
    return this->weapons_in_backpack_;
}

int Character::getHealth() const
{
    return this->life_;
}

int Character::getMaxHealth() const
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
    DynamicObject::update(time_elapsed);

    auto vel = std::get<0>(utils::geo::cartesianToPolar(this->getVelocity()));
    this->updateAnimation(time_elapsed,
                          vel / utils::getFloat(RM.getObjectParams("characters", this->getId()), "max_speed"));

    handleAmmoState();
    handleLifeState();

    auto rotation_diff = utils::geo::getAngleBetweenDegree(this->getRotation(), rotate_to_);
    auto is_negative = std::signbit(rotation_diff);
    auto rotation_sqrt = std::sqrt(std::abs(rotation_diff)) * (is_negative ? -1.0f : 1.0f);
    this->setRotation(this->getRotation() -
                      rotation_sqrt * CFG.getFloat("characters/mouse_reaction_speed") * time_elapsed);

    return life_ > 0;
}

void Character::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(shape_, states);

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

            gun_offset_.x = utils::getFloat(RM.getObjectParams("characters", this->getId()), "gun_offset_x");
            gun_offset_.y = utils::getFloat(RM.getObjectParams("characters", this->getId()), "gun_offset_y");

            if (new_quarter_ == 2 && theta >= 90.0f + Character::ROTATING_HYSTERESIS_)
                current_rotation_quarter_ = 2;
            else if (new_quarter_ != 2)
                current_rotation_quarter_ = new_quarter_;
            break;
        }
        case 2:
        {
            shape_.setTexture(&RM.getTexture("characters/" + this->getId() + "_2"));
            gun_offset_.x = -utils::getFloat(RM.getObjectParams("characters", this->getId()), "gun_offset_x");
            gun_offset_.y = utils::getFloat(RM.getObjectParams("characters", this->getId()), "gun_offset_y");

            if (new_quarter_ == 1 && theta < 90.0f - Character::ROTATING_HYSTERESIS_)
                current_rotation_quarter_ = 1;
            else if (new_quarter_ != 1)
                current_rotation_quarter_ = new_quarter_;
            break;
        }
        case 3:
        {
            shape_.setTexture(&RM.getTexture("characters/" + this->getId() + "_3"));
            gun_offset_.x = -utils::getFloat(RM.getObjectParams("characters", this->getId()), "gun_offset_x");
            gun_offset_.y = utils::getFloat(RM.getObjectParams("characters", this->getId()), "gun_offset_y");

            if (new_quarter_ == 4 && theta >= 270.0f + Character::ROTATING_HYSTERESIS_)
                current_rotation_quarter_ = 4;
            else if (new_quarter_ != 4)
                current_rotation_quarter_ = new_quarter_;
            break;
        }
        case 4:
        {
            shape_.setTexture(&RM.getTexture("characters/" + this->getId() + "_4"));
            gun_offset_.x = utils::getFloat(RM.getObjectParams("characters", this->getId()), "gun_offset_x");
            gun_offset_.y = utils::getFloat(RM.getObjectParams("characters", this->getId()), "gun_offset_y");

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
}

void Character::setPosition(float x, float y)
{
    AbstractDrawableObject::setPosition(x, y);
    (*current_weapon_)->setPosition(x + gun_offset_.x, y + gun_offset_.y);
}

void Character::setPositionX(float x)
{
    AbstractDrawableObject::setPositionX(x);
    (*current_weapon_)->setPositionX(x + gun_offset_.x);
}

void Character::setPositionY(float y)
{
    AbstractDrawableObject::setPositionY(y);
    (*current_weapon_)->setPositionY(y + gun_offset_.y);
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

float Character::getRotation() const
{
    return (*current_weapon_)->getRotation();
}