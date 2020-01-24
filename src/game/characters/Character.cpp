//
// Created by jul3x on 30.09.19.
//

#include <iostream>

#include <engine/utils/Geometry.h>
#include <engine/utils/Numeric.h>
#include <game/misc/ResourceManager.h>

#include <game/characters/Character.h>
#include <SFML/Graphics/CircleShape.hpp>


Character::Character(const sf::Vector2f& position,
                     const sf::Vector2f& velocity,
                     int max_life) :
        DynamicObject(position,
                      velocity,
                      {SIZE_X_, SIZE_Y_},
                      //Collision::Circle((SIZE_X_ - 40.0f) / 2.0f),
                      Collision::Box(60, 100, {20, 20}),
                      &ResourceManager::getInstance().getTexture("player"),
                      sf::Color(CFG.getInt("trail_color")),
                      CFG.getFloat("player_max_acceleration")),
        max_life_(max_life),
        Shootable(max_life) {}

bool Character::shot()
{
    auto new_velocity = (*current_weapon_)->use();

    if (!utils::isNearlyEqual(new_velocity, {0.0f, 0.0f}))
    {
        this->setForcedVelocity(new_velocity);
        return true;
    }

    return false;
}

void Character::getShot(const Bullet& bullet)
{
    //Engine::spawnBloodAnimation();
    this->setForcedVelocity(this->getVelocity() +
                            utils::getNormalized(bullet.getVelocity()) * static_cast<float>(bullet.getDeadlyFactor()) *
                            CFG.getFloat("get_shot_factor"));

    life_ -= bullet.getDeadlyFactor();
    life_ = life_ < 0 ? 0 : life_;
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

    return life_ > 0;
}

void Character::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    static sf::RectangleShape rect;
    rect.setPosition(this->getPosition() + this->getCollisionArea().getOffset());
    rect.setSize(sf::Vector2f(this->getCollisionArea().getA(),this->getCollisionArea().getB()));
    rect.setOrigin(this->getCollisionArea().getA() / 2.0f, this->getCollisionArea().getB() / 2.0f);
    rect.setFillColor(sf::Color::Red);
    target.draw(rect, states);
    if (this->isVisible())
    {
        target.draw(shape_, states);
        target.draw(**current_weapon_, states);
    }
}

void Character::setPosition(const sf::Vector2f& pos)
{
    AbstractDrawableObject::setPosition(pos);
    (*current_weapon_)->setPosition(pos);
}

void Character::setRotation(float theta)
{
    AbstractDrawableObject::setRotation(theta);
    (*current_weapon_)->setRotation(theta);
}

void Character::setPosition(float x, float y)
{
    AbstractDrawableObject::setPosition(x, y);
    (*current_weapon_)->setPosition(x, y);
}
