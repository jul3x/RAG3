//
// Created by jul3x on 30.09.19.
//

#include <game/events/ShotEvent.h>
#include <game/misc/ResourceManager.h>


ShotEvent::ShotEvent(const sf::Vector2f& position,
                     float direction,
                     float radius) :
        AnimationEvent(&RM.getTexture("animation_shot"),
                       {ShotEvent::WIDTH_PX_, ShotEvent::HEIGHT_PX_}, position,
                       ShotEvent::DURATION_S_, ShotEvent::MAX_FRAMES_COUNT_)
{
    shape_.setRotation(direction);
    shape_.setScale(radius / ShotEvent::WIDTH_PX_, radius / ShotEvent::WIDTH_PX_);
}
