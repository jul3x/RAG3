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
}

void Server::afterUpdate(float time_elapsed)
{
    Framework::afterUpdate(time_elapsed);
    sendMessagesToPlayers();
    for (auto& player : players_)
    {
        player.second->setCurrentSpecialObject(nullptr);
        player.second->setCurrentTalkableCharacter(nullptr);
    }
}

void Server::updateCamera(float time_elapsed)
{
    Framework::updateCamera(time_elapsed);
    if (!players_.empty())
        camera_->setPointingTo(players_.begin()->second->getPosition());
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
    for (auto& player : players_)
    {
        if (player.second->isAlive() && !player.second->update(time_elapsed))
        {
            clearPlayer(player.second.get());
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

            if (connection_statuses_.count(ip) && connection_statuses_[ip] != ConnectionStatus::Off)
                return;

            events_socket_[ip] = client;

            if (events_socket_.size() >= MAX_PLAYERS)
            {
                connection_parameters["s"] = false;
                connection_parameters["reason"] = std::string("Too many players");
                LOG.info("Too many players... Rejecting connection.");

                auto packet = ServerEventPacket(ServerEventPacket::Type::Connection, connection_parameters, ip);

                // TODO - events_socket_ should be cleared after sending
                events_socket_[ip]->send(packet);
                connection_statuses_[ip] = ConnectionStatus::Off;

                return;
            }

            connection_parameters["s"] = true;
            connection_parameters["map"] = map_->getMapName();
            auto packet = ServerEventPacket(ServerEventPacket::Type::Connection, connection_parameters, ip);
            events_socket_[ip]->send(packet);

            respawnPlayer(ip);

            for (auto& packet : cached_events_)
            {
                if (packet.isCachedForIp(ip))
                    events_socket_[ip]->send(packet);
            }

            connection_statuses_[ip] = ConnectionStatus::On;

            break;
        }
        case sf::Socket::NotReady:
//            LOG.info("No connection yet.");
            break;
        default:
            LOG.error("Failed connection attempt from: " + sf::IpAddress(ip).toString());
            break;
    }
}

void Server::handleMessagesFromPlayers()
{
    PlayerInputPacket packet;
    sf::IpAddress sender;
    unsigned short port;
    bool all_messages = false;

    while (!all_messages)
    {
        sf::Socket::Status status = data_receiver_socket_.receive(packet, sender, port);

        static constexpr auto max_ping = 400;

        switch (status)
        {
            case sf::Socket::Done:
            {
                //LOG.info("New data received from: " + sender.toString());
                auto ip = sender.toInteger();
                auto player_it = players_.find(ip);

                if (player_it != players_.end())
                {
                    if ((cached_packets_.count(ip) <= 0 ||
                         packet.getTimestamp() >= cached_packets_[ip].getTimestamp()) /* &&
                        utils::timeSinceEpochMillisec() - packet.getTimestamp() < max_ping */)
                        cached_packets_[ip] = packet;
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
                LOG.error("Failed connection attempt from: " + sender.toString());
                break;
            }
        }
    }

    for (auto& player : players_)
    {
        auto cached_packet = cached_packets_.find(player.first);

        if (cached_packet != cached_packets_.end())
        {
            auto delta = sf::Vector2f(0.0f, 0.0f);
            auto& data = cached_packet->second;

            UserInterface::applyMovement(player.second.get(), data.getKeys());
            player.second->setRotation(data.getRotation());
            player.second->setRotateTo(data.getRotation());

            if (data.isLeftMousePressed())
            {
                player.second->shot();
            }

            player.second->setCurrentWeapon(data.getCurrentWeapon());
        }
        else
        {
            LOG.error("No input packets received from: " + sf::IpAddress(player.first).toString());
        }
    }
}

void Server::sendMessagesToPlayers()
{
    static constexpr float packet_time_elapsed = 0.04f;
    static unsigned short port = 54002;

    if (time_elapsed_ - last_packet_timestamp_ >= packet_time_elapsed)
    {
        PlayersStatePacket packet(players_, cached_packets_);

        for (const auto& player : players_)
        {
            if (data_sender_socket_.send(packet, sf::IpAddress(player.first), port) != sf::Socket::Done)
            {
                LOG.error("[Server] Could not send data to client: " + sf::IpAddress(player.first).toString());
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
    for (const auto& socket : events_socket_)
    {
        PlayerEventPacket packet;
        auto status = socket.second->receive(packet);
        switch (status)
        {
            case sf::Socket::Done:
            {
                auto player_it = players_.find(socket.first);

                if (player_it != players_.end())
                {
                    switch (packet.getType())
                    {
                        case PlayerEventPacket::Type::UseObject:
                        {
                            useSpecialObject(player_it->second.get(), player_it->first);
                            break;
                        }
                        case PlayerEventPacket::Type::UseBackpackObject:
                        {
                            player_it->second->useItem(j3x::get<std::string>(packet.getParams(), "id"));
                            break;
                        }
                        case PlayerEventPacket::Type::NameChange:
                        {
                            ServerEventPacket server_packet(ServerEventPacket::Type::NameChange,
                                                            packet.getParams(),
                                                            player_it->first);
                            sendEventToPlayers(server_packet);
                            break;
                        }
                        case PlayerEventPacket::Type::Exit:
                        {
                            clearPlayer(player_it->second.get());
                            players_.erase(player_it);
                            events_socket_[player_it->first]->disconnect();
                            connection_statuses_.erase(player_it->first);
                            events_socket_.erase(player_it->first);

                            ServerEventPacket server_packet(ServerEventPacket::Type::PlayerExit,
                                                            0, player_it->first);
                            sendEventToPlayers(server_packet);
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
                    LOG.error("This player is not registered!");
                }
                break;
            }
            case sf::Socket::NotReady:
                break;
            default:
                LOG.error("Failed connection attempt from: " + sf::IpAddress(socket.first).toString());
                break;
        }
    }
}

void Server::clearPlayer(Player* player)
{
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

    for (auto& socket : events_socket_)
    {
        socket.second->send(packet);
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
    for (const auto& p : players_)
    {
        if (p.second.get() == player)
        {
            return p.first;
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
    players_[ip] = std::make_unique<Player>(starting_positions_.at(utils::num::getRandom(0, static_cast<int>(
            starting_positions_.size() - 1))));
    initPlayer(players_.at(ip).get());
}

void Server::respawn(const std::string& map_name)
{
    Framework::respawn(map_name);
    cached_events_.clear();
    cached_packets_.clear();
    starting_positions_.clear();
    map_->getList<NPC>().clear();

    for (auto& special : map_->getList<Special>())
    {
        if (special->getId() == "starting_position")
            starting_positions_.emplace_back(special->getPosition());
    }

    utils::eraseIf<std::shared_ptr<Special>>(map_->getList<Special>(), [this](std::shared_ptr<Special>& special) {
        if (special->getId() == "starting_position")
        {
            engine_->unregisterObj<HoveringObject>(special.get());
            return true;
        }
        return false;
    });
}

void Server::drawAdditionalPlayers(graphics::Graphics& graphics)
{
    for (auto& obj : players_)
        graphics.drawSorted(*obj.second);
}

void Server::drawAdditionalPlayersLighting()
{
    for (auto& player : players_)
        lighting_->add(*player.second->getLightPoint());
}

void Server::startGame(const std::string& map_name)
{
    Framework::startGame(map_name);
    this->respawn(map_name);
    events_socket_.clear();
    connection_statuses_.clear();

    for (auto& player : players_)
        unregisterCharacter(player.second.get());
    players_.clear();

    // bind sockets
    if (connection_listener_.listen(54000) != sf::Socket::Done)
    {
        throw std::invalid_argument("[Server] Cannot bind connection socket to desired port!");
    }
    if (data_receiver_socket_.bind(54001) != sf::Socket::Done)
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
    for (auto& event : events_socket_)
        event.second->setBlocking(true);
    auto packet = ServerEventPacket(ServerEventPacket::Type::Exit, 0, 0);
    sendEventToPlayers(packet);

    for (auto& event : events_socket_)
        event.second->disconnect();
    events_socket_.clear();
    connection_statuses_.clear();
    data_receiver_socket_.unbind();
    data_sender_socket_.unbind();
    connection_listener_.close();
}

void Server::close()
{
    disconnect();
    Framework::close();
}
