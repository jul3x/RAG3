//
// Created by jul3x on 31.10.19.
//

#include <game/events/SparksEvent.h>
#include <game/misc/ResourceManager.h>


SparksEvent::SparksEvent(const sf::Vector2f& position,
                         float direction,
                         float radius) :
        AnimationEvent(&RM.getTexture("animation_sparks"),
                       {SparksEvent::WIDTH_PX_, SparksEvent::HEIGHT_PX_}, position,
                       SparksEvent::DURATION_S_, SparksEvent::MAX_FRAMES_COUNT_)
{
    shape_.setRotation(direction);
    shape_.setScale(radius, radius);
}
