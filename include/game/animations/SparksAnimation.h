//
// Created by jprolejko on 31.10.19.
//

#ifndef RAG3_GAME_ANIMATIONS_SPARKSANIMATION_H
#define RAG3_GAME_ANIMATIONS_SPARKSANIMATION_H

#include <engine/graphics/AnimationEvent.h>


class SparksAnimation : public AnimationEvent {

public:
    SparksAnimation() = delete;

    SparksAnimation(const sf::Vector2f &position,
                    float direction,
                    float radius);

private:
    static constexpr short int MAX_FRAMES_COUNT_ = 4;
    static constexpr float DURATION_S_ = 0.3f;
    static constexpr short int WIDTH_PX_ = 64;
    static constexpr short int HEIGHT_PX_ = 53;
};


#endif // RAG3_GAME_ANIMATIONS_SPARKSANIMATION_H
