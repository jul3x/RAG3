//
// Created by jul3x on 25.03.20.
//

#include <R3E/system/Config.h>
#include <common/ResourceManager.h>

#include <common/ui/hud/LeftHud.h>


LeftHud::LeftHud(const sf::Vector2f& position) :
        AbstractDrawableObject(position,
                               {SIZE_X_ * CONF<float>("graphics/user_interface_zoom"),
                                SIZE_Y_ * CONF<float>("graphics/user_interface_zoom")},
                               &RM.getTexture("hud_left"))
{
    this->changeOrigin({0.0f, SIZE_Y_ * CONF<float>("graphics/user_interface_zoom")});
}
