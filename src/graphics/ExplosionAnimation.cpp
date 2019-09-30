//
// Created by jprolejko on 30.06.19.
//

#include <utils/Utils.h>

#include <graphics/ExplosionAnimation.h>


ExplosionAnimation::ExplosionAnimation(const sf::Vector2f &position,
                                       const sf::Vector2f &direction,
                                       float radius) :
        AnimationEvent("explosion" + std::to_string(utils::getRandom<int>(1, 3)),
                       {ExplosionAnimation::WIDTH_PX_, ExplosionAnimation::HEIGHT_PX_}, position,
                       ExplosionAnimation::DURATION_S_, ExplosionAnimation::MAX_FRAMES_COUNT_,
                       AnimationType::QUADRATIC) {
    float scale_factor = 20.0f;
    std::tuple<float, float> polar_direction = utils::cartesianToPolar(direction);
    float scale = scale_factor * radius * std::get<0>(polar_direction);
    shape_.setScale(scale_factor * radius / ExplosionAnimation::WIDTH_PX_,
                              scale_factor * radius / ExplosionAnimation::HEIGHT_PX_);
    shape_.setRotation(180.0f + std::get<1>(polar_direction) * 180.0f / M_PI);
}