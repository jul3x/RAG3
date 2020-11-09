//
// Created by jul3x on 06.10.20.
//

#include <R3E/system/Config.h>
#include <common/ResourceManager.h>

#include <ui/SmallBackpackHud.h>
#include <Game.h>


SmallBackpackHud::SmallBackpackHud(const sf::Vector2f& position) :
        AbstractDrawableObject(position,
                               {SIZE_X_ * CONF<float>("graphics/user_interface_zoom"),
                                SIZE_Y_ * CONF<float>("graphics/user_interface_zoom")},
                               &RM.getTexture("items_hud"))
{
    this->changeOrigin({SIZE_X_ * CONF<float>("graphics/user_interface_zoom"), 0.0f});

    objects_.emplace_back(position + j3x::getObj<sf::Vector2f>(CONF<j3x::List>("graphics/small_backpack_pos"), 0), "health");
    objects_.emplace_back(position + j3x::getObj<sf::Vector2f>(CONF<j3x::List>("graphics/small_backpack_pos"), 1), "more_speed");
    objects_.emplace_back(position + j3x::getObj<sf::Vector2f>(CONF<j3x::List>("graphics/small_backpack_pos"), 2), "crystal");

    for (auto& object : objects_)
    {
        object.setSize(2.0f * RMGET<sf::Vector2f>("specials", object.getId(), "size"));
        object.changeOrigin(RMGET<sf::Vector2f>("specials", object.getId(), "size"));
        object.removeShadow();
        object.setColor(255, 255, 255, 80);
        numbers_.emplace_back("", RM.getFont(), CONF<float>("graphics/backpack_text_size"));
        numbers_.back().setPosition(object.getPosition() + CONF<sf::Vector2f>("graphics/backpack_number_diff"));
    }
}

void SmallBackpackHud::update(float time_elapsed)
{
    auto& backpack = Game::get().getPlayer().getBackpack();

    int health = 0, speed = 0, crystal = 0;
    for (auto& object : backpack)
    {
        if (object.first.getId() == "health")
        {
            health = object.second;
        }
        else if (object.first.getId() == "more_speed")
        {
            speed = object.second;
        }
        else if (object.first.getId() == "crystal")
        {
            crystal = object.second;
        }
    }

    objects_[0].setColor(255, 255, 255, health ? 255 : 80);
    objects_[1].setColor(255, 255, 255, speed ? 255 : 80);
    objects_[2].setColor(255, 255, 255, crystal ? 255 : 80);

    numbers_[0].setString(health > 1 ? std::to_string(health) : "");
    numbers_[1].setString(speed > 1 ? std::to_string(speed) : "");
    numbers_[2].setString(crystal > 1 ? std::to_string(crystal) : "");
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
