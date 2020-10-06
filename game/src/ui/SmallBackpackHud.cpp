//
// Created by jul3x on 06.10.20.
//

#include <R3E/system/Config.h>
#include <common/ResourceManager.h>

#include <ui/SmallBackpackHud.h>


SmallBackpackHud::SmallBackpackHud(const sf::Vector2f& position) :
        AbstractDrawableObject(position,
                               {SIZE_X_ * CONF<float>("graphics/user_interface_zoom"),
                                SIZE_Y_ * CONF<float>("graphics/user_interface_zoom")},
                               &RM.getTexture("items_hud"))
{
    this->changeOrigin({SIZE_X_ * CONF<float>("graphics/user_interface_zoom"), 0.0f});
}

void SmallBackpackHud::update(float time_elapsed)
{
}

void SmallBackpackHud::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(shape_, states);
}
