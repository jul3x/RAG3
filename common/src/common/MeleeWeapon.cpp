
//
// Created by jul3x on 12.05.20.
//

#include <R3E/system/Config.h>

#include <common/ResourceManager.h>
#include <common/MeleeWeapon.h>

#include <utility>


MeleeWeapon::MeleeWeapon(Character* user, const std::string& id) :
        AbstractWeapon(user, RMGET<sf::Vector2f>("weapons", id, "size"),
                       RMGET<sf::Vector2f>("weapons", id, "offset"),
                       id),
        reversed_recoil_(RMGET<float>("weapons", id, "recoil")),
        use_timeout_(RMGET<float>("weapons", id, "spawn_timeout")),
        deadly_factor_(RMGET<float>("weapons", id, "deadly_factor")),
        use_elapsed_(0.0f),
        saved_rotation_(0.0f),
        is_used_(false),
        spawned_area_(false)
{
    this->changeOrigin(sf::Vector2f(0.0f, RMGET<sf::Vector2f>("weapons", id, "size").y) / 2.0f +
                       RMGET<sf::Vector2f>("weapons", id, "offset"));

    area_ = std::make_unique<MeleeWeaponArea>(this, CONF<float>("melee_weapon_range"));

    auto shadow_pos = this->getPosition();
    static_shadow_ = std::make_unique<graphics::TransformedTextureShadow>(
            shadow_pos, RMGET<sf::Vector2f>("weapons", this->getId(), "use_size"),
            CONF<float>("graphics/shadow_direction"),
            CONF<float>("graphics/shadow_length_factor"),
            &RM.getTexture("weapons/" + user_->getId() + "_" + this->getId()), sf::Color(CONF<int>("graphics/shadow_color")),
            z_index_,
            RMGET<int>("weapons", id, "frames_number"),
            RMGET<float>("weapons", id, "frame_duration"));
}

sf::Vector2f MeleeWeapon::use()
{
    if (time_elapsed_ < 0.0f)
    {
        auto texture_name = "weapons/" + user_->getId() + "_" + this->getId();
        bool flipped = saved_rotation_ > 90.0f && saved_rotation_ <= 270.0f;
        bool front = saved_rotation_ > 0.0f && saved_rotation_ <= 180.0f;
        if (!front)
        {
            texture_name += "_back";
        }

        auto texture = &RM.getTexture(texture_name);

        this->changeTexture(texture, true);

        static_shadow_->setSize(RMGET<sf::Vector2f>("weapons", this->getId(), front ? "use_size" : "use_size_back"));
        static_shadow_->changeTexture(texture, true);

        this->setSize(static_shadow_->getSize());
        this->setCurrentFrame(0);
        static_shadow_->setCurrentFrame(0);

        auto sine = static_cast<float>(std::sin(this->getRotation() * M_PI / 180.0f));
        auto cosine = static_cast<float>(std::cos(this->getRotation() * M_PI / 180.0f));

        use_elapsed_ = RMGET<float>("weapons", this->getId(), "use_time");
        time_elapsed_ = use_timeout_;
        is_used_ = true;
        spawned_area_ = false;
        this->setPosition(user_->getPosition(), {});
        this->setRotation(0.0f);
        this->setFlipY(false);
        this->setFlipX(flipped);
        setCurrentFrame(0);
        static_shadow_->setCurrentFrame(0);
        updateAnimation(0.0f);
        static_shadow_->updateAnimation(0.0f);

        return reversed_recoil_ * sf::Vector2f{cosine, sine};
    }

    return {0.0f, 0.0f};
}

float MeleeWeapon::getState() const
{
    return 1.0f;
}

void MeleeWeapon::setState(float state)
{
}

MeleeWeaponArea* MeleeWeapon::getMeleeWeaponArea() const
{
    return area_.get();
}

void MeleeWeapon::setPosition(const sf::Vector2f& position, const sf::Vector2f& offset)
{
    if (!is_used_)
    {
        AbstractDrawableObject::setPosition(position + offset);
    }
    else
    {
        bool flipped = saved_rotation_ > 90.0f && saved_rotation_ <= 270.0f;
        bool front = saved_rotation_ > 0.0f && saved_rotation_ <= 180.0f;

        sf::Vector2f use_offset, shadow_offset;

        if (flipped)
        {
            use_offset = RMGET<sf::Vector2f>("weapons", this->getId(), front ? "use_offset_flipped" : "use_offset_back_flipped");
            shadow_offset = RMGET<sf::Vector2f>("weapons", this->getId(), front ? "shadow_offset_flipped" : "shadow_offset_back_flipped");
        }
        else
        {
            use_offset = RMGET<sf::Vector2f>("weapons", this->getId(), front ? "use_offset" : "use_offset_back");
            shadow_offset = RMGET<sf::Vector2f>("weapons", this->getId(), front ? "shadow_offset" : "shadow_offset_back");
        }

        AbstractDrawableObject::setPosition(position + use_offset);
        static_shadow_->setPosition(position + shadow_offset);

        auto radians = saved_rotation_ * M_PI / 180.0f;

        area_->setPosition(position + RMGET<sf::Vector2f>("weapons", this->getId(), "area_offset") +
               RMGET<float>("weapons", this->getId(), "range") * sf::Vector2f{static_cast<float>(std::cos(radians)), static_cast<float>(std::sin(radians))});
    }

}

void MeleeWeapon::setRotation(float angle)
{
    if (!is_used_)
    {
        AbstractDrawableObject::setRotation(angle);
        saved_rotation_ = utils::geo::wrapAngle0_360(angle);
    }
    else
    {
        AbstractDrawableObject::setRotation(0.0f);
    }
}

void MeleeWeapon::update(float time_elapsed)
{
    AbstractWeapon::update(time_elapsed);

    use_elapsed_ -= time_elapsed;
    if (use_elapsed_ >= 0.0f)
    {
        auto old_frame = this->getCurrentFrame();
        updateAnimation(time_elapsed);
        static_shadow_->updateAnimation(time_elapsed);

        if (static_shadow_->getCurrentFrame() != this->getCurrentFrame())
        {
            static_shadow_->setCurrentFrame(this->getCurrentFrame());
        }

        if (this->getCurrentFrame() < old_frame || static_shadow_->getCurrentFrame() < old_frame)
        {
            this->setCurrentFrame(frames_number_ - 1);
            static_shadow_->setCurrentFrame(frames_number_ - 1);
        }

        if (!spawned_area_ && use_elapsed_ < RMGET<float>("weapons", this->getId(), "activate_time_elapsed"))
        {
            spawned_area_ = true;
            bool flipped = saved_rotation_ > 90.0f && saved_rotation_ <= 270.0f;
            bool front = saved_rotation_ > 0.0f && saved_rotation_ <= 180.0f;
            area_->setActive(true);

            auto radians = saved_rotation_ * M_PI / 180.0f;
            sf::Vector2f anim_pos = area_->getPosition() - RMGET<float>("weapons", this->getId(), "range") * sf::Vector2f{static_cast<float>(std::cos(radians)), static_cast<float>(std::sin(radians))};
            anim_pos += RMGET<float>("weapons", this->getId(), "range") * sf::Vector2f(flipped ? -1 : 1, front ? 1 : -1) / 2.0f;
            animation_spawning_function_(this->getId(), anim_pos, flipped);
        }
    }

    if (use_elapsed_ < 0 && use_elapsed_ > -10.0f)
    {
        AbstractDrawableObject::changeTexture(&RM.getTexture("weapons/" + this->getId()), true);
        this->setSize(RMGET<sf::Vector2f>("weapons", this->getId(), "size"));

        use_elapsed_ = -100.0f;
        area_->setActive(false);

        is_used_ = false;
        this->setFlipX(false);
        this->setRotation(user_->getRotateTo());
        setCurrentFrame(0);
        static_shadow_->setCurrentFrame(0);
        user_->setPosition(user_->getPosition());
    }
}

float MeleeWeapon::getDeadlyFactor() const
{
    return deadly_factor_;
}

void MeleeWeapon::changeTexture(sf::Texture* texture, bool reset)
{
    if (!is_used_)
        AbstractDrawableObject::changeTexture(texture, reset);
}

void MeleeWeapon::setFlipY(bool flip)
{
    if (!is_used_)
        AbstractDrawableObject::setFlipY(flip);
    else
    {
        AbstractDrawableObject::setFlipY(false);
        static_shadow_->setFlipY(false);
    }
}

bool MeleeWeapon::isUsed() const
{
    return is_used_;
}

void MeleeWeapon::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (is_used_)
    {
        target.draw(*static_shadow_, states);
    }
//    target.draw(*area_, states);
    target.draw(shape_, states);
}

float MeleeWeapon::getRotation() const
{
    if (!is_used_)
        return AbstractDrawableObject::getRotation();
    else
        return user_->getRotateTo();
}

void MeleeWeapon::setFlipX(bool flip)
{
    AbstractDrawableObject::setFlipX(flip);
    static_shadow_->setFlipX(flip);
}

void MeleeWeapon::registerAnimationSpawningFunction(std::function<void(const std::string&, const sf::Vector2f&, bool)> func)
{
    animation_spawning_function_ = std::move(func);
}

void MeleeWeapon::recalculate()
{
    reversed_recoil_ = (RMGET<float>("weapons", this->getId(), "recoil"));
    use_timeout_ = (RMGET<float>("weapons", this->getId(), "spawn_timeout"));
    deadly_factor_ = (RMGET<float>("weapons", this->getId(), "deadly_factor"));

    for (const auto& id : upgrades_)
    {
        reversed_recoil_ *= RMGET<float>("specials", id, "recoil_factor");
        use_timeout_ *= RMGET<float>("specials", id, "spawn_timeout_factor");
        deadly_factor_ *= RMGET<float>("specials", id, "deadly_factor");
    }
}
