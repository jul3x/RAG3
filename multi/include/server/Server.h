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

#include <common/Framework.h>
#include <common/characters/Player.h>

#include <packets/PlayerInputPacket.h>
#include <packets/PlayersStatePacket.h>
#include <server/MinimalUserInterface.h>


using namespace r3e;

class Server : public Framework {

public:
    Server();
    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;

    // Engine methods
    void initialize() override;
    void update(float time_elapsed) override;
    void draw(graphics::Graphics& graphics) override;

    void alertCollision(HoveringObject* h_obj, StaticObject* s_obj) override;
    void alertCollision(HoveringObject* h_obj, DynamicObject* d_obj) override;
    void alertCollision(DynamicObject* d_obj, StaticObject* s_obj) override;
    void alertCollision(DynamicObject* d_obj_1, DynamicObject* d_obj_2) override;

    // UI functions
    void useSpecialObject() override;

private:

    void updateMapObjects(float time_elapsed) override;
    void updatePlayers(float time_elapsed);
    void updateBullets(float time_elapsed) override;

    void checkAwaitingConnections();
    void handleMessagesFromPlayers();
    void sendMessagesToPlayers();

    std::unordered_map<std::string, Player> players_;
    std::unordered_map<std::string, PlayerInputPacket> cached_packets_;

    std::vector<sf::Vector2f> starting_positions_;

    sf::TcpListener connection_listener_;
    std::unordered_map<std::string, std::shared_ptr<sf::TcpSocket>> events_socket_;
    sf::UdpSocket data_receiver_socket_;
    sf::UdpSocket data_sender_socket_;

    float last_packet_timestamp_;

};

#endif //RAG3_MULTI_INCLUDE_SERVER_H
