//
// Created by jul3x on 18.12.19.
//

#ifndef RAG3_GAME_INCLUDE_GAME_H
#define RAG3_GAME_INCLUDE_GAME_H

#include <memory>

#include <R3E/system/AbstractGame.h>
#include <R3E/system/Engine.h>
#include <R3E/ai/AgentsManager.h>
#include <R3E/audio/MusicManager.h>
#include <R3E/graphics/Lightning.h>

#include <common/Map.h>
#include <common/Bullet.h>
#include <common/Fire.h>

#include <misc/Camera.h>
#include <misc/Journal.h>
#include <misc/SpecialFunctions.h>
#include <misc/Thought.h>
#include <misc/Achievements.h>
#include <misc/Stats.h>
#include <misc/DebugMapBlockage.h>
#include <misc/Explosion.h>
#include <characters/Player.h>
#include <characters/PlayerClone.h>

#include <ui/UserInterface.h>


using namespace r3e;

class Game : public AbstractGame {

public:
    using SpawningFunction = std::function<void(Character*, const std::string&, const sf::Vector2f&, float)>;

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

    // Engine methods
    void start();
    void initialize() override;
    void update(float time_elapsed) override;
    void draw(graphics::Graphics& graphics) override;

    void alertCollision(HoveringObject* h_obj, StaticObject* s_obj) override;
    void alertCollision(HoveringObject* h_obj, DynamicObject* d_obj) override;
    void alertCollision(DynamicObject* d_obj, StaticObject* s_obj) override;
    void alertCollision(DynamicObject* d_obj_1, DynamicObject* d_obj_2) override;

    // Getters
    Map& getMap();
    Player& getPlayer();
    Camera& getCamera();
    Stats& getStats();
    UserInterface& getUI();
    [[nodiscard]] Journal& getJournal() const;
    [[nodiscard]] const std::list<std::unique_ptr<Bullet>>& getBullets() const;
    [[nodiscard]] const std::list<std::unique_ptr<Fire>>& getFires() const;
    [[nodiscard]] Special* getCurrentSpecialObject() const;
    [[nodiscard]] Character* getCurrentTalkableCharacter() const;
    [[nodiscard]] const SpawningFunction& getSpawningFunction(const std::string& name) const;
    [[nodiscard]] float getRag3Time() const;

    // Spawn events
    void spawnSparksEvent(const sf::Vector2f& pos, float dir, float r);
    void spawnShotEvent(const std::string& name, const sf::Vector2f& pos, float dir);
    void spawnBloodEvent(const sf::Vector2f& pos, float dir);
    void spawnExplosionEvent(const sf::Vector2f& pos, float r);
    void spawnTeleportationEvent(const sf::Vector2f& pos);
    void spawnEvent(const std::string& name, const sf::Vector2f& pos, float dir = 0.0f, float r = 0.0f);
    void spawnSwirlEvent(const std::string& name, const sf::Vector2f& pos, bool flipped);
    void spawnExplosionForce(const sf::Vector2f& pos, float r);
    void spawnAchievement(const j3x::Parameters& params);
    void spawnThought(Character* user, const std::string& text);
    void spawnBonusText(const sf::Vector2f& pos, const std::string& text);
    void spawnFadeInOut();

    // Spawn objects for journal
    void spawnNull(Character* user, const std::string& name, const sf::Vector2f& pos, float dir);
    void spawnFire(Character* user, const std::string& name, const sf::Vector2f& pos, float dir);
    void spawnBullet(Character* user, const std::string& name, const sf::Vector2f& pos, float dir);
    void spawnSpecial(const sf::Vector2f& pos, const std::string& name);
    void spawnDecoration(const sf::Vector2f& pos, const std::string& name);

    // UI functions
    void talk();
    void useSpecialObject();
    void setBulletTime();
    void setNormalTime();
    void setRag3Time(float time_elapsed);

    // Journal methods
    [[nodiscard]] bool isJournalFreezed() const;

    void findAndDeleteBullet(Bullet* ptr);
    void findAndDeleteFire(Fire* ptr);
    void findAndDeleteDecoration(Decoration* ptr);
    void findAndDeleteSpecial(Special* ptr);

    Special* spawnNewSpecial(const std::string& id, int u_id, const sf::Vector2f& pos, Functional::Activation activation,
                             const j3x::List& funcs, const j3x::List& datas);
    Fire* spawnNewFire(Character* user, const sf::Vector2f& pos, float dir);
    Bullet* spawnNewBullet(Character* user, const std::string& id, const sf::Vector2f& pos, float dir);
    Obstacle* spawnNewObstacle(const std::string& id, int u_id, const sf::Vector2f& pos, Functional::Activation activation,
                               const j3x::List& funcs, const j3x::List& datas);
    ObstacleTile* spawnNewObstacleTile(const std::string& id, const sf::Vector2f& pos);
    Decoration* spawnNewDecoration(const std::string& id, int u_id, const sf::Vector2f& pos);

    NPC* spawnNewNPC(const std::string &id, int u_id, Functional::Activation activation,
                     const j3x::List& funcs, const j3x::List& datas);
    NPC* spawnNewPlayerClone(const std::string &weapon_id);

    // Registering methods
    void registerWeapons(Character* character);
    void registerWeapon(AbstractWeapon* weapon);
    void registerFunctions(Functional* functional) const;
    void registerLight(Lightable* light) const;

    // State methods
    void setGameState(Game::GameState state);
    [[nodiscard]] Game::GameState getGameState() const;

private:
    Game();

    void updateMapObjects(float time_elapsed);
    void updatePlayerClone(float time_elapsed);
    void updatePlayer(float time_elapsed);
    void updateBullets(float time_elapsed);
    void updateFire(float time_elapsed);
    void updateExplosions();

    void killNPC(NPC* npc);
    void cleanPlayerClone();

    std::unique_ptr<Engine> engine_;
    std::unique_ptr<UserInterface> ui_;
    std::unique_ptr<Camera> camera_;
    std::unique_ptr<ai::AgentsManager> agents_manager_;
    std::unique_ptr<audio::MusicManager> music_manager_;
    std::unique_ptr<graphics::Lightning> lightning_;

    std::unique_ptr<SpecialFunctions> special_functions_;
    std::unique_ptr<Achievements> achievements_;
    std::unique_ptr<Stats> stats_;
    std::unique_ptr<Journal> journal_;
    std::unique_ptr<Player> player_;
    std::unique_ptr<PlayerClone> player_clone_;
    std::unique_ptr<Map> map_;
    std::unique_ptr<DebugMapBlockage> debug_map_blockage_;

    std::list<std::unique_ptr<Bullet>> bullets_;
    std::list<std::unique_ptr<Fire>> fire_;
    std::list<std::unique_ptr<Explosion>> explosions_;
    std::list<std::pair<sf::Vector2f, float>> desired_explosions_;

    std::unordered_map<std::string, SpawningFunction> spawning_func_;

    Game::GameState state_;

    float time_elapsed_;
    float rag3_time_elapsed_;
    float current_time_factor_;

};


#endif //RAG3_GAME_INCLUDE_GAME_H
