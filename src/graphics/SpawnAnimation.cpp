//
// Created by jprolejko on 30.09.19.
//

#include <graphics/SpawnAnimation.h>


SpawnAnimation::SpawnAnimation(const sf::Vector2f &position,
                               float radius) :
        AnimationEvent("spawn",
                       {SpawnAnimation::WIDTH_PX_, SpawnAnimation::HEIGHT_PX_}, position,
                       SpawnAnimation::DURATION_S_, SpawnAnimation::MAX_FRAMES_COUNT_) {
        float scale_factor = 10.0f;
        float scale = scale_factor * radius;
        shape_.setScale(scale_factor * radius / SpawnAnimation::WIDTH_PX_,
                        scale_factor * radius / SpawnAnimation::HEIGHT_PX_);
}
