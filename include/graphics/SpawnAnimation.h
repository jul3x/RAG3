//
// Created by jprolejko on 05.09.19.
//

#ifndef RAG3_GRAPHICS_SPAWNANIMATION_H
#define RAG3_GRAPHICS_SPAWNANIMATION_H

#include <graphics/AnimationEvent.h>


class SpawnAnimation : public AnimationEvent {

public:
    SpawnAnimation() = delete;

    SpawnAnimation(const sf::Vector2f &position,
                   float radius) : AnimationEvent("spawn",
                                                  {SpawnAnimation::WIDTH_PX_, SpawnAnimation::HEIGHT_PX_}, position,
                                                  SpawnAnimation::DURATION_S_, SpawnAnimation::MAX_FRAMES_COUNT_) {
        float scale_factor = 10.0f;
        float scale = scale_factor * radius;
        shape_.setScale(scale_factor * radius / SpawnAnimation::WIDTH_PX_,
                        scale_factor * radius / SpawnAnimation::HEIGHT_PX_);
    }

private:
    static constexpr short int MAX_FRAMES_COUNT_ = 16;
    static constexpr float DURATION_S_ = 0.3f;
    static constexpr short int WIDTH_PX_ = 200;
    static constexpr short int HEIGHT_PX_ = 200;
};


#endif // RAG3_GRAPHICS_SPAWNANIMATION_H
