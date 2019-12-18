//
// Created by jul3x on 27.02.19.
//

#ifndef RAG3_ENGINE_SYSTEM_ENGINE_H
#define RAG3_ENGINE_SYSTEM_ENGINE_H

#include <memory>
#include <list>

#include <engine/graphics/Graphics.h>
#include <engine/graphics/AbstractCamera.h>
#include <engine/graphics/AnimationEvent.h>
#include <engine/system/AbstractUserInterface.h>
#include <engine/system/AbstractGame.h>


class Engine {

public:
    Engine(const Engine&) = delete;

    Engine& operator=(const Engine&) = delete;

    Engine();

    void initializeGraphics(const sf::Vector2i& size, const std::string& title, int style);

    void registerUI(AbstractUserInterface* user_interface);

    void registerCamera(AbstractCamera* camera);

    void registerGame(AbstractGame* game);

    void spawnAnimationEvent(const std::shared_ptr<AnimationEvent>& event);

    void setVisibility(AbstractDrawableObject& object) const;

    void update(int frame_rate);

    static sf::Vector2i detectResolution();

private:
    void ensureConstantFrameRate(int frame_rate);

    void restartClock();

    // Engine components
    std::unique_ptr<Graphics> graphics_;

    // Registered by Game
    AbstractUserInterface* ui_;
    AbstractCamera* camera_;
    AbstractGame* game_;

    sf::Clock clock_;
    sf::Time time_;

    std::list<std::shared_ptr<AnimationEvent>> animation_events_;
};


#endif //RAG3_ENGINE_SYSTEM_ENGINE_H
