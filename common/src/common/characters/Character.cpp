//
// Created by jul3x on 10.03.20.
//

#include <R3E/utils/Geometry.h>
#include <R3E/utils/Numeric.h>
#include <R3E/utils/Misc.h>
#include <R3E/system/Config.h>

#include <common/ResourceManager.h>
#include <common/characters/Character.h>
#include <common/weapons/NoWeapon.h>
#include <common/weapons/ShootingWeapon.h>
#include <common/weapons/MeleeWeapon.h>


Character::Character(const sf::Vector2f& position, const std::string& id, int u_id) :
        Character(position, id,
                  RMGET<std::string>("characters", id, "default_activation"),
                  RMGET<j3x::List>("characters", id, "default_functions"),
                  RMGET<j3x::List>("characters", id, "default_datas"), u_id)
{

}

Character::Character(const sf::Vector2f& position, const std::string& id,
                     const std::string& activation, const j3x::List& functions,
                     const j3x::List& datas, int u_id) :
        Functional(activation, functions, datas, id, u_id),
        Shootable(RMGET<float>("characters", id, "max_health")),
        DynamicObject(position, {},
                      RMGET<sf::Vector2f>("characters", id, "size"),
                      collision::Box(RMGET<sf::Vector2f>("characters", id, "collision_size").x,
                                     RMGET<sf::Vector2f>("characters", id, "collision_size").y,
                                     RMGET<sf::Vector2f>("characters", id, "collision_offset")),
                      &RM.getTexture("characters/" + id),
                      RMGET<int>("characters", id, "z_index"),
                      RMGET<int>("characters", id, "frames_number"),
                      RMGET<float>("characters", id, "frame_duration"),
                      CONF<float>("characters/max_acceleration")),
        global_state_(GlobalState::Normal),
        max_life_(RMGET<float>("characters", id, "max_health")),
        ammo_state_(AmmoState::High),
        life_state_(LifeState::High),
        gun_offset_(j3x::getObj<sf::Vector2f>(
                RMGET<j3x::List>("characters", id, "gun_offset").front())),
        current_rotation_quarter_(1),
        rotate_to_(0.0f),
        current_special_object_(nullptr),
        current_talkable_character_(nullptr),
        should_respond_(false),
        is_moving_(false),
        talk_moment_(0),
        is_talkable_(RMGET<bool>("characters", id, "is_talkable")),
        previous_pos_(position), current_pos_(position)
{
    this->changeOrigin(RMGET<sf::Vector2f>("characters", id, "size") / 2.0f +
                       RMGET<sf::Vector2f>("characters", id, "map_offset"));
    this->setDefaultWeapons();

    if (is_talkable_)
    {
        talkable_area_ = std::make_unique<TalkableArea>(this, CONF<float>("characters/talkable_distance"));
    }

    this->makeLightPoint(this->getPosition(),
                         CONF<float>("graphics/characters_light_point_size") * CONF<float>("graphics/GLOBAL_ZOOM"),
                         &RM.getTexture("lightpoint"), RMGET<std::string>("characters", id, "light_point"));

    auto shadow_pos = this->getPosition();
    static_shadow_ = std::make_unique<graphics::TransformedTextureShadow>(
            shadow_pos, this->getSize(), CONF<float>("graphics/shadow_direction"),
            CONF<float>("graphics/shadow_length_factor"),
            &RM.getTexture("characters/" + id), sf::Color(CONF<int>("graphics/shadow_color")),
            z_index_,
            RMGET<int>("characters", id, "frames_number"),
            RMGET<float>("characters", id, "frame_duration"));

    if (RMGET<bool>("characters", id, "show_health_bar"))
        life_bar_ = std::make_unique<LifeBar>(utils::capitalFirst(this->getId()), this->getMaxHealth());

    texture_name_ = this->getId();
}

bool Character::shot()
{
    auto force = weapons_in_backpack_.at(current_weapon_)->use();

    if (!utils::num::isNearlyEqual(force, {0.0f, 0.0f}))
    {
        this->addSteeringForce(force, CONF<float>("shot_force_duration"));
        return true;
    }

    return false;
}

void Character::getShot(const Bullet& bullet, float factor)
{
    Shootable::getShot(bullet, factor);
    //Engine::spawnBloodAnimation();
    this->addSteeringForce(utils::geo::getNormalized(bullet.getVelocity()) *
                           static_cast<float>(bullet.getDeadlyFactor()) *
                           CONF<float>("get_shot_factor") * factor, CONF<float>("shot_force_duration"));

}

void Character::getCut(const MeleeWeapon& weapon, float factor)
{
    //Engine::spawnBloodAnimation();
    life_ -= weapon.getDeadlyFactor() * factor;
    life_ = life_ < 0 ? 0 : life_;
}

int Character::getCurrentWeapon() const
{
    return current_weapon_;
}

void Character::makeOnlyOneWeapon(const std::string& id, float state)
{
    weapons_in_backpack_.clear();
    weapons_in_backpack_.emplace_back(AbstractWeapon::create(this, id));
    current_weapon_ = 0;

    weapons_in_backpack_.at(current_weapon_)->setState(state);
}

bool Character::addWeaponToBackpack(const std::shared_ptr<AbstractWeapon>& ptr)
{
    // If weapon exists
    for (auto& weapon : weapons_in_backpack_)
    {
        if (weapon->getId() == ptr->getId())
        {
            weapon->setState(1.0f);
            return false;
        }
    }
    // If there are less than 4 weapons in backpack
    for (auto& weapon : weapons_in_backpack_)
    {
        if (weapon->getId().empty() || weapon->getId() == "null")
        {
            weapon = ptr;
            return true;
        }
    }

    weapons_in_backpack_.emplace_back(ptr);
    return true;
}

void Character::addAmmoToWeapon(const std::string& id)
{
    for (auto& weapon : weapons_in_backpack_)
    {
        if (weapon->getId() == id)
        {
            weapon->addAmmo(RMGET<int>("weapons", id, "ammo_portion"));

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
    if (life_bar_ != nullptr)
        life_bar_->setMaxHealth(health);
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
    auto new_weapon = static_cast<int>(current_weapon_ - relative_position_backpack);

    if (new_weapon < 0)
        new_weapon = weapons_in_backpack_.size() - 1;

    if (new_weapon >= weapons_in_backpack_.size())
        new_weapon = 0;

    current_weapon_ = new_weapon;
}

bool Character::update(float time_elapsed)
{
    bool is_alive = life_ > 0;
    previous_pos_ = current_pos_;
    current_pos_ = this->getPosition();
    DynamicObject::update(time_elapsed);

    if (life_bar_ != nullptr)
    {
        life_bar_->setPosition(this->getPosition());
        life_bar_->setHealth(this->getHealth());
        life_bar_->setMaxHealth(this->getMaxHealth());
        life_bar_->update(time_elapsed);
    }

    auto max_speed = RMGET<float>("characters", this->getId(), "max_speed");
    auto speed_factor = this->getSpeedFactor();
    auto vel = std::get<0>(utils::geo::cartesianToPolar(current_pos_ - previous_pos_)) / time_elapsed;

    if ((is_moving_ && vel > max_speed / 10.0f) ||
        (!is_moving_ && vel > max_speed / 2.0f))
    {
        is_moving_ = true;
        this->updateAnimation(time_elapsed, max_speed > 0.0f ? vel / max_speed : 1.0f);
    }
    else
    {
        is_moving_ = false;

        if (max_speed > 0.0f)
            this->setCurrentFrame(0);
        else
            this->updateAnimation(time_elapsed, 1.0f);
    }

    weapons_in_backpack_.at(current_weapon_)->update(time_elapsed);

    if (decorator_ != nullptr)
        decorator_->updateAnimation(time_elapsed);

    handleAmmoState();
    handleLifeState();
    handleGlobalState(time_elapsed);

    auto rotation_diff = utils::geo::getAngleBetweenDegree(this->getRotation(), rotate_to_);
    auto is_negative = std::signbit(rotation_diff);
    auto rotation_sqrt = std::sqrt(std::abs(rotation_diff)) * (is_negative ? -1.0f : 1.0f);
    auto new_rotation = this->getRotation() -
                        rotation_sqrt * CONF<float>("characters/mouse_reaction_speed") * speed_factor * time_elapsed;

    auto new_rotation_diff = utils::geo::getAngleBetweenDegree(new_rotation, rotate_to_);

    if (is_negative != std::signbit(new_rotation_diff))
        this->setRotation(rotate_to_);
    else
        this->setRotation(new_rotation);

    if (should_respond_)
    {
        talking_time_elapsed_ -= time_elapsed;

        if (talking_time_elapsed_ < 0)
        {
            talking_func_(this, j3x::getObj<std::string>(talk_scenario_, talk_moment_));
            ++talk_moment_;
            should_respond_ = false;
            talking_time_elapsed_ = 10.0f;
        }
    }

    speed_factor_with_time_.second -= time_elapsed;

    if (speed_factor_with_time_.second < 0.0f && speed_factor_with_time_.second > -10.0f)
    {
        this->setSpeedFactor(1.0f, -100.0f);
    }

    return is_alive;
}

void Character::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    bool show_body = true;
    if (!weapons_in_backpack_.empty())
    {
        auto melee = dynamic_cast<MeleeWeapon*>(weapons_in_backpack_.at(current_weapon_).get());
        if (melee != nullptr && melee->isUsed())
        {
            show_body = false;
        }
    }

    if (show_body)
        target.draw(*static_shadow_, states);

    if (current_rotation_quarter_ == 3 || current_rotation_quarter_ == 4)
    {
        if (!weapons_in_backpack_.empty())
            target.draw(*weapons_in_backpack_.at(current_weapon_), states);

        if (show_body)
            target.draw(shape_, states);

        if (decorator_ != nullptr)
            target.draw(*decorator_, states);
    }
    else
    {
        if (show_body)
            target.draw(shape_, states);

        if (decorator_ != nullptr)
            target.draw(*decorator_, states);

        if (!weapons_in_backpack_.empty())
            target.draw(*weapons_in_backpack_.at(current_weapon_), states);
    }

    if (life_bar_ != nullptr)
        target.draw(*life_bar_);
}

void Character::setRotation(float theta)
{
    if (!this->isVisible())
        return;

    auto get_quarter = [](float theta) {
        if (theta >= 0.0f && theta < 45.0f)
            return 1;
        else if (theta >= 45.0f && theta < 90.0f)
            return 11;
        else if (theta >= 90.0f && theta < 135.0f)
            return 2;
        else if (theta >= 135.0f && theta < 180.0f)
            return 21;
        else if (theta >= 180.0f && theta < 270.0f)
            return 3;
        else
            return 4;
    };
    theta = utils::geo::wrapAngle0_360(theta);
    short int new_quarter = get_quarter(theta);

    static std::string added_name;
    if (!is_moving_)
    {
        added_name = "_standing";
        this->setCurrentFrame(0);
    }
    else
    {
        added_name = "";
    }

    static std::string weapon_added_name;
    weapon_added_name = "";

    gun_offset_ = j3x::getObj<sf::Vector2f>(
            RMGET<j3x::List>("characters", this->getId(), "gun_offset"), current_frame_);
    switch (current_rotation_quarter_)
    {
        case 1:
        {
            weapon_added_name = "_back";

            weapons_in_backpack_.at(current_weapon_)->setFlipY(false);
            this->setFlipX(false);
            static_shadow_->setFlipX(false);

            if (new_quarter == 11 && theta >= 45.0f + CONF<float>("characters/rotating_hysteresis"))
                current_rotation_quarter_ = 11;
            else if (new_quarter == 4 && theta < 360.0f - CONF<float>("characters/rotating_hysteresis"))
                current_rotation_quarter_ = 4;
            else if (new_quarter != 11 && new_quarter != 4)
                current_rotation_quarter_ = new_quarter;
            break;
        }
        case 11:
        {
            weapon_added_name = "_back";

            weapons_in_backpack_.at(current_weapon_)->setFlipY(true);
            this->setFlipX(false);
            static_shadow_->setFlipX(false);

            if (new_quarter == 2 && theta >= 90.0f + CONF<float>("characters/rotating_hysteresis"))
                current_rotation_quarter_ = 2;
            else if (new_quarter == 1 && theta < 45.0f - CONF<float>("characters/rotating_hysteresis"))
                current_rotation_quarter_ = 1;
            else if (new_quarter != 2 && new_quarter != 1)
                current_rotation_quarter_ = new_quarter;
            break;
        }
        case 2:
        {
            this->setFlipX(true);
            static_shadow_->setFlipX(true);
            weapon_added_name = "_back";
            gun_offset_.x = -gun_offset_.x;

            weapons_in_backpack_.at(current_weapon_)->setFlipY(false);

            if (new_quarter == 11 && theta < 90.0f - CONF<float>("characters/rotating_hysteresis"))
                current_rotation_quarter_ = 11;
            else if (new_quarter == 21 && theta >= 135.0f + CONF<float>("characters/rotating_hysteresis"))
                current_rotation_quarter_ = 21;
            else if (new_quarter != 11 && new_quarter != 21)
                current_rotation_quarter_ = new_quarter;
            break;
        }
        case 21:
        {
            this->setFlipX(true);
            static_shadow_->setFlipX(true);
            weapon_added_name = "_back";
            gun_offset_.x = -gun_offset_.x;

            weapons_in_backpack_.at(current_weapon_)->setFlipY(true);

            if (new_quarter == 2 && theta < 135.0f - CONF<float>("characters/rotating_hysteresis"))
                current_rotation_quarter_ = 2;
            else if (new_quarter == 3 && theta >= 180.0f + CONF<float>("characters/rotating_hysteresis"))
                current_rotation_quarter_ = 3;
            else if (new_quarter != 2 && new_quarter != 3)
                current_rotation_quarter_ = new_quarter;
            break;
        }
        case 3:
        {
            added_name += "_4";
            this->setFlipX(true);
            static_shadow_->setFlipX(true);
            gun_offset_.x = -gun_offset_.x;

            weapons_in_backpack_.at(current_weapon_)->setFlipY(true);

            if (new_quarter == 4 && theta >= 270.0f + CONF<float>("characters/rotating_hysteresis"))
                current_rotation_quarter_ = 4;
            else if (new_quarter == 21 && theta < 180.0f - CONF<float>("characters/rotating_hysteresis"))
                current_rotation_quarter_ = 21;
            else if (new_quarter != 4 && new_quarter != 21)
                current_rotation_quarter_ = new_quarter;
            break;
        }
        case 4:
        {
            added_name += "_4";
            this->setFlipX(false);
            static_shadow_->setFlipX(false);
            weapons_in_backpack_.at(current_weapon_)->setFlipY(false);

            if (new_quarter == 3 && theta < 270.0f - CONF<float>("characters/rotating_hysteresis"))
                current_rotation_quarter_ = 3;
            else if (new_quarter == 1 && theta >= CONF<float>("characters/rotating_hysteresis"))
                current_rotation_quarter_ = 1;
            else if (new_quarter != 3 && new_quarter != 1)
                current_rotation_quarter_ = new_quarter;
            break;
        }
        default:
            throw std::runtime_error("[Character] Invalid rotation quarter!");
    }

    this->changeTexture(&RM.getTexture("characters/" + this->getTextureName() + added_name));

    auto& weapon_id = weapons_in_backpack_.at(current_weapon_)->getId();

    if (!weapon_id.empty())
        weapons_in_backpack_.at(current_weapon_)->changeTexture(
                &RM.getTexture("weapons/" + weapon_id + weapon_added_name));

    for (auto& weapon : weapons_in_backpack_)
        weapon->setRotation(theta);

    weapons_in_backpack_.at(current_weapon_)->setPosition(this->getPosition(), gun_offset_);
}

void Character::setPosition(const sf::Vector2f& pos)
{
    AbstractDrawableObject::setPosition(pos);
    weapons_in_backpack_.at(current_weapon_)->setPosition(pos, gun_offset_);

    if (decorator_ != nullptr)
        decorator_->setPosition(pos);

    if (talkable_area_ != nullptr)
        talkable_area_->setPosition(pos);

    if (light_ != nullptr)
        light_->setPosition(pos);

    static_shadow_->setPosition(pos - RMGET<sf::Vector2f>("characters", this->getId(), "map_offset"));
}

void Character::setPosition(float x, float y)
{
    this->setPosition({x, y});
}

void Character::setPositionX(float x)
{
    this->setPosition(x, this->getPosition().y);
}

void Character::setPositionY(float y)
{
    this->setPosition(this->getPosition().x, y);
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

void Character::setSpeedFactor(float factor, float time)
{
    speed_factor_with_time_.first = factor;
    speed_factor_with_time_.second = time;
    this->setAcceleration(speed_factor_with_time_.first * CONF<float>("characters/max_acceleration"));
}

float Character::getSpeedFactor() const
{
    return speed_factor_with_time_.second > 0.0f ? speed_factor_with_time_.first : 1.0f;
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
    if (life_ > 0.67f * getMaxHealth())
        life_state_ = LifeState::High;
    else if (life_ > 0.2f * getMaxHealth())
        life_state_ = LifeState::Low;
    else if (life_ > 0.0f)
        life_state_ = LifeState::Critical;
    else
        life_state_ = LifeState::Dead;
}

void Character::handleAmmoState()
{
    if (weapons_in_backpack_.at(current_weapon_)->getState() > 0.7)
        ammo_state_ = AmmoState::High;
    else if (weapons_in_backpack_.at(current_weapon_)->getState() > 0.0)
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
            life_ -= time_elapsed * CONF<float>("on_fire_hurt_speed");
            on_fire_time_ -= time_elapsed;

            if (on_fire_time_ <= 0.0f)
                setGlobalState(GlobalState::Normal);
            break;
    }
}

float Character::getRotation() const
{
    return weapons_in_backpack_.at(current_weapon_)->getRotation();
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
                    on_fire_time_ = CONF<float>("on_fire_time");
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
                    on_fire_time_ = CONF<float>("on_fire_time");
                    break;
            }
            break;
    }
}

bool Character::isTalkable() const
{
    return is_talkable_;
}

void Character::setCurrentTalkableCharacter(Character* obj)
{
    current_talkable_character_ = obj;
}

Character* Character::getCurrentTalkableCharacter() const
{
    return current_talkable_character_;
}

TalkableArea* Character::getTalkableArea() const
{
    return talkable_area_.get();
}

bool Character::talk(const std::function<void(Character*, const std::string&)>& talking_func, Character* character)
{
    if (!should_respond_)
    {
        if (talk_moment_ < talk_scenario_.size())
        {
            const auto& sentence = j3x::getObj<std::string>(talk_scenario_, talk_moment_);
            if (!sentence.empty())
                talking_func(character, sentence);

            talking_func_ = talking_func;
            ++talk_moment_;

            if (talk_moment_ < talk_scenario_.size())
                should_respond_ = true;

            if (!sentence.empty() || talk_moment_ > 1)
                talking_time_elapsed_ = CONF<float>("characters/talking_respond_time");
        }
    }
    return talk_moment_ < talk_scenario_.size();
}

const std::string& Character::getTalkScenarioStr() const
{
    static std::string result;
    result.clear();

    j3x::serialize(talk_scenario_, result);

    return result;
}

void Character::setTalkScenarioStr(const std::string& str)
{
    talk_scenario_ = j3x::parseObj<j3x::List>("list", str);
}

const j3x::List& Character::getTalkScenario() const
{
    return talk_scenario_;
}

void Character::setTalkScenario(const j3x::List& str)
{
    talk_scenario_ = str;
}

bool Character::updateAnimation(float time_elapsed, float animation_speed_factor)
{
    static_shadow_->updateAnimation(time_elapsed, animation_speed_factor);
    return AbstractDrawableObject::updateAnimation(time_elapsed, animation_speed_factor);
}

void Character::setCurrentFrame(short int frame)
{
    AbstractDrawableObject::setCurrentFrame(frame);
    static_shadow_->setCurrentFrame(frame);
}

void Character::changeTexture(sf::Texture* texture, bool reset)
{
    AbstractDrawableObject::changeTexture(texture, reset);
    static_shadow_->changeTexture(texture);
    static_shadow_->updateAnimation(0.0f);
}

graphics::StaticShadow* Character::getShadow() const
{
    return static_shadow_.get();
}

float Character::getRotateTo() const
{
    return rotate_to_;
}

const sf::Vector2f& Character::getGunOffset() const
{
    return gun_offset_;
}

void Character::setRotateTo(float theta)
{
    rotate_to_ = theta;
}

void Character::setCurrentWeapon(int number)
{
    current_weapon_ = std::max(0, static_cast<int>(number % weapons_in_backpack_.size()));
}

void Character::makeLifeBar(const std::string& name)
{
    life_bar_ = std::make_unique<LifeBar>(name, this->getMaxHealth());
}

const std::string& Character::getTextureName() const
{
    return texture_name_;
}

void Character::setTextureName(const std::string& texture_name)
{
    texture_name_ = texture_name;
}

void Character::clearWeapons()
{
    weapons_in_backpack_.clear();
    current_weapon_ = 0;
}

void Character::setWeapons(const std::vector<std::shared_ptr<AbstractWeapon>>& weapons)
{
    weapons_in_backpack_ = weapons;
}

void Character::setDefaultWeapons()
{
    clearWeapons();
    for (const auto& weapon : RMGET<j3x::List>("characters", this->getId(), "weapons"))
    {
        auto& weapon_str = j3x::getObj<std::string>(weapon);
        weapons_in_backpack_.emplace_back(AbstractWeapon::create(this, weapon_str));
    }
}

void Character::forceIsMoving(bool is_moving)
{
    is_moving_ = is_moving;
}

bool Character::isMoving() const
{
    return is_moving_;
}
