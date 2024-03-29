//
// Created by jul3x on 25.03.20.
//

#include <R3E/system/Config.h>
#include <common/ResourceManager.h>

#include <common/ui/hud/RightHud.h>


RightHud::RightHud(const sf::Vector2f& position) :
        AbstractDrawableObject(position,
                               {SIZE_X_ * CONF<float>("graphics/user_interface_zoom"),
                                SIZE_Y_ * CONF<float>("graphics/user_interface_zoom")},
                               &RM.getTexture("hud_right")),
        face_(position + sf::Vector2f{FACE_OFFSET_X_, FACE_OFFSET_Y_} * CONF<float>("graphics/user_interface_zoom"),
              CONF<sf::Vector2f>("graphics/face_size"),
              &RM.getTexture("characters/henry/face")),
        level_(CONF<float>("graphics/inertial_states_change_speed")),
        exp_(CONF<float>("graphics/inertial_states_change_speed")),
        level_text_("level: 0, experience: 0", RM.getFont(), CONF<float>("graphics/level_text_size")),
        name_("", RM.getFont(), CONF<float>("graphics/name_text_size"))
{
    this->changeOrigin({SIZE_X_ * CONF<float>("graphics/user_interface_zoom"),
                        SIZE_Y_ * CONF<float>("graphics/user_interface_zoom")});

    level_text_.setFillColor(sf::Color::White);
    level_text_.setPosition(position + sf::Vector2f{LEVEL_X_, LEVEL_Y_}
                                       * CONF<float>("graphics/user_interface_zoom"));
    auto bound = level_text_.getLocalBounds();
    level_text_.setOrigin(bound.width, bound.height);

    name_.setFillColor(sf::Color::White);
    name_.setPosition(position + sf::Vector2f{NAME_X_, NAME_Y_}
                                 * CONF<float>("graphics/user_interface_zoom"));
}

void RightHud::update(int level, int exp, float time_elapsed)
{
    level_.setState(static_cast<float>(level));
    exp_.setState(static_cast<float>(exp));

    level_.update(time_elapsed);
    exp_.update(time_elapsed);

    auto level_str = std::to_string(static_cast<int>(std::round(level_.getState())));
    auto exp_str = std::to_string(static_cast<int>(std::round(exp_.getState())));

    level_text_.setString("level: " + level_str + ", experience: " + exp_str);
    auto bound = level_text_.getLocalBounds();
    level_text_.setOrigin(bound.width, bound.height);
}

void RightHud::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(shape_, states);
    target.draw(face_, states);
    target.draw(level_text_, states);
    target.draw(name_, states);
}

void RightHud::setName(const std::string& name)
{
    static constexpr auto max_length = 10;
    auto bound = name_.getLocalBounds();
    name_.setOrigin(bound.width, bound.height);

    name_.setString(name);

    auto length = name.length();
    auto size = CONF<float>("graphics/name_text_size");
    if (length > max_length)
        size = max_length / static_cast<float>(length) * size;
    name_.setCharacterSize(static_cast<int>(size));
}

void RightHud::setCharacter(const std::string& character)
{
    face_.changeTexture(&RM.getTexture("characters/" + character + "/face"));
}
