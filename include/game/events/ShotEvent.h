//
// Created by jul3x on 05.09.19.
//

#ifndef RAG3_GAME_EVENTS_SHOTEVENT_H
#define RAG3_GAME_EVENTS_SHOTEVENT_H

#include <engine/graphics/AnimationEvent.h>


using namespace r3e;

class ShotEvent : public graphics::AnimationEvent {

public:
    ShotEvent() = delete;

    ShotEvent(const sf::Vector2f& position,
              float direction,
              float radius);

private:
    static constexpr short int MAX_FRAMES_COUNT_ = 6;
    static constexpr float DURATION_S_ = 0.3f;
    static constexpr short int WIDTH_PX_ = 100;
    static constexpr short int HEIGHT_PX_ = 20;
};


#endif // RAG3_GAME_EVENTS_SHOTEVENT_H
