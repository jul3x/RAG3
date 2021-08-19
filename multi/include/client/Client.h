//
// Created by jul3x on 16.03.21.
//

#ifndef RAG3_MULTI_INCLUDE_CLIENT_H
#define RAG3_MULTI_INCLUDE_CLIENT_H

#include <memory>

#include <SFML/Network.hpp>

#include <R3E/system/AbstractGame.h>
#include <R3E/system/Engine.h>
#include <R3E/graphics/Lighting.h>

#include <common/Map.h>
#include <common/weapons/Bullet.h>
#include <common/weapons/Fire.h>
#include <common/misc/Camera.h>
#include <common/misc/SpecialFunctions.h>
#include <common/misc/Explosion.h>
#include <common/misc/Destruction.h>
#include <common/characters/Player.h>
#include <common/Framework.h>

#include <packets/PlayersStatePacket.h>
#include <client/ClientUserInterface.h>


using namespace r3e;

class Client : public Framework {

public:
    Client();
    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;

    // Engine methods
    void initialize() override;
    void update(float time_elapsed) override;
    void draw(graphics::Graphics& graphics) override;
    void close() override;

    void alertCollision(HoveringObject* h_obj, StaticObject* s_obj) override;
    void alertCollision(HoveringObject* h_obj, DynamicObject* d_obj) override;

    // Getters
    [[nodiscard]] Player* getPlayer() override;
    [[nodiscard]] Special* getCurrentSpecialObject() const override;

    // UI functions
    void useItem(const std::string& id) override;
    void useSpecialObject() override;
    void setGameState(GameState state) override;

private:
    void updatePlayers(float time_elapsed);

    void initPlayers() override;

    void establishConnection(const sf::IpAddress& ip);
    void handleEventsFromServer();
    void sendInputs();
    void receiveData();
    Player* getPlayer(sf::Uint32 ip);

    std::unordered_map<sf::Uint32, std::unique_ptr<Player>> players_;
    std::unordered_map<sf::Uint32, PlayerData> cached_datas_;
    std::unique_ptr<Player> player_;

    sf::UdpSocket data_send_socket_;
    sf::UdpSocket data_receive_socket_;
    sf::TcpSocket events_socket_;
    sf::IpAddress server_ip_;

    float last_packet_timestamp_;
    uint64_t last_received_packet_timestamp_;

};


#endif //RAG3_GAME_INCLUDE_GAME_H
