//
// Created by jul3x on 30.06.19.
//

#include <engine/utils/Utils.h>

#include <game/animations/ExplosionAnimation.h>
#include <game/misc/ResourceManager.h>


ExplosionAnimation::ExplosionAnimation(const sf::Vector2f& position,
                                       float radius) :
        AnimationEvent(
                &ResourceManager::getInstance().getTexture(
                        "animation_explosion" + std::to_string(utils::getRandom<int>(1, 3))),
                {ExplosionAnimation::WIDTH_PX_, ExplosionAnimation::HEIGHT_PX_}, position,
                ExplosionAnimation::DURATION_S_, ExplosionAnimation::MAX_FRAMES_COUNT_,
                AnimationType::QUADRATIC)
{
    float scale_factor = 20.0f;
    float scale = scale_factor * radius;
    shape_.setScale(scale_factor * radius / ExplosionAnimation::WIDTH_PX_,
                    scale_factor * radius / ExplosionAnimation::HEIGHT_PX_);
}