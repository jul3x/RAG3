//
// Created by jul3x on 30.06.19.
//

#include <engine/utils/Utils.h>

#include <game/events/ExplosionEvent.h>
#include <game/misc/ResourceManager.h>


ExplosionEvent::ExplosionEvent(const sf::Vector2f& position,
                               float radius) :
        AnimationEvent(
                &RM.getTexture(
                        "animation_explosion" + std::to_string(utils::num::getRandom<int>(1, 3))),
                {ExplosionEvent::WIDTH_PX_, ExplosionEvent::HEIGHT_PX_}, position,
                ExplosionEvent::DURATION_S_, ExplosionEvent::MAX_FRAMES_COUNT_,
                AnimationType::QUADRATIC)
{
    float scale_factor = 20.0f;
    float scale = scale_factor * radius;
    shape_.setScale(scale_factor * radius / ExplosionEvent::WIDTH_PX_,
                    scale_factor * radius / ExplosionEvent::HEIGHT_PX_);
}