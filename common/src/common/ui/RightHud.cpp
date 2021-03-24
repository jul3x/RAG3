//
// Created by jul3x on 25.03.20.
//

#include <R3E/system/Config.h>
#include <common/ResourceManager.h>

#include <common/ui/RightHud.h>


RightHud::RightHud(const sf::Vector2f& position) :
        AbstractDrawableObject(position,
                               {SIZE_X_ * CONF<float>("graphics/user_interface_zoom"),
                                SIZE_Y_ * CONF<float>("graphics/user_interface_zoom")},
                               &RM.getTexture("hud_right")),
        face_(position + sf::Vector2f{FACE_OFFSET_X_, FACE_OFFSET_Y_} * CONF<float>("graphics/user_interface_zoom"),
              {FACE_SIZE_X_ * CONF<float>("graphics/user_interface_zoom"),
               FACE_SIZE_Y_ * CONF<float>("graphics/user_interface_zoom")},
               &RM.getTexture("player_face"))
{
    this->changeOrigin({SIZE_X_ * CONF<float>("graphics/user_interface_zoom"),
                        SIZE_Y_ * CONF<float>("graphics/user_interface_zoom")});
}

void RightHud::updateState(float current, float max)
{
    // TODO
}

void RightHud::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(shape_, states);
    target.draw(face_, states);
}
