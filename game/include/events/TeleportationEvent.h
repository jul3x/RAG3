//
// Created by jul3x on 27.03.20.
//

#ifndef RAG3_GAME_EVENTS_TELEPORTATIONEVENT_H
#define RAG3_GAME_EVENTS_TELEPORTATIONEVENT_H

#include <R3E/graphics/AnimationEvent.h>


using namespace r3e;

class TeleportationEvent : public graphics::AnimationEvent {

public:
    TeleportationEvent() = delete;

    TeleportationEvent(const sf::Vector2f& position);

private:
    static constexpr short int MAX_FRAMES_COUNT_ = 100;
    static constexpr float DURATION_S_ = 1.0f;
    static constexpr short int WIDTH_PX_ = 120.0f;
    static constexpr short int HEIGHT_PX_ = 120.0f;

    static constexpr float POSITION_OFFSET_Y_ = 60.0f;
};


#endif // RAG3_GAME_EVENTS_TELEPORTATIONEVENT_H
