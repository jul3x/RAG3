//
// Created by jul3x on 16.03.21.
//

#include <R3E/utils/Geometry.h>

#include <common/ResourceManager.h>

#include <common/events/Event.h>
#include <common/misc/JournalEntries.h>

#include <packets/PlayerEventPacket.h>
#include <server/Server.h>


Server::Server() : Framework(), last_packet_timestamp_(0.0f)
{
}

void Server::initialize()
{
    Framework::initialize();

    ui_ = std::make_unique<MinimalUserInterface>(this);
    ui_->registerCamera(camera_.get());
    engine_->registerUI(ui_.get());
}

void Server::beforeUpdate(float time_elapsed)
{
    Framework::beforeUpdate(time_elapsed);
    checkAwaitingConnections();
    handleMessagesFromPlayers();
    handleEventsFromPlayers();
    handleTimeouts(time_elapsed);
    handleToErase();
    handlePeriodicalSpecials();
}

void Server::afterUpdate(float time_elapsed)
{
    Framework::afterUpdate(time_elapsed);
    sendMessagesToPlayers();
    for (auto& conn : connections_)
    {
        auto player = conn.second.player_.get();
        if (player != nullptr)
        {
            player->setCurrentSpecialObject(nullptr);
            player->setCurrentTalkableCharacter(nullptr);
        }
    }
}

void Server::updateCamera(float time_elapsed)
{
    Framework::updateCamera(time_elapsed);
    if (!connections_.empty())
    {
        auto player = connections_.begin()->second.player_.get();
        if (player != nullptr)
            camera_->setPointingTo(player->getPosition());
    }
    camera_->setZoomTo(1.0f);
}

void Server::draw(graphics::Graphics& graphics)
{
    static constexpr bool SERVER_DRAWING = true;
    if (SERVER_DRAWING)
        Framework::draw(graphics);
}

void Server::useSpecialObject()
{
    // TODO
//    auto curr = player_->getCurrentSpecialObject();
//    if (curr != nullptr)
//    {
//        curr->use(player_.get());
//    }
}

void Server::updatePlayers(float time_elapsed)
{
    for (auto& conn : connections_)
    {
        auto player = conn.second.player_.get();
        if (player != nullptr && player->isAlive() && !player->update(time_elapsed))
        {
            clearPlayer(player);
        }
    }
}

void Server::checkAwaitingConnections()
{
    auto client = std::make_shared<sf::TcpSocket>();
    client->setBlocking(false);
    sf::Socket::Status status = connection_listener_.accept(*client);
    auto ip = client->getRemoteAddress().toInteger();

    switch (status)
    {
        case sf::Socket::Done:
        {
            LOG.info("New connection attempt from: " + sf::IpAddress(ip).toString());
            j3x::Parameters connection_parameters;

            if (connections_.count(ip) && connections_[ip].status_ != ConnectionStatus::Off)
                return;

            auto[new_connection, placeholder] = connections_.emplace(ip, PlayerConnection());

            new_connection->second.events_socket_ = client;

            if (connections_.size() >= MAX_PLAYERS)
            {
                connection_parameters["s"] = static_cast<int>(ConnectionStatus::Off);
                connection_parameters["reason"] = std::string("Too many players.");
                LOG.info("Too many players... Rejecting connection.");

                auto packet = ServerEventPacket(ServerEventPacket::Type::Connection, connection_parameters, ip);

                new_connection->second.events_socket_->send(packet);
                new_connection->second.status_ = ConnectionStatus::Off;

                return;
            }

            connection_parameters["s"] = static_cast<int>(ConnectionStatus::InProgress);
            connection_parameters["map"] = map_->getMapName();
            connection_parameters["ip"] = static_cast<int>(ip);
            auto packet = ServerEventPacket(ServerEventPacket::Type::Connection, connection_parameters, ip);
            new_connection->second.events_socket_->send(packet);

            new_connection->second.status_ = ConnectionStatus::InProgress;

            break;
        }
        case sf::Socket::NotReady:
//            LOG.info("No connection yet.");
            break;
        default:
            LOG.error("[Server::checkAwaitingConnections] Failed connection attempt from: "
                      + sf::IpAddress(ip).toString() + " [status: " + utils::toString(status) + "]");
            break;
    }
}

void Server::handleMessagesFromPlayers()
{
    sf::IpAddress sender;
    unsigned short port;
    bool all_messages = false;

    while (!all_messages)
    {
        PlayerInputPacket packet;
        sf::Socket::Status status = data_receiver_socket_.receive(packet, sender, port);

        static constexpr auto max_ping = 400;

        switch (status)
        {
            case sf::Socket::Done:
            {
                //LOG.info("New data received from: " + sender.toString());
                auto ip = sender.toInteger();
                auto conn_it = connections_.find(ip);

                if (conn_it != connections_.end() && conn_it->second.status_ == ConnectionStatus::On)
                {
                    conn_it->second.ping_elapsed_ = 0.0f;
                    if ((packet.getTimestamp() >= conn_it->second.cached_packet_.getTimestamp()) /* &&
                        utils::timeSinceEpochMillisec() - packet.getTimestamp() < max_ping */)
                        conn_it->second.cached_packet_ = packet;
                    else
                        LOG.error("This packet is old or latency is too high!"
                                  "\nPacket timestamp difference: " +
                                  std::to_string(utils::timeSinceEpochMillisec() - packet.getTimestamp()));
                }
                else
                {
                    LOG.error("This player is not registered!");
                }
                break;
            }
            case sf::Socket::NotReady:
            {
                all_messages = true;
                break;
            }
            default:
            {
                all_messages = true;
                LOG.error("[Server::handleMessagesFromPlayer] Failed connection attempt from: "
                          + sender.toString() + " [status: " + utils::toString(status) + "]");
                break;
            }
        }
    }

    for (auto& conn : connections_)
    {
        auto& cached_packet = conn.second.cached_packet_;

        auto& data = cached_packet;
        auto player = conn.second.player_.get();

        if (player != nullptr)
        {
            UserInterface::applyMovement(player, data.getKeys());
            player->setRotation(data.getRotation());
            player->setRotateTo(data.getRotation());

            if (data.isLeftMousePressed())
            {
                player->shot();
            }

            player->setCurrentWeapon(data.getCurrentWeapon());
        }
    }
}

void Server::sendMessagesToPlayers()
{
    static constexpr float packet_time_elapsed = 0.04f;
    static unsigned short port = CONF<int>("udp_server_port");

    if (time_elapsed_ - last_packet_timestamp_ >= packet_time_elapsed)
    {
        PlayersStatePacket packet(connections_);

        for (const auto& conn : connections_)
        {
            if (data_sender_socket_.send(packet, sf::IpAddress(conn.first), port) != sf::Socket::Done)
            {
                LOG.error("[Server] Could not send data to client: " + sf::IpAddress(conn.first).toString());
            }
        }

        last_packet_timestamp_ = time_elapsed_;
    }
}

void Server::setGameState(Framework::GameState state)
{
    switch (state)
    {
        case GameState::Reverse:
        case GameState::Paused:
            state = state_;
            break;
        default:
            break;
    }

    state_ = state;
}

void Server::handleEventsFromPlayers()
{
    for (auto& conn : connections_)
    {
        auto& socket = conn.second.events_socket_;
        PlayerEventPacket packet;
        auto status = socket->receive(packet);
        switch (status)
        {
            case sf::Socket::Done:
            {
                auto player = conn.second.player_.get();

                if (player != nullptr)
                {
                    conn.second.ping_elapsed_ = 0.0f;

                    switch (packet.getType())
                    {
                        case PlayerEventPacket::Type::UseObject:
                        {
                            useSpecialObject(player, conn.first);
                            break;
                        }
                        case PlayerEventPacket::Type::UseBackpackObject:
                        {
                            player->useItem(j3x::get<std::string>(packet.getParams(), "id"));
                            break;
                        }
                        case PlayerEventPacket::Type::NameChange:
                        {
                            conn.second.name_ = j3x::get<std::string>(packet.getParams(), "name");
                            conn.second.character_ = j3x::get<std::string>(packet.getParams(), "texture");
                            ServerEventPacket server_packet(ServerEventPacket::Type::NameChange,
                                                            packet.getParams(),
                                                            conn.first);
                            sendEventToPlayers(server_packet);
                            break;
                        }
                        case PlayerEventPacket::Type::Respawn:
                        {
                            clearPlayer(player);
                            respawnPlayer(conn.first);

                            ServerEventPacket server_packet(ServerEventPacket::Type::PlayerRespawn,
                                                            0, conn.first);
                            sendEventToPlayers(server_packet);

                            j3x::Parameters data = {{"name",    conn.second.name_},
                                                    {"texture", conn.second.character_}};
                            ServerEventPacket change_name_packet(ServerEventPacket::Type::NameChange,
                                                                 data,
                                                                 conn.first);
                            sendEventToPlayers(change_name_packet);
                            break;
                        }
                        case PlayerEventPacket::Type::Exit:
                        {
                            clearPlayer(player);
                            ServerEventPacket server_packet(ServerEventPacket::Type::PlayerExit,
                                                            0, conn.first);
                            sendEventToPlayers(server_packet);
                            socket->disconnect();
                            to_erase_.emplace_back(conn.first);
                            break;
                        }
                        default:
                        {
                            break;
                        }
                    }
                }
                else
                {
                    if (conn.second.status_ == ConnectionStatus::Off)
                    {
                        LOG.error("This player is not registered!");
                        return;
                    }

                    if (conn.second.status_ == ConnectionStatus::On ||
                        packet.getType() != PlayerEventPacket::Type::Connection)
                        return;

                    j3x::Parameters connection_parameters;
                    auto event_socket = conn.second.events_socket_.get();
                    conn.second.ping_elapsed_ = 0.0f;

                    if (packet.getIntData() == map_->getDigest())
                    {
                        LOG.info("Connection successful.");
                        respawnPlayer(conn.first);
                        connection_parameters["s"] = static_cast<int>(ConnectionStatus::On);
                        auto server_packet =
                                ServerEventPacket(ServerEventPacket::Type::Connection, connection_parameters,
                                                  conn.first);

                        event_socket->send(server_packet);
                        conn.second.status_ = ConnectionStatus::On;

                        for (auto& cached_packet : cached_events_)
                        {
                            if (cached_packet.isCachedForIp(conn.first))
                                event_socket->send(cached_packet);
                        }
                        auto end_packet = ServerEventPacket(
                                ServerEventPacket::Type::EndOfCachedEvents, {},
                                conn.first);
                        event_socket->send(end_packet);
                    }
                    else
                    {
                        LOG.info("Maps does not match... (" + std::to_string(map_->getDigest()) + ", " +
                                 std::to_string(packet.getIntData()) + ") Rejecting connection.");

                        connection_parameters["s"] = static_cast<int>(ConnectionStatus::Off);
                        connection_parameters["reason"] = std::string("Maps does not match.");
                        auto server_packet =
                                ServerEventPacket(ServerEventPacket::Type::Connection, connection_parameters,
                                                  conn.first);
                        event_socket->send(server_packet);
                        conn.second.status_ = ConnectionStatus::Off;
                    }

                }
                break;
            }
            case sf::Socket::NotReady:
                break;
            default:
                LOG.error("[Server::handleEventsFromPlayers] Failed connection attempt from: "
                          + sf::IpAddress(conn.first).toString() + " [status: " + utils::toString(status) + "]");
                break;
        }
    }
}

void Server::clearPlayer(Player* player)
{
    if (player == nullptr)
        return;

    player->setDead();
    engine_->unregisterObj<DynamicObject>(player);
    unregisterWeapons(player);
}

void Server::useSpecialObject(Player* player, sf::Uint32 ip)
{
    auto obj = player->getCurrentSpecialObject();
    if (obj != nullptr)
    {
        auto packet = ServerEventPacket(ServerEventPacket::Type::UseObject, obj->getUniqueId(), ip);
        sendEventToPlayers(packet);
        obj->use(player);
    }
}

void Server::sendEventToPlayers(ServerEventPacket& packet)
{
    cached_events_.emplace_back(packet);

    for (auto& conn : connections_)
    {
        conn.second.events_socket_->send(packet);
    }
}

void Server::alertCollision(HoveringObject* h_obj, DynamicObject* d_obj)
{
    auto bullet = dynamic_cast<Bullet*>(h_obj);
    auto character = dynamic_cast<Character*>(d_obj);

    auto factor = 1.0f;

    if (bullet != nullptr && character != nullptr)
    {
        if (bullet->getUser() != character)
        {
            character->getShot(*bullet, factor);
            bullet->setDead();
        }
        return;
    }

    auto special = dynamic_cast<Special*>(h_obj);

    if (special != nullptr && character != nullptr && special->isActive())
    {
        if (special->getActivation() == Functional::Activation::OnEnter)
        {
            const auto& funcs = special->getFunctions();

            // Necessary hack
            bool should_send = false;
            for (size_t i = 0; i < funcs.size(); ++i)
                if (j3x::getObj<std::string>(funcs, i) == "Destroy")
                    should_send = true;

            auto player = dynamic_cast<Player*>(d_obj);
            if (should_send)
            {
                auto packet = ServerEventPacket(ServerEventPacket::Type::EnteredObject,
                                                special->getUniqueId(), getPlayerIP(player));
                sendEventToPlayers(packet);
            }

            if (special->isUsableByNPC())
            {
                special->use(character);
            }
            else if (player != nullptr)
            {
                special->use(player);
            }
        }
        else if (special->getActivation() == Functional::Activation::OnUse)
        {
            character->setCurrentSpecialObject(special);
        }
        else
        {
            auto player = dynamic_cast<Player*>(d_obj);
            auto packet = ServerEventPacket(ServerEventPacket::Type::CollectedObject,
                                            special->getUniqueId(), getPlayerIP(player));
            sendEventToPlayers(packet);
            if (player != nullptr)
            {
                player->addSpecialToBackpack(special->getId(), 1,
                                             [this](Functional* functional) { this->registerFunctions(functional); });
                special_functions_->destroy(special, {}, player);
            }
        }
    }

    auto talkable_area = dynamic_cast<TalkableArea*>(h_obj);

    if (talkable_area != nullptr && character != nullptr)
    {
        character->setCurrentTalkableCharacter(talkable_area->getFather());
    }

    auto explosion = dynamic_cast<Explosion*>(h_obj);

    if (character != nullptr && explosion != nullptr)
    {
        explosion->applyForce(character, 1.0f);
    }

    auto fire = dynamic_cast<Fire*>(h_obj);

    if (character != nullptr && fire != nullptr)
    {
        if (fire->getUser() != character)
            character->setGlobalState(Character::GlobalState::OnFire);
    }

    auto melee_weapon_area = dynamic_cast<MeleeWeaponArea*>(h_obj);

    if (character != nullptr && melee_weapon_area != nullptr)
    {
        if (character != melee_weapon_area->getFather()->getUser())
        {
            float angle = utils::geo::wrapAngle0_360(std::get<1>(utils::geo::cartesianToPolar(
                    melee_weapon_area->getFather()->getUser()->getPosition() - character->getPosition())) * 180.0 /
                                                     M_PI);
            spawnBloodEvent(character->getPosition() + sf::Vector2f(0.0f, angle > 0 && angle <= 180 ? 5.0 : -5.0),
                            angle, melee_weapon_area->getFather()->getDeadlyFactor());
            melee_weapon_area->setActive(false);
            character->getCut(*melee_weapon_area->getFather(), factor);
        }
    }
}

sf::Uint32 Server::getPlayerIP(Player* player)
{
    for (const auto& conn : connections_)
    {
        if (conn.second.player_.get() == player)
        {
            return conn.first;
        }
    }

    return {};
}

void Server::obstacleDestroyedEvent(Obstacle* obstacle)
{
    auto packet = ServerEventPacket(ServerEventPacket::Type::DestroyedObstacle, obstacle->getUniqueId(), 0);
    sendEventToPlayers(packet);
}

void Server::respawnPlayer(sf::Uint32 ip)
{
    connections_[ip].player_ =
            std::make_unique<Player>(starting_positions_.at(utils::num::getRandom(0, static_cast<int>(
                    starting_positions_.size() - 1))));
    initPlayer(connections_[ip].player_.get());
}

void Server::respawn(const std::string& map_name)
{
    Framework::respawn(map_name);
    cached_events_.clear();
    connections_.clear();
    starting_positions_.clear();
    map_->getList<NPC>().clear();

    for (auto& special : map_->getList<Special>())
    {
        if (special->getId() == "starting_position")
            starting_positions_.emplace_back(special->getPosition());
    }

    clearStartingPositions();
}

void Server::drawAdditionalPlayers(graphics::Graphics& graphics)
{
    for (auto& conn : connections_)
    {
        auto player = conn.second.player_.get();
        if (player != nullptr)
        {
            graphics.drawSorted(*player);
        }
    }
}

void Server::drawAdditionalPlayersLighting()
{
    for (auto& conn : connections_)
    {
        auto player = conn.second.player_.get();
        if (player != nullptr)
        {
            lighting_->add(*player->getLightPoint());
        }
    }
}

void Server::startGame(const std::string& map_name)
{
    Framework::startGame(map_name);
    this->respawn(map_name);

    for (auto& conn : connections_)
        unregisterCharacter(conn.second.player_.get());
    connections_.clear();

    // bind sockets
    if (connection_listener_.listen(CONF<int>("tcp_port")) != sf::Socket::Done)
    {
        throw std::invalid_argument("[Server] Cannot bind connection socket to desired port!");
    }
    if (data_receiver_socket_.bind(CONF<int>("udp_client_port")) != sf::Socket::Done)
    {
        throw std::invalid_argument("[Server] Cannot bind data receiver socket to desired port!");
    }
    connection_listener_.setBlocking(false);
    data_receiver_socket_.setBlocking(false);
    // TODO maybe sender should be non-blocking?
    ui_->startGame();

}

void Server::disconnect()
{
    for (auto& conn : connections_)
        conn.second.events_socket_->setBlocking(true);
    auto packet = ServerEventPacket(ServerEventPacket::Type::Exit, 0, 0);
    sendEventToPlayers(packet);

    for (auto& conn : connections_)
    {
        conn.second.events_socket_->disconnect();
        clearPlayer(conn.second.player_.get());
    }

    connections_.clear();
    data_receiver_socket_.unbind();
    data_sender_socket_.unbind();
    connection_listener_.close();
}

void Server::close()
{
    disconnect();
    Framework::close();
}

void Server::handleTimeouts(float time_elapsed)
{
    static constexpr auto MAX_CLIENT_TIMEOUT = 5.0f;

    for (auto& conn : connections_)
    {
        conn.second.ping_elapsed_ += time_elapsed;
        if (conn.second.ping_elapsed_ > MAX_CLIENT_TIMEOUT)
        {
            clearPlayer(conn.second.player_.get());
            ServerEventPacket server_packet(ServerEventPacket::Type::PlayerExit,
                                            0, conn.first);
            sendEventToPlayers(server_packet);
            conn.second.events_socket_->disconnect();
            to_erase_.emplace_back(conn.first);
        }
    }
}

void Server::handleToErase()
{
    for (auto&& key : to_erase_)
        connections_.erase(key);

    to_erase_.clear();
}

void Server::preupdate(float time_elapsed)
{
}

void Server::addToDestroyedSpecials(const std::string& id, const sf::Vector2f& pos)
{
    destroyed_specials_.emplace_back(id, pos, time_elapsed_);
}

void Server::handlePeriodicalSpecials()
{
    utils::eraseIf<PeriodicalSpecial>(destroyed_specials_, [this](auto& special) {
        static const auto spawn_time = CONF<float>("periodical_spawn_time");

        if (std::get<2>(special) + spawn_time < time_elapsed_)
        {
            auto obj = this->spawnSpecial(std::get<1>(special), std::get<0>(special));
            ServerEventPacket server_packet(ServerEventPacket::Type::SpecialSpawn,
                                            {{"uid", obj->getUniqueId()},
                                             {"id",  std::get<0>(special)},
                                             {"pos", std::get<1>(special)}}, 0);
            sendEventToPlayers(server_packet);
            return true;
        }
        return false;
    });
}
