//
// Created by jul3x on 30.06.19.
//

#include <R3E/utils/Utils.h>

#include <events/ExplosionEvent.h>
#include <misc/ResourceManager.h>


ExplosionEvent::ExplosionEvent(const sf::Vector2f& position,
                               float radius) :
        AnimationEvent(
                &RM.getTexture(
                        "animation_explosion" + std::to_string(utils::num::getRandom<int>(1, 3))),
                {ExplosionEvent::WIDTH_PX_, ExplosionEvent::HEIGHT_PX_}, position,
                ExplosionEvent::DURATION_S_, ExplosionEvent::MAX_FRAMES_COUNT_,
                AnimationType::QUADRATIC)
{
    shape_.setScale(radius / ExplosionEvent::WIDTH_PX_,
                    radius / ExplosionEvent::HEIGHT_PX_);
}