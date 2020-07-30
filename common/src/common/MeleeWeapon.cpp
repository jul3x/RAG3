
//
// Created by jul3x on 12.05.20.
//

#include <R3E/system/Config.h>

#include <common/ResourceManager.h>
#include <common/MeleeWeapon.h>


MeleeWeapon::MeleeWeapon(Character* user, const std::string& id) :
        AbstractWeapon(user,
                       {utils::j3x::get<float>(RM.getObjectParams("weapons", id), "size_x"),
                        utils::j3x::get<float>(RM.getObjectParams("weapons", id), "size_y")},
                       {utils::j3x::get<float>(RM.getObjectParams("weapons", id), "offset_x"),
                        utils::j3x::get<float>(RM.getObjectParams("weapons", id), "offset_y")},
                       id),
        reversed_recoil_(utils::j3x::get<float>(RM.getObjectParams("weapons", id), "recoil")),
        use_timeout_(utils::j3x::get<float>(RM.getObjectParams("weapons", id), "spawn_timeout")),
        deadly_factor_(utils::j3x::get<float>(RM.getObjectParams("weapons", id), "deadly_factor")),
        use_elapsed_(0.0f),
        saved_rotation_(0.0f),
        is_used_(false)
{
    this->changeOrigin(sf::Vector2f(0.0f,
                                    utils::j3x::get<float>(RM.getObjectParams("weapons", id), "size_y")) / 2.0f +
                       sf::Vector2f(utils::j3x::get<float>(RM.getObjectParams("weapons", id), "offset_x"),
                                    utils::j3x::get<float>(RM.getObjectParams("weapons", id), "offset_y")));

    area_ = std::make_unique<MeleeWeaponArea>(this, CFG.get<float>("melee_weapon_range"));

    auto shadow_pos = this->getPosition();
    static_shadow_ = std::make_unique<graphics::TransformedTextureShadow>(
            shadow_pos, sf::Vector2f{utils::j3x::get<float>(RM.getObjectParams("weapons", this->getId()), "use_size_x"),
                                     utils::j3x::get<float>(RM.getObjectParams("weapons", this->getId()), "use_size_y")},
            CFG.get<float>("graphics/shadow_direction"),
            CFG.get<float>("graphics/shadow_length_factor"),
            &RM.getTexture("weapons/" + user_->getId() + "_" + this->getId()), sf::Color(CFG.get<int>("graphics/shadow_color")),
            z_index_,
            utils::j3x::get<int>(RM.getObjectParams("weapons", id), "frames_number"),
            utils::j3x::get<float>(RM.getObjectParams("weapons", id), "frame_duration"));
}

sf::Vector2f MeleeWeapon::use()
{
    if (time_elapsed_ < 0.0f)
    {
        auto added_str = "";
        if (saved_rotation_ > 90.0f && saved_rotation_ <= 270.0f)
        {
            added_str = "_2";
        }
        auto texture = &RM.getTexture("weapons/" + user_->getId() + "_" + this->getId() + added_str);
        this->changeTexture(texture, true);
        static_shadow_->changeTexture(texture, true);

        this->setSize(static_shadow_->getSize());
        this->setCurrentFrame(0);
        static_shadow_->setCurrentFrame(0);
        this->setPosition(user_->getPosition(), {});
        use_elapsed_ = utils::j3x::get<float>(RM.getObjectParams("weapons", this->getId()), "use_time");
        time_elapsed_ = use_timeout_;
        is_used_ = true;

        auto sine = static_cast<float>(std::sin(this->getRotation() * M_PI / 180.0f));
        auto cosine = static_cast<float>(std::cos(this->getRotation() * M_PI / 180.0f));
        auto offset_position = this->getPosition();

        spawning_function_(user_, "", offset_position, this->getRotation());

        area_->setActive(true);

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
        auto use_offset = sf::Vector2f(utils::j3x::get<float>(RM.getObjectParams("weapons", this->getId()), "use_offset_x"),
                                       utils::j3x::get<float>(RM.getObjectParams("weapons", this->getId()), "use_offset_y"));

        auto shadow_offset = sf::Vector2f(utils::j3x::get<float>(RM.getObjectParams("weapons", this->getId()), "shadow_offset_x"),
                                          utils::j3x::get<float>(RM.getObjectParams("weapons", this->getId()), "shadow_offset_y"));
        AbstractDrawableObject::setPosition(position + use_offset);
        static_shadow_->setPosition(position + shadow_offset);

        auto radians = saved_rotation_ * M_PI / 180.0f;
        area_->setPosition(position + CFG.get<float>("melee_weapon_offset_x") *
                sf::Vector2f{static_cast<float>(std::cos(radians)), static_cast<float>(std::sin(radians))});
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
    }

    if (use_elapsed_ < 0 && use_elapsed_ > -10.0f)
    {
        AbstractDrawableObject::changeTexture(&RM.getTexture("weapons/" + this->getId()), true);
        this->setSize({utils::j3x::get<float>(RM.getObjectParams("weapons", this->getId()), "size_x"),
                       utils::j3x::get<float>(RM.getObjectParams("weapons", this->getId()), "size_y")});

        use_elapsed_ = -100.0f;
        area_->setActive(false);

        is_used_ = false;
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
        AbstractDrawableObject::setFlipY(false);
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
    target.draw(shape_, states);
}

float MeleeWeapon::getRotation() const
{
    if (!is_used_)
        return AbstractDrawableObject::getRotation();
    else
        return user_->getRotateTo();
}
