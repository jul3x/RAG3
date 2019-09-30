//
// Created by jprolejko on 27.02.19.
//

#ifndef RAG3_SYSTEM_ENGINE_H
#define RAG3_SYSTEM_ENGINE_H

#include <memory>

#include <graphics/Graphics.h>
#include <graphics/AnimationEvent.h>
#include <objects/Player.h>
#include <objects/Map.h>
#include <system/UserInterface.h>
#include <physics/Physics.h>


class Engine {

public:
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;

    static Engine& getInstance() {
        static Engine engine_instance;
        return engine_instance;
    }

    Player& getPlayer();

    void update(int frame_rate);

    static sf::Vector2i detectResolution();

private:
    Engine();

    void ensureConstantFrameRate(int frame_rate);
    void restartClock();

    Physics physics_;
    UserInterface ui_;
    sf::Clock clock_;
    sf::Time time_;

    Player player_;
    Map map_;
    std::list<std::unique_ptr<AnimationEvent>> animation_events_;
};


#endif //RAG3_SYSTEM_ENGINE_H
