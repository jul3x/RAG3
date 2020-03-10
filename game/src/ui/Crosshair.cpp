//
// Created by jul3x on 21.02.20.
//

#include <R3E/system/Config.h>

#include <misc/ResourceManager.h>
#include <ui/Crosshair.h>


Crosshair::Crosshair() :
        AbstractDrawableObject({0.0f, 0.0f},
                               {SIZE_X_ * CFG.getFloat("user_interface_zoom"),
                                SIZE_Y_ * CFG.getFloat("user_interface_zoom")},
                               &RM.getTexture("crosshair"))
{
}
