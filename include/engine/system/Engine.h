//
// Created by jul3x on 27.02.19.
//

#ifndef RAG3_ENGINE_SYSTEM_ENGINE_H
#define RAG3_ENGINE_SYSTEM_ENGINE_H

#include <memory>
#include <unordered_set>
#include <list>

#include <engine/graphics/Graphics.h>
#include <engine/graphics/AbstractCamera.h>
#include <engine/graphics/AnimationEvent.h>
#include <engine/system/AbstractUserInterface.h>
#include <engine/system/AbstractGame.h>
#include <engine/objects/AbstractPhysicalObject.h>


class Engine {

public:
    Engine(const Engine&) = delete;

    Engine& operator=(const Engine&) = delete;

    Engine();

    void initializeGraphics(const sf::Vector2i& size, const std::string& title, int style);

    void registerUI(AbstractUserInterface* user_interface);

    void registerCamera(AbstractCamera* camera);

    void registerGame(AbstractGame* game);

    void registerStaticObject(StaticObject* obj);

    void registerDynamicObject(DynamicObject* obj);

    void registerHoveringObject(HoveringObject* obj);

    void deleteStaticObject(StaticObject* obj);

    void deleteDynamicObject(DynamicObject* obj);

    void deleteHoveringObject(HoveringObject* obj);

    void spawnAnimationEvent(const std::shared_ptr<AnimationEvent>& event);

    void setVisibility(AbstractDrawableObject& object) const;

    void update(int frame_rate);

    static sf::Vector2i detectResolution();

private:
    void DSCollisions(float time_elapsed);

    void DDCollisions(float time_elapsed);

    void HSCollisions(float time_elapsed);

    void HDCollisions(float time_elapsed);

    void updateAnimationEvents(float time_elapsed);

    void draw();

    void ensureConstantFrameRate(int frame_rate);

    void restartClock();

    // Engine components
    std::unique_ptr<Graphics> graphics_;

    // Registered by Game
    AbstractUserInterface* ui_;
    AbstractCamera* camera_;
    AbstractGame* game_;

    // Objects
    std::unordered_set<StaticObject*> s_objects_;
    std::unordered_set<DynamicObject*> d_objects_;
    std::unordered_set<HoveringObject*> h_objects_;


    sf::Clock clock_;
    sf::Time time_;

    std::list<std::shared_ptr<AnimationEvent>> animation_events_;
};


#endif //RAG3_ENGINE_SYSTEM_ENGINE_H
