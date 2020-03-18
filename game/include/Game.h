//
// Created by jul3x on 18.12.19.
//

#ifndef RAG3_GAME_GAME_H
#define RAG3_GAME_GAME_H

#include <memory>

#include <R3E/system/AbstractGame.h>
#include <R3E/system/Engine.h>
#include <R3E/ai/AgentsManager.h>
#include <R3E/audio/MusicManager.h>

#include <misc/Camera.h>
#include <characters/Player.h>
#include <environment/Map.h>
#include <weapons/Bullet.h>
#include <ui/UserInterface.h>


using namespace r3e;

class Game : public AbstractGame {

public:
    Game(const Game&) = delete;

    Game& operator=(const Game&) = delete;

    static Game& get()
    {
        static Game instance;
        return instance;
    }

    const sf::Vector2f& getPlayerPosition() const;

    const ai::MapBlockage& getMapBlockage() const;

    ai::AgentsManager& getAgentsManager() const;

    void spawnBullet(const std::string& name, const sf::Vector2f& pos, float dir);

    void spawnSparksEvent(const sf::Vector2f& pos, float dir, float r);

    void spawnShotEvent(const std::string& name, const sf::Vector2f& pos, const float dir);

    void spawnExplosionEvent(const sf::Vector2f& pos, float r);

    void initialize() override;

    void update(float time_elapsed) override;

    void draw(graphics::Graphics& graphics) override;

    void alertCollision(HoveringObject* h_obj, StaticObject* s_obj) override;

    void alertCollision(HoveringObject* h_obj, DynamicObject* d_obj) override;

    void alertCollision(DynamicObject* d_obj, StaticObject* s_obj) override;

    void alertCollision(DynamicObject* d_obj_1, DynamicObject* d_obj_2) override;

    void deleteStaticObject(StaticObject* s_obj);

    void deleteHoveringObject(HoveringObject* h_obj);

    void deleteDynamicObject(DynamicObject* d_obj);

    void setBulletTime();

    void setNormalTime();

    float getCurrentTimeFactor() const;

    float getFPS() const;

    void start();

private:
    Game();

    static constexpr float COLLISION_GRID_SIZE_ = 400.0f;

    std::unique_ptr<Engine> engine_;
    std::unique_ptr<UserInterface> ui_;
    std::unique_ptr<Camera> camera_;
    std::unique_ptr<ai::AgentsManager> agents_manager_;
    std::unique_ptr<audio::MusicManager> music_manager_;

    std::unique_ptr<Player> player_;
    std::unique_ptr<Map> map_;
    std::list<std::unique_ptr<Bullet>> bullets_;

    float current_time_factor_;

};


#endif //RAG3_GAME_GAME_H