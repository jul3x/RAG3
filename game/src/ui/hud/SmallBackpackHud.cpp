//
// Created by jul3x on 06.10.20.
//

#include <R3E/system/Config.h>
#include <common/ResourceManager.h>

#include <ui/hud/SmallBackpackHud.h>


SmallBackpackHud::SmallBackpackHud(Player* player, const sf::Vector2f& position) :
        AbstractDrawableObject(position,
                               {SIZE_X_ * CONF<float>("graphics/user_interface_zoom"),
                                SIZE_Y_ * CONF<float>("graphics/user_interface_zoom")},
                               &RM.getTexture("items_hud")),
       player_(player)
{
    this->changeOrigin({SIZE_X_ * CONF<float>("graphics/user_interface_zoom"), 0.0f});

    objects_.emplace_back(position + j3x::getObj<sf::Vector2f>(CONF<j3x::List>("graphics/small_backpack_pos"), 0), NAMES_[0]);
    objects_.emplace_back(position + j3x::getObj<sf::Vector2f>(CONF<j3x::List>("graphics/small_backpack_pos"), 1), NAMES_[1]);
    objects_.emplace_back(position + j3x::getObj<sf::Vector2f>(CONF<j3x::List>("graphics/small_backpack_pos"), 2), NAMES_[2]);

    for (auto& object : objects_)
    {
        object.setSize(CONF<float>("graphics/global_zoom") * RMGET<sf::Vector2f>("specials", object.getId(), "size"));
        object.changeOrigin(CONF<float>("graphics/global_zoom") * RMGET<sf::Vector2f>("specials", object.getId(), "size") / 2.0f);
        object.removeShadow();
        object.setColor(255, 255, 255, 80);
        numbers_.emplace_back("", RM.getFont(), CONF<float>("graphics/backpack_text_size"));
        numbers_.back().setPosition(object.getPosition() + CONF<sf::Vector2f>("graphics/backpack_number_diff"));
    }
}

void SmallBackpackHud::update(float time_elapsed)
{
    auto& backpack = player_->getBackpack();

    int health = 0, speed = 0, rag3 = 0;
    for (auto& object : backpack)
    {
        if (object.first.getId() == NAMES_[0])
        {
            health = object.second;
        }
        else if (object.first.getId() == NAMES_[1])
        {
            speed = object.second;
        }
        else if (object.first.getId() == NAMES_[2])
        {
            rag3 = object.second;
        }
    }

    objects_[0].setColor(255, 255, 255, health ? 255 : 80);
    objects_[1].setColor(255, 255, 255, speed ? 255 : 80);
    objects_[2].setColor(255, 255, 255, rag3 ? 255 : 80);

    numbers_[0].setString(health > 1 ? std::to_string(health) : "");
    numbers_[1].setString(speed > 1 ? std::to_string(speed) : "");
    numbers_[2].setString(rag3 > 1 ? std::to_string(rag3) : "");
}

void SmallBackpackHud::registerGui(tgui::Gui* gui, tgui::Theme* theme)
{
    auto placeholder_size = CONF<sf::Vector2f>("graphics/backpack_placeholder_size");
    auto pos_offset = this->getPosition() - placeholder_size / 2.0f;

    for (size_t i = 0; i < 3; ++i)
    {
        tooltips_.emplace_back(theme, pos_offset + j3x::getObj<sf::Vector2f>(CONF<j3x::List>("graphics/small_backpack_pos"), i, false));
        tooltips_.back().bindFunction(std::bind([this](const std::string& name){ player_->useItem(name); }, NAMES_[i]));
        tooltips_.back().bindText(RMGET<std::string>("specials", NAMES_[i], "tooltip_header"),
                         RMGET<std::string>("specials", NAMES_[i], "tooltip"));
        tooltips_.back().bindGui(gui);
        tooltips_.back().show(true);
    }
}


void SmallBackpackHud::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(shape_, states);
    for (auto& object : objects_)
    {
        target.draw(object, states);
    }

    for (auto& number : numbers_)
    {
        target.draw(number, states);
    }
}

void SmallBackpackHud::doShow(bool hide)
{
    for (auto& tooltip : tooltips_)
    {
        tooltip.show(hide);
    }
}
