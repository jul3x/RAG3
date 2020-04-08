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

#include <common/Map.h>
#include <common/Bullet.h>

#include <misc/Camera.h>
#include <misc/Journal.h>
#include <misc/SpecialFunctions.h>
#include <misc/Thought.h>
#include <misc/Achievements.h>
#include <misc/Stats.h>
#include <characters/Player.h>
#include <characters/PlayerClone.h>

#include <ui/UserInterface.h>


using namespace r3e;

class Game : public AbstractGame {

public:
    enum class GameState
    {
        Paused,
        Normal,
        Reverse
    };

    Game(const Game&) = delete;

    Game& operator=(const Game&) = delete;

    static Game& get()
    {
        static Game instance;
        return instance;
    }

    const sf::Vector2f& getPlayerPosition() const;

    Map& getMap();

    Player& getPlayer();

    Stats& getStats();

    const Journal& getJournal() const;

    const std::list<std::unique_ptr<Bullet>>& getBullets() const;

    const ai::MapBlockage& getMapBlockage() const;

    ai::AgentsManager& getAgentsManager() const;

    void spawnBullet(const std::string& name, const sf::Vector2f& pos, float dir);

    void spawnSparksEvent(const sf::Vector2f& pos, float dir, float r);

    void spawnShotEvent(const std::string& name, const sf::Vector2f& pos, const float dir);

    void spawnExplosionEvent(const sf::Vector2f& pos, float r);

    void spawnTeleportationEvent(const sf::Vector2f& pos);

    void spawnThought(const std::string& text);

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

    NPC* spawnNewNPC(const std::string &id);

    NPC* spawnNewPlayerClone();

    void cleanPlayerClone();

    Bullet* spawnNewBullet(const std::string& id, const sf::Vector2f& pos, float dir);

    Obstacle* spawnNewObstacle(const std::string& id, const sf::Vector2f& pos);

    ObstacleTile* spawnNewObstacleTile(const std::string& id, const sf::Vector2f& pos);

    void findAndDeleteBullet(Bullet* ptr);

    void findAndDeleteDecoration(Decoration* ptr);

    Special* getCurrentSpecialObject() const;

    void useSpecialObject();

    void setBulletTime();

    void setNormalTime();

    bool isJournalFreezed() const;

    void setGameState(Game::GameState state);

    Game::GameState getGameState() const;

    float getCurrentTimeFactor() const;

    float getFPS() const;

    void start();

private:
    Game();

    void updateMapObjects(float time_elapsed);

    void killNPC(NPC* npc);

    static constexpr float COLLISION_GRID_SIZE_ = 400.0f;

    std::unique_ptr<Engine> engine_;
    std::unique_ptr<UserInterface> ui_;
    std::unique_ptr<Camera> camera_;
    std::unique_ptr<ai::AgentsManager> agents_manager_;
    std::unique_ptr<audio::MusicManager> music_manager_;

    std::unique_ptr<SpecialFunctions> special_functions_;
    std::unique_ptr<Achievements> achievements_;
    std::unique_ptr<Stats> stats_;
    std::unique_ptr<Journal> journal_;
    std::unique_ptr<Player> player_;
    std::unique_ptr<PlayerClone> player_clone_;
    std::unique_ptr<Map> map_;

    std::list<std::unique_ptr<Bullet>> bullets_;
    std::list<std::unique_ptr<Thought>> thoughts_;

    Special* current_special_object_;

    Game::GameState state_;
    float current_time_factor_;

};


#endif //RAG3_GAME_GAME_H
