//
// Created by jul3x on 31.10.19.
//

#ifndef RAG3_GAME_EVENTS_SPARKSEVENT_H
#define RAG3_GAME_EVENTS_SPARKSEVENT_H

#include <engine/graphics/AnimationEvent.h>


using namespace r3e;

class SparksEvent : public graphics::AnimationEvent {

public:
    SparksEvent() = delete;

    SparksEvent(const sf::Vector2f& position,
                float direction,
                float radius);

private:
    static constexpr short int MAX_FRAMES_COUNT_ = 4;
    static constexpr float DURATION_S_ = 0.3f;
    static constexpr short int WIDTH_PX_ = 256;
    static constexpr short int HEIGHT_PX_ = 212;
};


#endif // RAG3_GAME_EVENTS_SPARKSEVENT_H
