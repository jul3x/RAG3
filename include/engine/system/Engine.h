//
// Created by jul3x on 27.02.19.
//

#ifndef RAG3_ENGINE_SYSTEM_ENGINE_H
#define RAG3_ENGINE_SYSTEM_ENGINE_H

#include <memory>

#include <engine/graphics/Graphics.h>
#include <game/misc/Camera.h>
#include <engine/graphics/AnimationEvent.h>
#include <game/characters/Player.h>
#include <game/environment/Map.h>
#include <game/weapons/Bullet.h>
#include <game/ui/UserInterface.h>


class Engine {

public:
    Engine(const Engine&) = delete;

    Engine& operator=(const Engine&) = delete;

    static Engine& getInstance()
    {
        static Engine engine_instance;
        return engine_instance;
    }

    void initializeGraphics(const sf::Vector2i& size, const std::string& title, int style);

    void registerUI(AbstractUserInterface* user_interface);

    void registerCamera(AbstractCamera* camera);

    Graphics& getGraphics() const;

    Player& getPlayer();

    void spawnBullet(const std::string& name, const sf::Vector2f& pos, float dir);

    void spawnSparksAnimation(const sf::Vector2f& pos, float dir, float r);

    void spawnShotAnimation(const sf::Vector2f& pos, float dir, float r);

    void spawnExplosionAnimation(const sf::Vector2f& pos, float r);

    void forceCameraShaking();

    void update(int frame_rate);

    static sf::Vector2i detectResolution();

private:
    Engine();

    void ensureConstantFrameRate(int frame_rate);

    void restartClock();

    // Engine components
    std::unique_ptr<Graphics> graphics_;

    // Registered by game
    AbstractUserInterface* ui_;
    AbstractCamera* camera_;
    Player player_;
    Map map_;
    std::list<Bullet> bullets_;

    sf::Clock clock_;
    sf::Time time_;

    std::list<std::unique_ptr<AnimationEvent>> animation_events_;
};


#endif //RAG3_ENGINE_SYSTEM_ENGINE_H
