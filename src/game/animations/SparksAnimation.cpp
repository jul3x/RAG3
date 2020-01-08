//
// Created by jul3x on 31.10.19.
//

#include <game/animations/SparksAnimation.h>
#include <game/misc/ResourceManager.h>


SparksAnimation::SparksAnimation(const sf::Vector2f& position,
                                 float direction,
                                 float radius) :
        AnimationEvent(&ResourceManager::getInstance().getTexture("animation_sparks"),
                       {SparksAnimation::WIDTH_PX_, SparksAnimation::HEIGHT_PX_}, position,
                       SparksAnimation::DURATION_S_, SparksAnimation::MAX_FRAMES_COUNT_)
{

    shape_.setRotation(direction);
    shape_.setScale(radius, radius);

}
