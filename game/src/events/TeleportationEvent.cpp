//
// Created by jul3x on 27.03.20.
//

#include <common/ResourceManager.h>

#include <events/TeleportationEvent.h>


TeleportationEvent::TeleportationEvent(const sf::Vector2f& position) :
        AnimationEvent(
                &RM.getTexture(
                        "animations/teleportation"),
                {TeleportationEvent::WIDTH_PX_, TeleportationEvent::HEIGHT_PX_}, position + sf::Vector2f{0.0f, TeleportationEvent::POSITION_OFFSET_Y_},
                TeleportationEvent::DURATION_S_, TeleportationEvent::MAX_FRAMES_COUNT_,
                AnimationType::QUADRATIC)
{
    shape_.setOrigin(TeleportationEvent::WIDTH_PX_ / 2.0f, TeleportationEvent::HEIGHT_PX_);
}