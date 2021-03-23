//
// Created by jul3x on 16.03.21.
//

#ifndef RAG3_MULTI_INCLUDE_SERVER_H
#define RAG3_MULTI_INCLUDE_SERVER_H

#include <memory>

#include <SFML/Network.hpp>

#include <R3E/system/AbstractGame.h>
#include <R3E/system/Engine.h>
#include <R3E/ai/AgentsManager.h>

#include <common/Map.h>
#include <common/Bullet.h>
#include <common/Fire.h>

#include <misc/Camera.h>
#include <misc/Explosion.h>
#include <characters/Player.h>

#include <packets/PlayerInputPacket.h>
#include <packets/PlayersStatePacket.h>
#include <server/MinimalUserInterface.h>


using namespace r3e;

class Server : public AbstractGame {

public:
    using SpawningFunction = std::function<void(Character*, const std::string&, const sf::Vector2f&, float)>;
    using AnimationSpawningFunction = std::function<void(const std::string&, const sf::Vector2f&, float, bool)>;

    Server(const Server&) = delete;

    Server& operator=(const Server&) = delete;

    static Server& get()
    {
        static Server instance;
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
    [[nodiscard]] const std::list<std::unique_ptr<Bullet>>& getBullets() const;
    [[nodiscard]] const std::list<std::unique_ptr<Fire>>& getFires() const;
    [[nodiscard]] Special* getCurrentSpecialObject() const;
    [[nodiscard]] Character* getCurrentTalkableCharacter() const;
    [[nodiscard]] std::tuple<SpawningFunction, AnimationSpawningFunction> getSpawningFunction(const std::string& name);

    // Spawn events
    void spawnExplosionForce(const sf::Vector2f& pos, float r);

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

    Special* spawnNewSpecial(const std::string& id, int u_id, const sf::Vector2f& pos, Functional::Activation activation,
                             const j3x::List& funcs, const j3x::List& datas);
    Fire* spawnNewFire(Character* user, const sf::Vector2f& pos, float dir);
    Bullet* spawnNewBullet(Character* user, const std::string& id, const sf::Vector2f& pos, float dir);
    Obstacle* spawnNewObstacle(const std::string& id, int u_id, const sf::Vector2f& pos, Functional::Activation activation,
                               const j3x::List& funcs, const j3x::List& datas);
    ObstacleTile* spawnNewObstacleTile(const std::string& id, const sf::Vector2f& pos);
    Decoration* spawnNewDecoration(const std::string& id, int u_id, const sf::Vector2f& pos);

    // Registering methods
    void registerWeapons(Character* character);
    void registerWeapon(AbstractWeapon* weapon);
    void registerFunctions(Functional* functional) const;

private:
    Server();

    void updateMapObjects(float time_elapsed);
    void updatePlayers(float time_elapsed);
    void updateBullets(float time_elapsed);
    void updateFire(float time_elapsed);
    void updateExplosions();

    void checkAwaitingConnections();
    void handleMessagesFromPlayers();
    void sendMessagesToPlayers();

    std::unique_ptr<Engine> engine_;
    std::unique_ptr<Camera> camera_;
    std::unique_ptr<MinimalUserInterface> ui_;

    std::unordered_map<std::string, Player> players_;
    std::unordered_map<std::string, PlayerInputPacket> cached_packets_;

    std::unique_ptr<Map> map_;

    std::list<std::unique_ptr<Bullet>> bullets_;
    std::list<std::unique_ptr<Fire>> fire_;
    std::list<std::unique_ptr<Explosion>> explosions_;
    std::list<std::pair<sf::Vector2f, float>> desired_explosions_;
    std::vector<sf::Vector2f> starting_positions_;

    std::unordered_map<std::string, SpawningFunction> spawning_func_;

    sf::UdpSocket connection_receiver_socket_;
    sf::UdpSocket data_receiver_socket_;
    sf::UdpSocket data_sender_socket_;

    float time_elapsed_;
    float last_packet_timestamp_;

};

#endif //RAG3_MULTI_INCLUDE_SERVER_H
