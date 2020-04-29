//
// Created by jul3x on 25.03.20.
//

#include <R3E/system/Config.h>
#include <common/ResourceManager.h>

#include <ui/LeftHud.h>


LeftHud::LeftHud(const sf::Vector2f& position) :
        AbstractDrawableObject(position,
                               {SIZE_X_ * CFG.get<float>("graphics/user_interface_zoom"),
                                SIZE_Y_ * CFG.get<float>("graphics/user_interface_zoom")},
                               &RM.getTexture("hud_left")),
        crystal_(position + sf::Vector2f{CRYSTAL_OFFSET_X_, CRYSTAL_OFFSET_Y_},
                 {CRYSTAL_SIZE_X_, CRYSTAL_SIZE_Y_},
                 &RM.getTexture("crystal"))
{
    this->changeOrigin({0.0f, SIZE_Y_ * CFG.get<float>("graphics/user_interface_zoom")});
}

void LeftHud::updateState(float current, float max)
{
    // TODO
}

void LeftHud::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(shape_, states);
    target.draw(crystal_, states);
}
