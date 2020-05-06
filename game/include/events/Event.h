//
// Created by jul3x on 30.04.20.
//

#ifndef RAG3_GAME_INCLUDE_EVENTS_EVENT_H
#define RAG3_GAME_INCLUDE_EVENTS_EVENT_H

#include <R3E/graphics/AnimationEvent.h>


using namespace r3e;

class Event : public graphics::AnimationEvent {

public:
    Event() = delete;

    Event(const sf::Vector2f& position, const std::string& id);

    Event(const sf::Vector2f& position, const std::string& id,
          float direction,
          float radius);

};


#endif //RAG3_GAME_INCLUDE_EVENTS_EVENT_H
