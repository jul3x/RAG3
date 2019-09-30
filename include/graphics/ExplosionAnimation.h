//
// Created by jprolejko on 30.08.19.
//

#ifndef RAG3_GRAPHICS_EXPLOSIONANIMATION_H
#define RAG3_GRAPHICS_EXPLOSIONANIMATION_H

#include <graphics/AnimationEvent.h>


class ExplosionAnimation : public AnimationEvent {

public:
    ExplosionAnimation() = delete;

    ExplosionAnimation(const sf::Vector2f &position,
                       const sf::Vector2f &velocity,
                       float radius);

private:
    static constexpr short int MAX_FRAMES_COUNT_ = 64;
    static constexpr float DURATION_S_ = 1.0f;
    static constexpr short int WIDTH_PX_ = 512;
    static constexpr short int HEIGHT_PX_ = 512;
};


#endif // RAG3_GRAPHICS_EXPLOSIONANIMATION_H
