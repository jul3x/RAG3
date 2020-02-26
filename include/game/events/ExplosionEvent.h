//
// Created by jul3x on 30.08.19.
//

#ifndef RAG3_GAME_EVENTS_EXPLOSIONEVENT_H
#define RAG3_GAME_EVENTS_EXPLOSIONEVENT_H

#include <engine/graphics/AnimationEvent.h>


using namespace r3e;

class ExplosionEvent : public graphics::AnimationEvent {

public:
    ExplosionEvent() = delete;

    ExplosionEvent(const sf::Vector2f& position,
                       float radius);

private:
    static constexpr short int MAX_FRAMES_COUNT_ = 64;
    static constexpr float DURATION_S_ = 1.0f;
    static constexpr short int WIDTH_PX_ = 512;
    static constexpr short int HEIGHT_PX_ = 512;
};


#endif // RAG3_GAME_EVENTS_EXPLOSIONEVENT_H
