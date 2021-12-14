//
// Created by jul3x on 03.11.19.
//

#include <R3E/system/Config.h>
#include <R3E/utils/Utils.h>

#include <common/ResourceManager.h>
#include <common/weapons/NoWeapon.h>

#include <common/characters/Player.h>
#include <common/weapons/ShootingWeapon.h>


Player::Player(const sf::Vector2f& position) :
        Character(position, CONF<std::string>("general/character")),
        is_alive_(true),
        side_stepping_freeze_time_(-1.0f),
        skill_points_(0),
        is_running_(false),
        running_fuel_(0.0f)
{
    skills_names_ = {{Skills::Intelligence, "Intelligence"},
                     {Skills::Heart, "Heart"},
                     {Skills::Strength, "Strength"},
                     {Skills::Agility, "Agility"}};

    if (CONF<bool>("no_clip_mode"))
    {
        this->changeCollisionArea(collision::None());
    }
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
                               utils::geo::polarToCartesian(RMGET<float>("characters", "henry", "side_step_speed"),
                                                            (this->getRotation() + static_cast<int>(dir) * 90.0f) *
                                                            M_PI / 180.0f),
                               CONF<float>("side_step_force_duration"));

        side_stepping_freeze_time_ = CONF<float>("side_stepping_freeze_time");
        return true;
    }

    return false;
}

bool Player::update(float time_elapsed)
{
    side_stepping_freeze_time_ -= time_elapsed;

    if (is_running_)
    {
        running_fuel_ -= time_elapsed;
        if (running_fuel_ <= 0.0f)
        {
            running_fuel_ = 0.0f;
            setRunning(false);
        }
    }
    else
    {
        running_fuel_ = std::min(running_fuel_ + getRunningFuelSpeed() * time_elapsed, getMaxRunningFuel());
    }

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

void Player::getShot(const Bullet& bullet, float factor)
{
    if (!CONF<bool>("god_mode"))
    {
        Character::getShot(bullet, factor);
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

void Player::getCut(const MeleeWeapon& weapon, float factor)
{
    if (!CONF<bool>("god_mode"))
    {
        static auto strength_skill_factor = CONF<float>("characters/strength_skill_factor");
        auto skill_factor = (strength_skill_factor - getSkill(Skills::Strength)) / strength_skill_factor;
        Character::getCut(weapon, factor * skill_factor);
    }
}

void
Player::addSpecialToBackpack(const std::string& id, int count, const std::function<void(Functional*)>& register_func)
{
    for (auto& item : backpack_)
    {
        if (item.first.getId() == id)
        {
            item.second++;
            return;
        }
    }

    backpack_.emplace_back(std::make_pair(Special({}, id), count));
    backpack_.back().first
             .setSize(CONF<float>("graphics/GLOBAL_ZOOM") * RMGET<sf::Vector2f>("specials", id, "size"));
    backpack_.back().first.changeOrigin(
            CONF<float>("graphics/GLOBAL_ZOOM") * RMGET<sf::Vector2f>("specials", id, "size") / 2.0f);
    backpack_.back().first.removeShadow();
    backpack_.back().first.lightOff();
    register_func(&backpack_.back().first);
}

void Player::addSkillPoints(int skill_points)
{
    skill_points_ += skill_points;
}

bool Player::addSkill(Player::Skills skill)
{
    static auto max_skills_count = CONF<int>("characters/max_skills_count");
    auto skills_count = getSkill(skill);
    if (skill_points_ > 0 && skills_count < max_skills_count)
    {
        skills_[skill] = ++skills_count;
        --skill_points_;
        return skill_points_ > 0;
    }

    return skill_points_ > 0;
}

void Player::upgradeWeapon(const std::string& weapon, const std::string& special)
{
    bool has_special = false;
    for (auto it = backpack_.begin(); it != backpack_.end(); ++it)
    {
        if (it->first.getId() == special && it->second > 0)
        {
            has_special = true;
            --(it->second);

            if (it->second <= 0)
            {
                backpack_.erase(it);
            }
            break;
        }
    }

    if (!has_special)
        return;

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
    return skills_.count(skill) ? skills_.at(skill) : 0;
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

void Player::changePlayerTexture(const std::string& name)
{
    this->setTextureName(name);
    this->changeTexture(&RM.getTexture("characters/" + name), true);
    this->setSize(RMGET<sf::Vector2f>("characters", name, "size"));
    static_shadow_->setSize(RMGET<sf::Vector2f>("characters", name, "size"));
}

float Player::getMaxHealth() const
{
    static auto factor = CONF<float>("characters/health_skill_factor");
    return max_life_ * static_cast<float>(factor + getSkill(Skills::Heart)) / factor;
}

float Player::getMaxTimeManipulation() const
{
    static auto factor = CONF<float>("characters/intelligence_skill_factor");
    return CONF<float>("characters/max_base_time_manipulation") *
           static_cast<float>(factor + getSkill(Skills::Intelligence)) / factor;
}

float Player::getTimeManipulationFuelSpeed() const
{
    static auto factor = CONF<float>("characters/intelligence_skill_factor");
    return CONF<float>("characters/time_manipulation_fuel_speed") *
           static_cast<float>(factor + getSkill(Skills::Intelligence)) / factor;
}

float Player::getMaxRunningFuel() const
{
    static auto factor = CONF<float>("characters/agility_skill_factor");
    return CONF<float>("characters/max_base_running_fuel") * static_cast<float>(factor + getSkill(Skills::Agility)) /
           factor;
}

float Player::getRunningFuelSpeed() const
{
    static auto factor = CONF<float>("characters/agility_skill_factor");
    return CONF<float>("characters/running_fuel_speed") * static_cast<float>(factor + getSkill(Skills::Agility)) /
           factor;
}

void Player::setRunning(bool run)
{
    if (!run)
    {
        is_running_ = false;
        return;
    }

    if (running_fuel_ > CONF<float>("running_min_time"))
    {
        is_running_ = true;
    }
}

bool Player::isRunning() const
{
    return is_running_;
}

float Player::getSpeedFactor() const
{
    static auto factor = CONF<float>("characters/agility_skill_factor");
    return Character::getSpeedFactor() * static_cast<float>(factor + getSkill(Skills::Agility)) / factor;
}

float Player::getRunningFuel() const
{
    return running_fuel_;
}

const std::string& Player::getName() const
{
    return name_;
}

void Player::makeLifeBar(const std::string& name)
{
    Character::makeLifeBar(name);
    name_ = name;
}

void Player::setName(const std::string& name)
{
    name_ = name;
}

void Player::setSkillPoints(int points)
{
    skill_points_ = points;
}

void Player::setSkill(const std::string& skill, int count)
{
    if (skill == "Intelligence")
        skills_[Skills::Intelligence] = count;
    else if (skill == "Heart")
        skills_[Skills::Heart] = count;
    else if (skill == "Strength")
        skills_[Skills::Strength] = count;
    else
        skills_[Skills::Agility] = count;
}

const j3x::List& Player::getSkills() const
{
    return j3x::convertToList<Player::Skills>(
        SKILLS, [this](j3x::List& ret, const auto& item) { 
            ret.emplace_back(j3x::List{this->getSkillName(item), this->getSkill(item)}); 
        });
}

const std::string& Player::getSkillName(Player::Skills skill) const
{
    return skills_names_.at(skill);
}

const j3x::List& Player::getBackpackToSerialize()
{
    return j3x::convertToList<std::pair<Special, int>>(
        backpack_, [](j3x::List& ret, const auto& item) { 
            ret.emplace_back(j3x::List{item.first.getId(), item.second}); 
        });
}

const j3x::List& Player::getWeaponsToSerialize()
{
    return j3x::convertToList<std::shared_ptr<AbstractWeapon>>(
        weapons_in_backpack_, [](j3x::List& ret, const auto& weapon) { 
            ret.emplace_back(j3x::List{
                weapon->getId(), weapon->getState(), 
                j3x::convertToList(weapon->getUpgrades())
            });
        });
}


