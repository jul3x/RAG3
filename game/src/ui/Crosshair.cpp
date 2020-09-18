//
// Created by jul3x on 21.02.20.
//

#include <R3E/system/Config.h>

#include <common/ResourceManager.h>
#include <ui/Crosshair.h>


Crosshair::Crosshair() :
        AbstractDrawableObject({0.0f, 0.0f},
                               CONF<sf::Vector2f>("graphics/crosshair_size"),
                               &RM.getTexture("crosshair"))
{
}
