
//
// Created by jul3x on 12.05.20.
//

#include <common/ResourceManager.h>
#include <common/MeleeWeapon.h>


MeleeWeapon::MeleeWeapon(const std::string& id) :
        AbstractWeapon({utils::j3x::get<float>(RM.getObjectParams("weapons", id), "size_x"),
                        utils::j3x::get<float>(RM.getObjectParams("weapons", id), "size_y")},
                       {utils::j3x::get<float>(RM.getObjectParams("weapons", id), "offset_x"),
                        utils::j3x::get<float>(RM.getObjectParams("weapons", id), "offset_y")},
                       id),
        enabled_time_(0.0f),
        reversed_recoil_(utils::j3x::get<float>(RM.getObjectParams("weapons", id), "recoil")),
        use_timeout_(utils::j3x::get<float>(RM.getObjectParams("weapons", id), "spawn_timeout"))
{
    this->changeOrigin(sf::Vector2f(0.0f,
                                    utils::j3x::get<float>(RM.getObjectParams("weapons", id), "size_y")) / 2.0f +
                       sf::Vector2f(utils::j3x::get<float>(RM.getObjectParams("weapons", id), "offset_x"),
                                    utils::j3x::get<float>(RM.getObjectParams("weapons", id), "offset_y")));
}

sf::Vector2f MeleeWeapon::use()
{
    if (time_elapsed_ < 0.0f)
    {
        auto sine = static_cast<float>(std::sin(this->getRotation() * M_PI / 180.0f));
        auto cosine = static_cast<float>(std::cos(this->getRotation() * M_PI / 180.0f));
        auto offset_position = this->getPosition();

        spawning_function_("", offset_position, this->getRotation());

        time_elapsed_ = use_timeout_;

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
