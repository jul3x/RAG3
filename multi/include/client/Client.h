//
// Created by jul3x on 16.03.21.
//

#ifndef RAG3_MULTI_INCLUDE_CLIENT_H
#define RAG3_MULTI_INCLUDE_CLIENT_H

#include <memory>

#include <SFML/Network.hpp>

#include <R3E/system/AbstractGame.h>
#include <R3E/system/Engine.h>
#include <R3E/graphics/Lightning.h>

#include <common/Map.h>
#include <common/Bullet.h>
#include <common/Fire.h>

#include <misc/Camera.h>
#include <misc/SpecialFunctions.h>
#include <misc/Explosion.h>
#include <misc/Destruction.h>
#include <characters/Player.h>

#include <packets/PlayersStatePacket.h>
#include <client/ClientUserInterface.h>


using namespace r3e;

class Client : public AbstractGame {

public:
    using SpawningFunction = std::function<void(Character*, const std::string&, const sf::Vector2f&, float)>;
    using AnimationSpawningFunction = std::function<void(const std::string&, const sf::Vector2f&, float, bool)>;

    Client(const Client&) = delete;

    Client& operator=(const Client&) = delete;

    static Client& get()
    {
        static Client instance;
        return instance;
    }

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
    Map& getMap();
    Player& getPlayer();
    Camera& getCamera();
    [[nodiscard]] const std::list<std::unique_ptr<Bullet>>& getBullets() const;
    [[nodiscard]] const std::list<std::unique_ptr<Fire>>& getFires() const;
    [[nodiscard]] Special* getCurrentSpecialObject() const;
    [[nodiscard]] std::tuple<SpawningFunction, AnimationSpawningFunction> getSpawningFunction(const std::string& name);

    // Spawn events
    void spawnSparksEvent(const sf::Vector2f& pos, float dir, float r);
    void spawnShotEvent(const std::string& name, const sf::Vector2f& pos, float dir);
    void spawnBloodEvent(const sf::Vector2f& pos, float dir, float deadly_factor);
    void spawnExplosionEvent(const sf::Vector2f& pos);
    void spawnKillEvent(const sf::Vector2f& pos);
    void spawnTeleportationEvent(const sf::Vector2f& pos);
    void spawnEvent(const std::string& name, const sf::Vector2f& pos, float dir = 0.0f, float r = 0.0f);
    void spawnSwirlEvent(const std::string& name, const sf::Vector2f& pos, bool flipped);
    void spawnExplosionForce(const sf::Vector2f& pos, float r);
    void spawnFadeInOut();

    // Spawn objects for journal
    void spawnNull(Character* user, const std::string& name, const sf::Vector2f& pos, float dir);
    void spawnFire(Character* user, const std::string& name, const sf::Vector2f& pos, float dir);
    void spawnBullet(Character* user, const std::string& name, const sf::Vector2f& pos, float dir);
    void spawnSpecial(const sf::Vector2f& pos, const std::string& name);
    void spawnDecoration(const sf::Vector2f& pos, const std::string& name);

    // UI functions
    void useSpecialObject();

    void findAndDeleteBullet(Bullet* ptr);
    void findAndDeleteFire(Fire* ptr);
    void findAndDeleteDecoration(Decoration* ptr);
    void findAndDeleteSpecial(Special* ptr);
    void findAndDeleteDestructionSystem(DestructionSystem* ptr);

    Special* spawnNewSpecial(const std::string& id, int u_id, const sf::Vector2f& pos, Functional::Activation activation,
                             const j3x::List& funcs, const j3x::List& datas);
    Fire* spawnNewFire(Character* user, const sf::Vector2f& pos, float dir);
    Bullet* spawnNewBullet(Character* user, const std::string& id, const sf::Vector2f& pos, float dir);
    Obstacle* spawnNewObstacle(const std::string& id, int u_id, const sf::Vector2f& pos, Functional::Activation activation,
                               const j3x::List& funcs, const j3x::List& datas);
    ObstacleTile* spawnNewObstacleTile(const std::string& id, const sf::Vector2f& pos);
    Decoration* spawnNewDecoration(const std::string& id, int u_id, const sf::Vector2f& pos);

    DestructionSystem* spawnNewDestructionSystem(const sf::Vector2f& pos, float dir, const DestructionParams& params, float quantity_factor);

    // Registering methods
    void registerWeapons(Character* character);
    void registerWeapon(AbstractWeapon* weapon);
    void registerFunctions(Functional* functional) const;
    void registerLight(Lightable* light) const;

private:
    Client();

    void initDestructionParams();

    void updateMapObjects(float time_elapsed);
    void updatePlayers(float time_elapsed);
    void updateBullets(float time_elapsed);
    void updateFire(float time_elapsed);
    void updateExplosions();
    void updateDestructionSystems(float time_elapsed);

    void establishConnection(const sf::IpAddress& ip);
    void sendInputs();
    void receiveData();
    Player* getPlayer(const std::string& ip);


    std::unique_ptr<Engine> engine_;
    std::unique_ptr<ClientUserInterface> ui_;
    std::unique_ptr<Camera> camera_;
    std::unique_ptr<graphics::Lightning> lightning_;

    std::unique_ptr<SpecialFunctions> special_functions_;
    std::unordered_map<std::string, std::unique_ptr<Player>> players_;
    std::unordered_map<std::string, PlayerData> cached_datas_;
    std::unique_ptr<Player> player_;
    std::unique_ptr<Map> map_;

    std::list<std::unique_ptr<Bullet>> bullets_;
    std::list<std::unique_ptr<Fire>> fire_;
    std::list<std::unique_ptr<Explosion>> explosions_;
    std::list<std::pair<sf::Vector2f, float>> desired_explosions_;
    std::list<std::unique_ptr<DestructionSystem>> destruction_systems_;

    std::unordered_map<std::string, DestructionParams> destruction_params_;
    std::unordered_map<std::string, SpawningFunction> spawning_func_;

    sf::UdpSocket data_send_socket_;
    sf::UdpSocket data_receive_socket_;
    sf::IpAddress server_ip_;

    float time_elapsed_;
    float last_packet_timestamp_;
    uint64_t last_received_packet_timestamp_;

};


#endif //RAG3_GAME_INCLUDE_GAME_H
