//
// Created by jul3x on 16.03.21.
//

#include <R3E/utils/Geometry.h>
#include <R3E/utils/Misc.h>

#include <common/ResourceManager.h>
#include <common/events/Event.h>
#include <common/misc/JournalEntries.h>

#include <packets/PlayerInputPacket.h>
#include <packets/PlayersStatePacket.h>
#include <packets/PlayerEventPacket.h>
#include <packets/ServerEventPacket.h>
#include <client/Client.h>


Client::Client() : Framework(), last_packet_timestamp_(0.0f), last_received_packet_timestamp_(0),
                   connection_status_(ConnectionStatus::Off)
{
}

void Client::initialize()
{
    player_ = std::make_unique<Player>(sf::Vector2f{0.0f, 0.0f});
    CFG.set("characters_to_play", CONF<j3x::List>("multi_to_play"));

    Framework::initialize();

    ui_ = std::make_unique<ClientUserInterface>(this);
    ui_->registerCamera(camera_.get());
    ui_->registerPlayer(player_.get());
    engine_->registerUI(ui_.get());
}

void Client::preupdate(float time_elapsed)
{
    Framework::preupdate(time_elapsed);
    handleTimeout(time_elapsed);
    sendInputs();
    handleEventsFromServer();
    receiveData();
}

void Client::afterUpdate(float time_elapsed)
{
    Framework::afterUpdate(time_elapsed);
    weather_->update(time_elapsed);
}

void Client::updateCamera(float time_elapsed)
{
    Framework::updateCamera(time_elapsed);
    camera_->setPointingTo(player_->getPosition());
    camera_->setZoomTo(1.0f);
}

Player* Client::getPlayer()
{
    return player_.get();
}

void Client::alertCollision(HoveringObject* h_obj, StaticObject* s_obj)
{
    auto bullet = dynamic_cast<Bullet*>(h_obj);
    auto fire = dynamic_cast<Fire*>(h_obj);
    auto obstacle = dynamic_cast<Obstacle*>(s_obj);
    auto obstacle_tile = dynamic_cast<ObstacleTile*>(s_obj);

    if (bullet != nullptr)
    {
        if (obstacle != nullptr || obstacle_tile != nullptr)
        {
            spawnSparksEvent(bullet->getPosition(), bullet->getRotation() - 90.0f, 0.0f);
            spawnSparksEvent2(bullet->getPosition(), bullet->getRotation() - 90.0f, 0.0f);
            bullet->setDead();
        }
    }
    else if (fire != nullptr)
    {
        fire->setDead();
    }
}

void Client::alertCollision(HoveringObject* h_obj, DynamicObject* d_obj)
{
    auto bullet = dynamic_cast<Bullet*>(h_obj);
    auto character = dynamic_cast<Character*>(d_obj);

    if (bullet != nullptr && character != nullptr)
    {
        if (bullet->getUser() != character)
        {
            float offset = bullet->getRotation() > 0.0f && bullet->getRotation() < 180.0f ? -5.0f : 5.0f;
            spawnBloodEvent(character->getPosition() + sf::Vector2f(0.0f, offset), bullet->getRotation() + 180.0f,
                            bullet->getDeadlyFactor());

            bullet->setDead();
        }
        return;
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
        }
    }
}

Special* Client::getCurrentSpecialObject() const
{
    return player_->getCurrentSpecialObject();
}

void Client::useSpecialObject()
{
    auto curr = player_->getCurrentSpecialObject();
    if (curr != nullptr)
    {
        PlayerEventPacket packet(PlayerEventPacket::Type::UseObject);
        events_socket_.send(packet);
        //curr->use(player_.get());
    }
}

void Client::updatePlayers(float time_elapsed)
{
    if (player_->isAlive() && !player_->update(time_elapsed))
        this->killPlayer(player_.get());

    for (auto& player : players_)
    {
        if (player.second->isAlive() && !player.second->update(time_elapsed))
        {
            this->killPlayer(player.second.get());
        }
    }
}

bool Client::establishConnection(const sf::IpAddress& ip)
{
    static unsigned short port = CONF<int>("tcp_port");

    events_socket_.setBlocking(true);
    auto status = events_socket_.connect(ip, port, sf::seconds(2.0f));
    if (status != sf::Socket::Done)
    {
        LOG.error("[Client] Could not establish connection with host: " + ip.toString()
                  + " [status: " + utils::toString(status) + "]");
        return false;
    }

    events_socket_.setBlocking(false);
    LOG.info("[Client] Connection with host: " + ip.toString() + " successful!");
    server_ip_ = ip;

    connection_status_ = ConnectionStatus::InProgress;
    server_ping_elapsed_ = 0.0f;
    return true;
}

void Client::handleEventsFromServer()
{
    if (connection_status_ == ConnectionStatus::Off)
        return;

    ServerEventPacket packet;
    auto status = events_socket_.receive(packet);
    switch (status)
    {
        case sf::Socket::Done:
        {
            server_ping_elapsed_ = 0.0f;
            if (connection_status_ == ConnectionStatus::InProgress)
            {
                switch (packet.getType())
                {
                    case ServerEventPacket::Type::Connection:
                    {
                        const auto& params = packet.getParams();

                        switch (j3x::get<int>(params, "s"))
                        {
                            case static_cast<int>(ConnectionStatus::On):
                            {
                                j3x::Parameters data;
                                data["name"] = std::string(CONF<std::string>("general/player_name"));
                                data["texture"] = std::string(CONF<std::string>("general/character"));
                                PlayerEventPacket player_packet(PlayerEventPacket::Type::NameChange, data);
                                events_socket_.send(player_packet);

                                ui_->startGame();
                                setGameState(Framework::GameState::Normal);

                                connection_status_ = ConnectionStatus::On;
                                break;
                            }
                            case static_cast<int>(ConnectionStatus::InProgress):
                            {
                                this->respawn(j3x::get<std::string>(params, "map"));
                                ip_on_server_ = sf::IpAddress(static_cast<sf::Uint32>(j3x::get<int>(params, "ip")));
                                setGameState(Framework::GameState::Menu);
                                PlayerEventPacket player_packet(PlayerEventPacket::Type::Connection, map_->getDigest());
                                events_socket_.send(player_packet);
                                break;
                            }
                            case static_cast<int>(ConnectionStatus::Off):
                            {
                                disconnect();
                                ui_->spawnNoteWindow("Server connection not allowed. Reason:\n\"" +
                                                     j3x::get<std::string>(params, "reason") + "\"", false);
                                break;
                            }
                        }

                        break;
                    }
                    default:
                        break;
                }
            }
            else
            {
                switch (packet.getType())
                {
                    case ServerEventPacket::Type::UseObject:
                    case ServerEventPacket::Type::EnteredObject:
                    {
                        auto obj = map_->getObjectById<Special>(packet.getUID());
                        auto player = getPlayer(packet.getIP());
                        if (obj != nullptr && player != nullptr)
                        {
                            obj->use(player);
                        }
                        break;
                    }
                    case ServerEventPacket::Type::CollectedObject:
                    {
                        auto obj = map_->getObjectById<Special>(packet.getUID());
                        auto player = getPlayer(packet.getIP());
                        if (obj != nullptr && player != nullptr)
                        {
                            player->addSpecialToBackpack(
                                    obj->getId(), 1,
                                    [this](Functional* functional) { this->registerFunctions(functional); });
                            spawnSound(RM.getSound("collect"), obj->getPosition());
                            special_functions_->destroy(obj, {}, player);
                        }
                        break;
                    }
                    case ServerEventPacket::Type::DestroyedObstacle:
                    {
                        auto obj = map_->getObjectById<Obstacle>(packet.getUID());
                        if (obj != nullptr)
                        {
                            obj->setHealth(-1.0);
                        }
                        break;
                    }
                    case ServerEventPacket::Type::NameChange:
                    {
                        auto player = getPlayer(packet.getIP());

                        player->makeLifeBar(j3x::get<std::string>(packet.getParams(), "name"));
                        player->changePlayerTexture(j3x::get<std::string>(packet.getParams(), "texture"));
                        break;
                    }
                    case ServerEventPacket::Type::SpecialSpawn:
                    {
                        const auto& pos = j3x::get<sf::Vector2f>(packet.getParams(), "pos");
                        this->spawnSpecial(pos,
                                           j3x::get<std::string>(packet.getParams(), "id"),
                                           j3x::get<int>(packet.getParams(), "uid"));
                        this->spawnEvent("dust", pos);
                        break;
                    }
                    case ServerEventPacket::Type::EndOfCachedEvents:
                    {
                        auto player = getPlayer(packet.getIP());

                        if (player != nullptr)
                        {
                            unregisterWeapons(player);
                            player->setDefaultWeapons();
                            registerWeapons(player);
                            player->getBackpack().clear();
                            ui_->clearThoughts();
                        }

                        break;
                    }
                    case ServerEventPacket::Type::PlayerExit:
                    case ServerEventPacket::Type::PlayerRespawn:
                    {
                        auto player = getPlayer(packet.getIP());

                        if (player != player_.get())
                        {
                            killPlayer(player);

                            auto it = players_.find(packet.getIP());
                            if (it != players_.end())
                            {
                                players_.erase(it);
                            }
                        }
                        else if (packet.getType() == ServerEventPacket::Type::PlayerRespawn) // we are respawning
                        {
                            unregisterCharacter(player_.get());
                            player_ = std::make_unique<Player>(sf::Vector2f{0.0f, 0.0f});
                            ui_->registerPlayer(player_.get());
                            initPlayer(player_.get());
                        }

                        break;
                    }
                    case ServerEventPacket::Type::Exit:
                    {
                        this->disconnect();
                        ui_->openMenu();
                        ui_->spawnNoteWindow("Server connection has been closed.", false);
                        break;
                    }
                    default:
                    {
                        break;
                    }
                }
            }
            break;
        }
        case sf::Socket::NotReady:
            break;
        default:
            if (connection_status_ == ConnectionStatus::InProgress)
            {
                ui_->spawnNoteWindow("Connection with server interrupted with unknown reason.", false);
                connection_status_ = ConnectionStatus::Off;
            }

            LOG.error("[Client::handleEventsFromServer] Failed TCP connection attempt from: "
                      + server_ip_.toString() + " [status: " + utils::toString(status) + "]");
            break;
    }
}

void Client::sendInputs()
{
    if (connection_status_ != ConnectionStatus::On)
        return;

    static constexpr float packet_time_elapsed = 0.01f;
    static unsigned short port = CONF<int>("udp_client_port");

    if (time_elapsed_ - last_packet_timestamp_ >= packet_time_elapsed)
    {
        PlayerInputPacket packet
                (ui_->getKeysPressed(), ui_->isLeftMousePressed(), player_->getRotation(), player_->getCurrentWeapon());

        if (data_send_socket_.send(packet, server_ip_, port) != sf::Socket::Done)
        {
            LOG.error("[Client] Could not send data to host: " + server_ip_.toString());
        }

        last_packet_timestamp_ = time_elapsed_;
    }
}

void Client::receiveData()
{
    if (connection_status_ != ConnectionStatus::On)
        return;

    PlayersStatePacket packet;
    sf::IpAddress sender;
    unsigned short port;
    sf::Socket::Status status = data_receive_socket_.receive(packet, sender, port);
    switch (status)
    {
        case sf::Socket::Done:
        {
            LOG.info("New data received from: " + sender.toString());

            static constexpr auto max_ping = 400;
            server_ping_elapsed_ = 0.0f;

            if (sender == server_ip_ && packet.getTimestamp() >= last_received_packet_timestamp_ /*&&
                utils::timeSinceEpochMillisec() - packet.getTimestamp() < max_ping*/)
            {
                for (const auto& data : packet.getDatas())
                {
                    Player* player;
                    if (isMe(data.first))
                    {
                        player = player_.get();
                    }
                    else
                    {
                        player = getPlayer(data.first);
                        player->setRotation(data.second.rotation_);
                        player->setRotateTo(data.second.rotation_);
                        player->setCurrentWeapon(data.second.current_weapon_);
                    }

                    if (data.second.current_special_id_ != -1)
                        player->setCurrentSpecialObject(
                                map_->getObjectById<Special>(data.second.current_special_id_));
                    else
                        player->setCurrentSpecialObject(nullptr);
                    player->setPosition(data.second.pos_);
                    player->setVelocity(data.second.vel_);
                    player->setForcedVelocity(data.second.vel_);
                    player->setHealth(data.second.health_);
                    player->setGlobalState(data.second.state_);

                    size_t i = 0;
                    for (auto state : data.second.weapon_state_)
                    {
                        player->getWeapons().at(i)->setState(state);
                        ++i;
                    }

                    cached_datas_[data.first] = data.second;
                }
            }
            else
            {
                LOG.error("This server is not registered or packet is old or latency is too high!"
                          "\nPacket timestamp difference: " +
                          std::to_string(utils::timeSinceEpochMillisec() - packet.getTimestamp()));

            }

            last_received_packet_timestamp_ = packet.getTimestamp();
            break;
        }
        case sf::Socket::NotReady:
        {
            break;
        }
        default:
        {
            LOG.error("[Client::receiveData] Failed UDP connection attempt from: "
                      + sender.toString() + " [status: " + utils::toString(status) + "]");

            break;
        }
    }

    for (const auto& data : cached_datas_)
    {
        if (!isMe(data.first))
        {
            auto player = getPlayer(data.first);

            if (data.second.is_shooting_)
                player->shot();
        }
    }
}

Player* Client::getPlayer(sf::Uint32 ip)
{
    if (isMe(ip))
        return player_.get();

    auto it = players_.find(ip);

    if (it == players_.end())
    {
        players_[ip] = std::make_unique<Player>(sf::Vector2f{0.0f, 0.0f});
        initPlayer(players_[ip].get());

        return players_[ip].get();
    }

    return it->second.get();
}

void Client::setGameState(Framework::GameState state)
{
    switch (state)
    {
        case GameState::Normal:
            if (state_ == GameState::Menu)
            {
                camera_->setZoomTo(1.0f);
                camera_->setCenter({player_->getPosition().x, player_->getPosition().y, 0.0f});
            }
            engine_->turnOnCollisions();

            break;
        case GameState::Reverse:
        case GameState::Paused:
            state = state_;
            break;
        default:
            break;
    }

    state_ = state;
}

void Client::close()
{
    disconnect();
    Framework::close();
}

void Client::useItem(const std::string& id)
{
    Framework::useItem(id);
    j3x::Parameters data;
    data["id"] = id;
    auto packet = PlayerEventPacket(PlayerEventPacket::Type::UseBackpackObject, data);
    events_socket_.send(packet);
}

void Client::respawn(const std::string& map_name)
{
    player_ = std::make_unique<Player>(sf::Vector2f{0.0f, 0.0f});
    player_->setName(CONF<std::string>("general/player_name"));
    ui_->registerPlayer(player_.get());

    Framework::respawn(map_name);
    map_->getList<NPC>().clear();
    clearStartingPositions();
}

void Client::drawAdditionalPlayers(graphics::Graphics& graphics)
{
    for (auto& player : players_)
    {
        if (player.second->isAlive())
            graphics.drawSorted(*player.second);
    }
}

void Client::drawAdditionalPlayersLighting()
{
    for (auto& player : players_)
    {
        lighting_->add(*player.second->getLightPoint());
    }
}

void Client::startGame(const std::string& ip_address)
{
    if (connection_status_ != ConnectionStatus::Off)
        return;

    for (auto& player : players_)
        unregisterCharacter(player.second.get());
    players_.clear();
    cached_datas_.clear();
    data_receive_socket_.unbind();
    if (data_receive_socket_.bind(CONF<int>("udp_server_port")) != sf::Socket::Done)
    {
        LOG.error("[Client] Could not bind receiving socket to desired port.");
    }
    data_receive_socket_.setBlocking(false);

    auto connection_result = this->establishConnection(ip_address);

    if (!connection_result)
    {
        ui_->spawnNoteWindow("Could not establish connection with desired host.", false);
    }
}

void Client::disconnect()
{
    for (auto& player : players_)
        unregisterCharacter(player.second.get());
    players_.clear();
    cached_datas_.clear();

    PlayerEventPacket packet(PlayerEventPacket::Type::Exit);
    events_socket_.setBlocking(true);
    events_socket_.send(packet);
    events_socket_.disconnect();

    data_receive_socket_.unbind();
    server_ip_ = {};
    connection_status_ = ConnectionStatus::Off;
}

void Client::handleTimeout(float time_elapsed)
{
    auto MAX_SERVER_TIMEOUT = connection_status_ == ConnectionStatus::InProgress ? 2.0f : 5.0f;

    if (connection_status_ == ConnectionStatus::Off)
        return;

    server_ping_elapsed_ += time_elapsed;

    if (server_ping_elapsed_ > MAX_SERVER_TIMEOUT)
    {
        this->disconnect();
        ui_->openMenu();
        ui_->spawnNoteWindow("Server connection timeouted.", false);
    }
}

void Client::respawnWithoutReload()
{
    PlayerEventPacket player_packet(PlayerEventPacket::Type::Respawn, 0);
    events_socket_.send(player_packet);
}

bool Client::isMe(sf::Uint32 ip)
{
    return ip == ip_on_server_.toInteger();
}
