//
// Created by jprolejko on 30.09.19.
//

#include <game/animations/ShotAnimation.h>


ShotAnimation::ShotAnimation(const sf::Vector2f &position,
                             float direction,
                             float radius) :
        AnimationEvent("shot",
                       {ShotAnimation::WIDTH_PX_, ShotAnimation::HEIGHT_PX_}, position,
                       ShotAnimation::DURATION_S_, ShotAnimation::MAX_FRAMES_COUNT_) {

        shape_.setRotation(direction);
        shape_.setScale(radius, radius);

}
