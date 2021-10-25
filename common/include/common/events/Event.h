//
// Created by jul3x on 30.04.20.
//

#ifndef RAG3_GAME_INCLUDE_EVENTS_EVENT_H
#define RAG3_GAME_INCLUDE_EVENTS_EVENT_H

#include <memory>

#include <R3E/graphics/AnimationEvent.h>


using namespace r3e;

class Journal;

class Event : public graphics::AnimationEvent, public Identifiable {

public:
    Event() = delete;

    ~Event() override;

    Event(Journal* journal, const sf::Vector2f& position, const std::string& id);

    Event(Journal* journal, const sf::Vector2f& position, const std::string& id,
          float direction,
          float radius);

private:
    Journal* journal_;

};


#endif //RAG3_GAME_INCLUDE_EVENTS_EVENT_H
