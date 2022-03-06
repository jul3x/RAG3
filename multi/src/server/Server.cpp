//
// Created by jul3x on 16.03.21.
//

#include <R3E/utils/Geometry.h>

#include <common/ResourceManager.h>

#include <common/events/Event.h>
#include <common/misc/JournalEntries.h>

#include <packets/PlayerEventPacket.h>
#include <server/Server.h>


Server::Server() : Framework(), last_packet_timestamp_(0.0f), is_game_finished_(false)
{
}

void Server::initialize()
{
    CFG.set("characters_to_play", CONF<j3x::List>("multi_to_play"));
    CFG.set("settings_tabs", CONF<j3x::List>("server_settings_tabs"));
    CFG.set("maps_order", CONF<j3x::List>("multi_maps_order"));

    // Mockup to load parameters
    RMGET<float>("characters", "henry", "max_health");
    RMSET<float>("characters", "henry", "max_health", CONF<float>("multi_max_health"));

    Framework::initialize();

    ui_ = std::make_unique<MinimalUserInterface>(this);
    ui_->registerCamera(camera_.get());
    engine_->registerUI(ui_.get());
}

void Server::beforeUpdate(float time_elapsed)
{
    Framework::beforeUpdate(time_elapsed);

    if (!is_game_finished_)
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

    if (is_game_finished_)
        return;

    endGameIfNeeded(time_elapsed);
}

void Server::updateCamera(float time_elapsed)
{
    if (!CONF<bool>("draw_server"))
        return;

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
    if (CONF<bool>("draw_server"))
        Framework::draw(graphics);
}

void Server::updatePlayers(float time_elapsed)
{
    for (auto& conn : connections_)
    {
        auto player = conn.second.player_.get();
        if (player != nullptr && player->isAlive() && !player->update(time_elapsed))
        {
            clearPlayer(conn.first, conn.second);

            LOG.info("[Server] Player (" + sf::IpAddress(conn.first).toString() +
                     ") is dead. Death cause: \n" + std::to_string(static_cast<int>(player->getPossibleDeathCause())));
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

            if (connections_.size() >= CONF<int>("server/max_players"))
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
        try {
            auto &socket = conn.second.events_socket_;
            PlayerEventPacket packet;
            auto status = socket->receive(packet);
            switch (status) {
                case sf::Socket::Done: {
                    auto player = conn.second.player_.get();

                    if (player != nullptr) {
                        conn.second.ping_elapsed_ = 0.0f;

                        switch (packet.getType()) {
                            case PlayerEventPacket::Type::UseObject: {
                                useSpecialObject(player, conn.first);
                                break;
                            }
                            case PlayerEventPacket::Type::UseBackpackObject: {
                                player->useItem(j3x::get<std::string>(packet.getParams(), "id"));
                                break;
                            }
                            case PlayerEventPacket::Type::NameChange: {
                                conn.second.name_ = j3x::get<std::string>(packet.getParams(), "name");
                                conn.second.character_ = j3x::get<std::string>(packet.getParams(), "texture");
                                ServerEventPacket server_packet(ServerEventPacket::Type::NameChange,
                                                                packet.getParams(),
                                                                conn.first);
                                sendEventToPlayers(server_packet);
                                break;
                            }
                            case PlayerEventPacket::Type::Message: {
                                ServerEventPacket server_packet(ServerEventPacket::Type::Message,
                                                                packet.getParams(),
                                                                conn.first);
                                sendEventToPlayers(server_packet);
                                break;
                            }
                            case PlayerEventPacket::Type::Respawn: {
                                if (conn.second.player_ != nullptr)
                                    conn.second.player_->setPossibleDeathCause(DeathCause::Unknown, nullptr);
                                clearPlayer(conn.first, conn.second);
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
                            case PlayerEventPacket::Type::Exit: {
                                if (conn.second.player_ != nullptr)
                                    conn.second.player_->setPossibleDeathCause(DeathCause::Unknown, nullptr);
                                clearPlayer(conn.first, conn.second);
                                ServerEventPacket server_packet(ServerEventPacket::Type::PlayerExit,
                                                                0, conn.first);
                                sendEventToPlayers(server_packet);
                                socket->disconnect();
                                to_erase_.emplace_back(conn.first);
                                break;
                            }
                            default: {
                                break;
                            }
                        }
                    } else {
                        if (conn.second.status_ == ConnectionStatus::Off) {
                            LOG.error("This player is not registered!");
                            return;
                        }

                        if (conn.second.status_ == ConnectionStatus::On ||
                            packet.getType() != PlayerEventPacket::Type::Connection)
                            return;

                        j3x::Parameters connection_parameters;
                        auto event_socket = conn.second.events_socket_.get();
                        conn.second.ping_elapsed_ = 0.0f;

                        if (packet.getIntData() == map_->getDigest()) {
                            LOG.info("Connection successful.");
                            respawnPlayer(conn.first);
                            connection_parameters["s"] = static_cast<int>(ConnectionStatus::On);

                            auto server_packet =
                                    ServerEventPacket(ServerEventPacket::Type::Connection, connection_parameters,
                                                      conn.first);

                            event_socket->send(server_packet);
                            conn.second.status_ = ConnectionStatus::On;

                            for (auto &cached_packet: cached_events_) {
                                if (cached_packet.isCachedForIp(conn.first))
                                    event_socket->send(cached_packet);
                            }

                            auto all_stats = j3x::List();
                            for (const auto &connection: connections_) {
                                auto stats = j3x::List(
                                        {static_cast<int>(connection.first), connection.second.stats_.kills_,
                                         connection.second.stats_.deaths_});
                                all_stats.emplace_back(stats);
                            }

                            auto end_packet = ServerEventPacket(
                                    ServerEventPacket::Type::EndOfCachedEvents, {{"stats", std::move(all_stats)}},
                                    conn.first);
                            event_socket->send(end_packet);
                        } else {
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
        catch (std::exception& e) // broad exception because none client should have ability to crash the server
        {
            LOG.error("[Server] Exception occured during handling events from players.");
            LOG.error(e.what());

            clearPlayer(conn.first, conn.second);
            LOG.info("[Server] Player (" + sf::IpAddress(conn.first).toString() +
                     ") has been kicked out because of the server crash attempt");

            ServerEventPacket server_packet(ServerEventPacket::Type::PlayerExit,
                                            0, conn.first);
            sendEventToPlayers(server_packet);
            conn.second.events_socket_->disconnect();
            to_erase_.emplace_back(conn.first);
        }
    }
}

void Server::clearPlayer(sf::Uint32 ip, PlayerConnection& conn)
{
    if (conn.player_ == nullptr)
        return;

    if (conn.player_->isAlive())
    {
        ++conn.stats_.deaths_;
        sf::Uint32 killer_ip = 0;
        auto killer = conn.player_->getPossibleKiller();
        auto cause = conn.player_->getPossibleDeathCause();
        if (killer != nullptr)
        {
            killer_ip = getPlayerIP(killer);
            auto conn_it = connections_.find(killer_ip);

            if (conn_it != connections_.end())
            {
                ++conn_it->second.stats_.kills_;
            }
        }

        ServerEventPacket packet = {ServerEventPacket::Type::PlayerDeath,
                                    {{"k", static_cast<int>(killer_ip)}, {"c", static_cast<int>(cause)}}, ip};
        sendEventToPlayers(packet, false);
    }

    conn.player_->setDead();
    engine_->unregisterObj<DynamicObject>(conn.player_.get());
    unregisterWeapons(conn.player_.get());
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

void Server::sendEventToPlayers(ServerEventPacket& packet, bool cache)
{
    if (cache)
        cached_events_.emplace_back(packet);

    for (auto& conn : connections_)
    {
        conn.second.events_socket_->send(packet);
    }
}

void Server::alertCollision(HoveringObject* h_obj, DynamicObject* d_obj)
{
    auto bullet = dynamic_cast<Bullet*>(h_obj);
    auto player = dynamic_cast<Player*>(d_obj);

    auto factor = 1.0f;

    if (bullet != nullptr && player != nullptr)
    {
        if (bullet->getUser() != player)
        {
            player->getShot(*bullet, factor);
            bullet->setDead();
        }
        return;
    }

    auto special = dynamic_cast<Special*>(h_obj);

    if (special != nullptr && player != nullptr && special->isActive())
    {
        if (special->getActivation() == Functional::Activation::OnEnter)
        {
            const auto& funcs = special->getFunctions();

            // Necessary hack
            bool should_send = false;
            bool should_cache = false;
            for (size_t i = 0; i < funcs.size(); ++i)
            {
                const auto& func = j3x::getObj<std::string>(funcs, i);
                if (func == "Destroy")
                    should_cache = true;

                if (should_cache || func == "Teleport")
                    should_send = true;
            }

            if (should_send)
            {
                auto packet = ServerEventPacket(ServerEventPacket::Type::EnteredObject,
                                                special->getUniqueId(), getPlayerIP(player));
                sendEventToPlayers(packet, should_cache);
            }

            special->use(player);
        }
        else if (special->getActivation() == Functional::Activation::OnUse)
        {
            player->setCurrentSpecialObject(special);
        }
        else
        {
            auto packet = ServerEventPacket(ServerEventPacket::Type::CollectedObject,
                                            special->getUniqueId(), getPlayerIP(player));
            sendEventToPlayers(packet);
            player->addSpecialToBackpack(special->getId(), 1,
                                         [this](Functional* functional) { this->registerFunctions(functional); });
            special_functions_->destroy(special, {}, player);
        }
    }

    auto explosion = dynamic_cast<Explosion*>(h_obj);

    if (player != nullptr && explosion != nullptr)
    {
        explosion->applyForce(player, 1.0f);
    }

    auto fire = dynamic_cast<Fire*>(h_obj);

    if (player != nullptr && fire != nullptr)
    {
        if (fire->getUser() != player)
            player->setGlobalState(Character::GlobalState::OnFire);
    }

    auto melee_weapon_area = dynamic_cast<MeleeWeaponArea*>(h_obj);

    if (player != nullptr && melee_weapon_area != nullptr)
    {
        if (player != melee_weapon_area->getFather()->getUser())
        {
            melee_weapon_area->setActive(false);
            player->getCut(*melee_weapon_area->getFather(), factor);
        }
    }

    auto activation_area = dynamic_cast<ActivationArea*>(h_obj);
    if (activation_area != nullptr && player != nullptr)
    {
        activation_area->setActive(true);
        activation_area->getFather()->setActive(true);
        return;
    }
}

sf::Uint32 Server::getPlayerIP(const Character* player)
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
    engine_->getCollisions().setWindowedCollisionCheck(nullptr, 0.0f);
    cached_events_.clear();
    connections_.clear();
    starting_positions_.clear();
    destroyed_specials_.clear();
    map_->getList<NPC>().clear();

    for (auto& special : map_->getList<Special>())
    {
        if (special->getId() == "starting_position")
            starting_positions_.emplace_back(special->getPosition());
    }

    clearStartingPositions();

    game_start_timestamp_ = 0.0f;
    is_game_finished_ = false;
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
    destroyed_specials_.clear();

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
    data_sender_socket_.setBlocking(false);
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

        if (conn.second.player_ != nullptr)
            conn.second.player_->setPossibleDeathCause(DeathCause::Unknown, nullptr);
        clearPlayer(conn.first, conn.second);
    }

    connections_.clear();
    destroyed_specials_.clear();
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
            if (conn.second.player_ != nullptr)
                conn.second.player_->setPossibleDeathCause(DeathCause::Unknown, nullptr);
            clearPlayer(conn.first, conn.second);
            LOG.info("[Server] Player (" + sf::IpAddress(conn.first).toString() +
                     ") has been kicked out due to the timeout");

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
        static const auto spawn_time = CONF<float>("server/object_spawn_time");

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

const std::unordered_map<sf::Uint32, PlayerConnection>& Server::getConnections() const
{
    return connections_;
}

void Server::endGameIfNeeded(float time_elapsed)
{
    if (!connections_.empty())
        game_start_timestamp_ += time_elapsed;

    bool end = false;
    if (CONF<std::string>("server/end_type") == "time")
    {
        if (game_start_timestamp_ >= CONF<float>("server/end_time"))
            end = true;
    }
    else if (CONF<std::string>("server/end_type") == "kills")
    {
        for (const auto& conn : connections_)
            if (conn.second.stats_.kills_ >= CONF<int>("server/end_kills"))
                end = true;
    }
    else if (CONF<std::string>("server/end_type") == "deaths")
    {
        for (const auto& conn : connections_)
            if (conn.second.stats_.deaths_ >= CONF<int>("server/end_deaths"))
                end = true;
    }

    if (end)
    {
        // Get winners (there may be draws)
        std::vector<std::tuple<sf::Uint32, int, int>> to_sort;
        for (const auto& item : connections_)
        {
            to_sort.emplace_back(item.first, item.second.stats_.kills_, item.second.stats_.deaths_);
        }

        sortPlayersResult(to_sort);

        j3x::List winners;
        winners.push_back(static_cast<int>(std::get<0>(to_sort.front())));

        for (size_t i = 1; i < to_sort.size(); ++i)
        {
            if (std::get<1>(to_sort[i]) != std::get<1>(to_sort[i - 1]) ||
                std::get<2>(to_sort[i]) != std::get<2>(to_sort[i - 1]))
                break;

            winners.push_back(static_cast<int>(std::get<0>(to_sort[i])));
        }

        ServerEventPacket server_packet(ServerEventPacket::Type::GameEnd,
                                        {{"winners", winners}}, 0);
        sendEventToPlayers(server_packet);

        LOG.info("[Server] Player (" + sf::IpAddress(j3x::getObj<int>(winners.front())).toString() +
                 ") and other players (" + utils::toString(winners.size() - 1) + ") wins the game");

        is_game_finished_ = true;
    }
}
