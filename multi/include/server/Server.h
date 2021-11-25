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
#include <packets/ServerEventPacket.h>
#include <server/MinimalUserInterface.h>


using namespace r3e;

class Server : public Framework {

public:
    Server();
    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;

    // Engine methods
    void initialize() override;
    void draw(graphics::Graphics& graphics) override;
    void close() override;

    void alertCollision(HoveringObject* h_obj, DynamicObject* d_obj) override;

    // UI functions
    void respawn(const std::string& map_name) override;
    void useSpecialObject() override;
    void setGameState(GameState state) override;
    void startGame(const std::string& map_name) override;
    void disconnect();

private:
    static constexpr auto MAX_PLAYERS = 8;

    void preupdate(float time_elapsed) override;
    void beforeUpdate(float time_elapsed) override;
    void afterUpdate(float time_elapsed) override;

    void updateCamera(float time_elapsed) override;
    void updatePlayers(float time_elapsed) override;

    void obstacleDestroyedEvent(Obstacle* obstacle) override;
    void drawAdditionalPlayers(graphics::Graphics& graphics) override;
    void drawAdditionalPlayersLighting() override;
    void clearPlayer(Player* player);
    void respawnPlayer(sf::Uint32 ip);
    void useSpecialObject(Player* player, sf::Uint32 ip);

    sf::Uint32 getPlayerIP(Player* player);
    void checkAwaitingConnections();
    void handleMessagesFromPlayers();
    void handleEventsFromPlayers();
    void sendMessagesToPlayers();
    void sendEventToPlayers(ServerEventPacket& packet);
    void handleTimeouts(float time_elapsed);
    void handleToErase();

    std::unordered_map<sf::Uint32, PlayerConnection> connections_;
    std::vector<ServerEventPacket> cached_events_;
    std::vector<sf::Uint32> to_erase_;

    std::vector<sf::Vector2f> starting_positions_;

    sf::TcpListener connection_listener_;
    sf::UdpSocket data_receiver_socket_;
    sf::UdpSocket data_sender_socket_;

    sf::IpAddress local_ip_;

    float last_packet_timestamp_;

};

#endif //RAG3_MULTI_INCLUDE_SERVER_H
