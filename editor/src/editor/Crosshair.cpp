//
// Created by jul3x on 10.03.20.
//

#include <cmath>
#include <sstream>

#include <R3E/system/Config.h>

#include <common/ResourceManager.h>
#include <common/DecorationTile.h>

#include <editor/Crosshair.h>
#include <Editor.h>


using namespace editor;

Crosshair::Crosshair() :
        AbstractDrawableObject({0.0f, 0.0f},
                               {SIZE_X_ * CONF<float>("user_interface_zoom"),
                                SIZE_Y_ * CONF<float>("user_interface_zoom")},
                               nullptr),
        text_("", RM.getFont(), CONF<float>("crosshair_text_size"))
{
}

void Crosshair::setPosition(float x, float y)
{
    shape_.setPosition(x, y);
    text_.setPosition(x, y);

    auto coords = Editor::get().getMapCoordinates({x, y});
    std::ostringstream out;
    out.precision(1);
    out << std::fixed << "(" << coords.x << ", " << coords.y << ")" ;
    text_.setString(out.str());
}

void Crosshair::setPosition(const sf::Vector2f& pos)
{
    this->setPosition(pos.x, pos.y);
}

void Crosshair::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(shape_, states);
    target.draw(text_, states);
}

std::string Crosshair::getPositionStr() const
{
    return text_.getString();
}
