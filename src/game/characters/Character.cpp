//
// Created by jul3x on 30.09.19.
//

#include <iostream>

#include <engine/utils/Geometry.h>
#include <engine/utils/Numeric.h>
#include <game/misc/ResourceManager.h>

#include <game/characters/Character.h>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Text.hpp>


Character::Character(const sf::Vector2f& position,
                     const sf::Vector2f& velocity,
                     int max_life) :
        DynamicObject(position,
                      velocity,
                      {SIZE_X_, SIZE_Y_},
                      Collision::Box(COLLISION_SIZE_X_, COLLISION_SIZE_Y_, {0.0f, COLLISION_OFFSET_Y_}),
                      &RM.getTexture("player_1"),
                      sf::Color(CFG.getInt("trail_color")),
                      CFG.getFloat("max_acceleration")),
        max_life_(max_life),
        ammo_state_(AmmoState::High),
        life_state_(LifeState::High),
        path_(nullptr),
        gun_offset_({CFG.getFloat("gun_offset_x"), CFG.getFloat("gun_offset_y")}),
        Shootable(max_life) {}

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
                            CFG.getFloat("get_shot_factor"));
}

int Character::getCurrentWeapon() const
{
    return static_cast<int>(std::distance<std::vector<std::unique_ptr<AbstractWeapon>>::const_iterator>(
            this->weapons_in_backpack_.begin(), this->current_weapon_));
}

const std::vector<std::unique_ptr<AbstractWeapon>>& Character::getWeapons() const
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

    handleAmmoState();
    handleLifeState();

    auto rotation_diff = utils::geo::getAngleBetweenDegree(this->getRotation(), rotate_to_);
    auto is_negative = std::signbit(rotation_diff);
    auto rotation_sqrt = std::sqrt(std::abs(rotation_diff)) * (is_negative ? -1.0f : 1.0f);
    this->setRotation(this->getRotation() -
                      rotation_sqrt * CFG.getFloat("mouse_reaction_speed") * time_elapsed);

    return life_ > 0;
}

void Character::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(shape_, states);
    target.draw(**current_weapon_, states);

    static sf::VertexArray path(sf::LineStrip);
    path.clear();

    if (path_ != nullptr && !path_->empty())
    {
        for (const auto& v : *path_)
        {
            path.append(sf::Vertex{v.first, sf::Color::Red});
        }
    }

    target.draw(path, states);
}

void Character::setRotation(float theta)
{
    (*current_weapon_)->setRotation(theta);

    if (theta >= 0.0f && theta < 90.0f)
    {
        shape_.setTexture(&RM.getTexture("player_1"));
        gun_offset_.x = CFG.getFloat("gun_offset_x");
        gun_offset_.y = CFG.getFloat("gun_offset_y");
    }
    else if (theta >= 90.0f && theta < 180.0f)
    {
        shape_.setTexture(&RM.getTexture("player_2"));
        gun_offset_.x = -CFG.getFloat("gun_offset_x");
        gun_offset_.y = CFG.getFloat("gun_offset_y");
    }
    else if (theta >= 180.0f && theta < 270.0f)
    {
        shape_.setTexture(&RM.getTexture("player_3"));
        gun_offset_.x = -CFG.getFloat("gun_offset_x");
        gun_offset_.y = CFG.getFloat("gun_offset_y");
    }
    else
    {
        shape_.setTexture(&RM.getTexture("player_4"));
        gun_offset_.x = CFG.getFloat("gun_offset_x");
        gun_offset_.y = CFG.getFloat("gun_offset_y");
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
