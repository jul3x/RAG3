//
// Created by jprolejko on 05.09.19.
//

#ifndef RAG3_GRAPHICS_SHOTANIMATION_H
#define RAG3_GRAPHICS_SHOTANIMATION_H

#include <graphics/AnimationEvent.h>


class ShotAnimation : public AnimationEvent {

public:
    ShotAnimation() = delete;

    ShotAnimation(const sf::Vector2f &position,
                  float direction,
                  float radius);

private:
    static constexpr short int MAX_FRAMES_COUNT_ = 6;
    static constexpr float DURATION_S_ = 0.3f;
    static constexpr short int WIDTH_PX_ = 100;
    static constexpr short int HEIGHT_PX_ = 20;
};


#endif // RAG3_GRAPHICS_SHOTNANIMATION_H
