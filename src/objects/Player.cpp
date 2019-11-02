//
// Created by jprolejko on 30.09.19.
//

#include <iostream>

#include <utils/Geometry.h>
#include <utils/Numeric.h>
#include <system/ResourceManager.h>

#include <objects/Player.h>


Player::Player(const sf::Vector2f &position,
               const sf::Vector2f &velocity) :
        DynamicObject(position,
                      velocity,
                      {SIZE_X_, SIZE_Y_},
                      "player",
                      CFG.getFloat("player_max_acceleration")),
        life_(10) {}

void Player::initialize() {
    weapons_in_backpack_.emplace_back(ResourceManager::getInstance().getWeapon("m4"));
    weapons_in_backpack_.emplace_back(ResourceManager::getInstance().getWeapon("desert_eagle"));
    weapons_in_backpack_.emplace_back(ResourceManager::getInstance().getWeapon("shotgun"));
    weapons_in_backpack_.emplace_back(ResourceManager::getInstance().getWeapon("rocket_launcher"));

    current_weapon_ = weapons_in_backpack_.begin();
}

bool Player::shot() {
    auto new_velocity = current_weapon_->shot();

    if (!utils::isNearlyEqual(new_velocity, {0.0f, 0.0f}))
    {
        this->setForcedVelocity(new_velocity);
        return true;
    }

    return false;
}

void Player::getShot(const Bullet &bullet) {
    //Engine::spawnBloodAnimation();
    this->setForcedVelocity(this->getVelocity() +
        utils::getNormalized(bullet.getVelocity()) * static_cast<float>(bullet.getDeadlyFactor()) * 2.0f);
    
    life_ -= bullet.getDeadlyFactor();
}

void Player::switchWeapon(int relative_position_backpack) {
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

bool Player::update(float time_elapsed) {
    DynamicObject::update(time_elapsed);

    current_weapon_->setPosition(this->getPosition());
    current_weapon_->setRotation(this->getRotation());

    return life_ > 0;
}

void Player::draw(sf::RenderTarget &target, sf::RenderStates states) const {
    if (this->isVisible())
    {
        target.draw(shape_, states);
        target.draw(*current_weapon_, states);
    }
}