//
// Created by jul3x on 10.03.20.
//

#include <cmath>

#include <R3E/system/Config.h>

#include <common/ResourceManager.h>
#include <common/DecorationTile.h>

#include <editor/Crosshair.h>


using namespace editor;

Crosshair::Crosshair() :
        AbstractDrawableObject({0.0f, 0.0f},
                               {SIZE_X_ * CFG.getFloat("user_interface_zoom"),
                                SIZE_Y_ * CFG.getFloat("user_interface_zoom")},
                               nullptr)
{
}
