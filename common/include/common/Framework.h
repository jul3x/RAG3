//
// Created by jul3x on 23.03.21.
//

#ifndef RAG3_COMMON_INCLUDE_FRAMEWORK_H
#define RAG3_COMMON_INCLUDE_FRAMEWORK_H

#include <memory>

#include <R3E/system/AbstractGame.h>
#include <R3E/system/Engine.h>
#include <R3E/graphics/Lighting.h>
#include <R3E/ai/AgentsManager.h>

#include <common/misc/Destruction.h>
#include <common/Map.h>
#include <common/weapons/Bullet.h>
#include <common/weapons/Fire.h>
#include <common/characters/Player.h>
#include <common/characters/PlayerClone.h>
#include <common/misc/Camera.h>
#include <common/misc/SpecialFunctions.h>
#include <common/misc/DebugMapBlockage.h>
#include <common/misc/Explosion.h>
#include <common/ui/UserInterface.h>
#include <common/misc/AbstractStats.h>
#include <common/misc/Thought.h>
#include <common/misc/Weather.h>


using namespace r3e;

class GameUserInterface;

class Journal;

class Framework : public AbstractGame {

public:
    enum class GameState {
        Menu,
        Paused,
        Normal,
        Reverse
    };

    using SpawningFunction = std::function<void(Character*, const std::string&, const sf::Vector2f&, float)>;
    using AnimationSpawningFunction = std::function<void(const std::string&, const sf::Vector2f&, float, bool)>;

    Framework();

    // Engine methods
    void start();
    void initialize() override;
    void update(float time_elapsed) override;
    void draw(graphics::Graphics& graphics) override;
    void close() override;

    void alertCollision(HoveringObject* h_obj, StaticObject* s_obj) override;
    void alertCollision(HoveringObject* h_obj, DynamicObject* d_obj) override;
    void alertCollision(DynamicObject* d_obj, StaticObject* s_obj) override;
    void alertCollision(DynamicObject* d_obj_1, DynamicObject* d_obj_2) override;

    // Getters
    [[nodiscard]] virtual Player* getPlayer();
    [[nodiscard]] virtual Map* getMap();
    [[nodiscard]] virtual Camera* getCamera();
    [[nodiscard]] virtual UserInterface* getUI();
    [[nodiscard]] virtual Journal* getJournal();
    [[nodiscard]] virtual AbstractStats* getStats();
    [[nodiscard]] virtual SpecialFunctions* getSpecialFunctions();
    [[nodiscard]] virtual const std::list<std::unique_ptr<Bullet>>& getBullets() const;
    [[nodiscard]] virtual const std::list<std::unique_ptr<Fire>>& getFires() const;
    [[nodiscard]] virtual Special* getCurrentSpecialObject() const;
    [[nodiscard]] virtual Character* getCurrentTalkableCharacter() const;
    [[nodiscard]] virtual std::tuple<SpawningFunction, AnimationSpawningFunction>
    getSpawningFunction(const std::string& name);
    [[nodiscard]] virtual float getRag3Time() const;
    [[nodiscard]] virtual float getTimeManipulationFuel() const;

    // Spawn events
    virtual DestructionSystem* spawnSparksEvent(const sf::Vector2f& pos, float dir, float r);
    virtual DestructionSystem* spawnSparksEvent2(const sf::Vector2f& pos, float dir, float r);
    virtual void spawnShotEvent(const std::string& name, const sf::Vector2f& pos, float dir);
    virtual DestructionSystem* spawnBloodEvent(const sf::Vector2f& pos, float dir, float deadly_factor);
    virtual void spawnExplosionEvent(const sf::Vector2f& pos);
    virtual void spawnKillEvent(const sf::Vector2f& pos);
    virtual void spawnTeleportationEvent(const sf::Vector2f& pos);
    virtual void spawnEvent(const std::string& name, const sf::Vector2f& pos, float dir = 0.0f, float r = 0.0f);
    virtual void spawnSwirlEvent(const std::string& name, const sf::Vector2f& pos, bool flipped);
    virtual void spawnExplosionForce(const sf::Vector2f& pos, float r);
    virtual void spawnSound(const sf::SoundBuffer& sound, const sf::Vector2f& pos, bool force_pitch = false);
    virtual void spawnFadeInOut();

    // Spawn objects for journal
    virtual void spawnNull(Character* user, const std::string& name, const sf::Vector2f& pos, float dir);
    virtual Fire* spawnFire(Character* user, const std::string& name, const sf::Vector2f& pos, float dir);
    virtual Bullet* spawnBullet(Character* user, const std::string& name, const sf::Vector2f& pos, float dir);
    virtual Special* spawnSpecial(const sf::Vector2f& pos, const std::string& name);
    virtual Decoration* spawnDecoration(const sf::Vector2f& pos, const std::string& name);

    // UI functions
    virtual void useItem(const std::string& id);
    virtual void useSpecialObject();
    virtual void spawnThought(Character* user, const std::string& text);
    virtual void spawnBonusText(const sf::Vector2f& pos, const std::string& text);
    virtual void respawn();

    virtual void findAndDeleteBullet(Bullet* ptr);
    virtual void findAndDeleteFire(Fire* ptr);
    virtual void findAndDeleteDecoration(Decoration* ptr);
    virtual void findAndDeleteSpecial(Special* ptr);
    virtual void findAndDeleteDestructionSystem(DestructionSystem* ptr);

    virtual NPC* spawnNewPlayerClone(const std::string& weapon_id);
    virtual NPC* spawnNewNPC(const std::string& id, int u_id, Functional::Activation activation,
                             const j3x::List& funcs, const j3x::List& datas);
    virtual Special*
    spawnNewSpecial(const std::string& id, int u_id, const sf::Vector2f& pos, Functional::Activation activation,
                    const j3x::List& funcs, const j3x::List& datas);
    virtual Fire* spawnNewFire(Character* user, const sf::Vector2f& pos, float dir);
    virtual Bullet* spawnNewBullet(Character* user, const std::string& id, const sf::Vector2f& pos, float dir);
    virtual Obstacle*
    spawnNewObstacle(const std::string& id, int u_id, const sf::Vector2f& pos, Functional::Activation activation,
                     const j3x::List& funcs, const j3x::List& datas);
    virtual ObstacleTile* spawnNewObstacleTile(const std::string& id, const sf::Vector2f& pos);
    virtual Decoration* spawnNewDecoration(const std::string& id, int u_id, const sf::Vector2f& pos);
    virtual DestructionSystem*
    spawnNewDestructionSystem(const sf::Vector2f& pos, float dir, const DestructionParams& params,
                              float quantity_factor);

    // Registering methods
    virtual void registerWeapons(Character* character);
    virtual void registerWeapon(AbstractWeapon* weapon);
    virtual void registerFunctions(Functional* functional) const;
    virtual void registerLight(Lightable* light) const;

    virtual void setRag3Time(float duration);
    virtual void forceZoomTo(AbstractPhysicalObject* obj);

    // State methods
    virtual void setGameState(Framework::GameState state);
    [[nodiscard]] Framework::GameState getGameState() const;

protected:
    virtual void initParams();

    virtual void updateMapObjects(float time_elapsed);
    virtual void updateBullets(float time_elapsed);
    virtual void updateFire(float time_elapsed);
    virtual void updateExplosions();
    virtual void updateDestructionSystems(float time_elapsed);

    virtual void obstacleDestroyedEvent(Obstacle* obstacle);

    virtual void initObstacles();
    virtual void initDecorations();
    virtual void initWeapons();
    virtual void initNPCs();
    virtual void initSpecials();
    virtual void initPlayers();

    std::unique_ptr<Engine> engine_;
    std::unique_ptr<Camera> camera_;
    std::unique_ptr<graphics::Lighting> lighting_;
    std::unique_ptr<UserInterface> ui_;
    std::unique_ptr<ai::AgentsManager> agents_manager_;

    std::unique_ptr<AbstractStats> stats_;
    std::unique_ptr<SpecialFunctions> special_functions_;
    std::unique_ptr<Map> map_;
    std::unique_ptr<WeatherSystem> weather_;
    std::unique_ptr<DebugMapBlockage> debug_map_blockage_;

    std::list<std::unique_ptr<Bullet>> bullets_;
    std::list<std::unique_ptr<Fire>> fire_;
    std::list<std::unique_ptr<Explosion>> explosions_;
    std::list<std::pair<sf::Vector2f, float>> desired_explosions_;
    std::list<std::unique_ptr<DestructionSystem>> destruction_systems_;

    std::unordered_map<std::string, DestructionParams> destruction_params_;
    std::unordered_map<std::string, SpawningFunction> spawning_func_;
    std::unique_ptr<WeatherParams> weather_params_;

    float time_elapsed_;
    Framework::GameState state_;

};


#endif //RAG3_COMMON_INCLUDE_FRAMEWORK_H
