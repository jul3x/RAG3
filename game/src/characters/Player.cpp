//
// Created by jul3x on 03.11.19.
//

#include <R3E/system/Config.h>
#include <R3E/utils/Utils.h>

#include <common/ResourceManager.h>
#include <common/NoWeapon.h>

#include <characters/Player.h>
#include <common/ShootingWeapon.h>
#include <Game.h>


Player::Player(const sf::Vector2f& position) :
        Character(position, "player"),
        is_alive_(true),
        side_stepping_freeze_time_(-1.0f),
        skill_points_(0)
{
    if (CONF<bool>("no_clip_mode"))
    {
        this->changeCollisionArea(collision::None());
    }

    skills_.emplace(std::make_pair(Skills::Intelligence, 0));
    skills_.emplace(std::make_pair(Skills::Heart, 0));
    skills_.emplace(std::make_pair(Skills::Strength, 0));
    skills_.emplace(std::make_pair(Skills::Agility, 0));
}

void Player::setDead()
{
    is_alive_ = false;
}

bool Player::isAlive() const
{
    return is_alive_;
}

bool Player::sideStep(Player::SideStepDir dir)
{
    if (side_stepping_freeze_time_ <= 0.0f)
    {
        this->addSteeringForce(this->getSpeedFactor() *
                               utils::geo::polarToCartesian(RMGET<float>("characters", "player", "side_step_speed"),
                                       (this->getRotation() + static_cast<int>(dir) * 90.0f) * M_PI / 180.0f),
                               CONF<float>("side_step_force_duration"));

        side_stepping_freeze_time_ = CONF<float>("side_stepping_freeze_time");
        return true;
    }

    return false;
}

bool Player::update(float time_elapsed)
{
    side_stepping_freeze_time_ -= time_elapsed;
    bool alive = Character::update(time_elapsed);
    return alive;
}

void Player::setHealth(float life)
{
    if (!CONF<bool>("god_mode"))
    {
        Character::setHealth(life);
    }
}

void Player::getShot(const Bullet& bullet)
{
    if (!CONF<bool>("god_mode"))
    {
        Character::getShot(bullet);
    }
}

void Player::handleGlobalState(float time_elapsed)
{
    switch (global_state_)
    {
        case GlobalState::Normal:
            break;

        case GlobalState::OnFire:
            if (!CONF<bool>("god_mode"))
            {
                life_ -= time_elapsed * CONF<float>("on_fire_hurt_speed");
            }
            on_fire_time_ -= time_elapsed;

            if (on_fire_time_ <= 0.0f)
                setGlobalState(GlobalState::Normal);
            break;
    }
}

void Player::getCut(const MeleeWeapon& weapon)
{
    if (!CONF<int>("bool"))
    {
        Character::getCut(weapon);
    }
}

void Player::addSpecialToBackpack(Special* special)
{
    for (auto& item : backpack_)
    {
        if (item.first.getId() == special->getId())
        {
            item.second++;
            return;
        }
    }

    backpack_.emplace_back(std::make_pair(Special({}, special->getId()), 1));
    backpack_.back().first.setSize(2.0f * RMGET<sf::Vector2f>("specials", special->getId(), "size"));
    backpack_.back().first.changeOrigin(RMGET<sf::Vector2f>("specials", special->getId(), "size"));
    backpack_.back().first.removeShadow();
    Game::get().registerFunctions(&backpack_.back().first);
}

void Player::addSkillPoints(int skill_points)
{
    skill_points_ += skill_points;
}

bool Player::addSkill(Player::Skills skill)
{
    if (skill_points_ > 0)
    {
        skills_[skill] = skills_.at(skill) + 1;
        --skill_points_;
    }

    return skill_points_ > 0;
}

void Player::upgradeWeapon(const std::string& weapon, const std::string& special)
{
    bool has_special = false;
    for (auto it = backpack_.begin(); it != backpack_.end(); ++it)
    {
        std::cout << special << std::endl;
        if (it->first.getId() == special && it->second > 0)
        {
            has_special = true;
            --(it->second);

            if (it->second <= 0)
            {
                backpack_.erase(it);
                return;
            }
        }
    }

    if (!has_special) return;

    for (auto& obj : weapons_in_backpack_)
    {
        if (obj->getId() == weapon)
        {
            obj->upgrade(special);
        }
    }
}


int Player::getSkillPoints() const
{
    return skill_points_;
}

int Player::getSkill(Skills skill) const
{
    return skills_.at(skill);
}

void Player::useItem(const std::string& name)
{
    for (auto it = backpack_.begin(); it != backpack_.end(); ++it)
    {
        if (it->first.getId() == name)
        {
            it->first.use(this);
            --(it->second);

            if (it->second <= 0)
            {
                backpack_.erase(it);
                return;
            }
        }
    }
}

std::list<std::pair<Special, int>>& Player::getBackpack()
{
    return backpack_;
}