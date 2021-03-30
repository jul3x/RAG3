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


Client::Client() : Framework(), last_packet_timestamp_(0.0f), last_received_packet_timestamp_(0)
{
    setGameState(GameState::Normal);
}

void Client::initialize()
{
    player_ = std::make_unique<Player>(sf::Vector2f{0.0f, 0.0f});
    Framework::initialize();

    engine_->initializeSoundManager(CONF<float>("sound/sound_attenuation"));

    ui_ = std::make_unique<ClientUserInterface>(this);

    ui_->registerCamera(camera_.get());
    ui_->registerPlayer(player_.get());
    engine_->registerUI(ui_.get());

    for (auto& special : map_->getList<Special>())
    {
        if (special->getId() == "starting_position")
            player_->setPosition(special->getPosition());
    }

    if (data_receive_socket_.bind(54002) != sf::Socket::Done)
    {
        LOG.error("[Client] Could not bind receiving socket to desired port.");
    }
    data_receive_socket_.setBlocking(false);

    establishConnection("192.168.1.23");
}

void Client::update(float time_elapsed)
{
    sendInputs();
    handleEventsFromServer();
    receiveData();

    if (CONF<bool>("sound/sound_on"))
    {
        Engine::changeSoundListenerPosition(player_->getPosition());
    }

    updateExplosions();
    updateMapObjects(time_elapsed);
    updatePlayers(time_elapsed);
    updateBullets(time_elapsed);
    updateFire(time_elapsed);
    updateDestructionSystems(time_elapsed);

    camera_->update(time_elapsed);
    camera_->setPointingTo(player_->getPosition());
    camera_->setZoomTo(1.0f);

    time_elapsed_ += time_elapsed;
}

void Client::draw(graphics::Graphics& graphics)
{
    static sf::RenderStates states;

    sf::Shader* curr_shader = &RM.getShader(j3x::get<std::string>(map_->getParams(), "shader"));
    curr_shader->setUniform("time", this->time_elapsed_);
    states.shader = curr_shader;

    auto draw = [&graphics](auto& list) {
        for (auto& obj : list)
            graphics.drawSorted(*obj);
    };

    auto draw_light = [this](auto& list) {
        for (const auto& obj : list)
        {
            auto light = obj->getLightPoint();
            if (light != nullptr)
                this->lightning_->add(*light);
        }
    };

    draw(map_->getList<DecorationTile>());
    draw(map_->getList<Decoration>());
    draw(map_->getList<Obstacle>());
    draw(map_->getList<ObstacleTile>());
    draw(map_->getList<PlacedWeapon>());
    draw(destruction_systems_);
    draw(bullets_);
    draw(fire_);

    for (auto& special : map_->getList<Special>())
        if (special->isDrawable())
            graphics.drawSorted(*special);

    if (player_->isAlive())
        graphics.drawSorted(*player_);

    for (auto& player : players_)
    {
        if (player.second->isAlive())
            graphics.drawSorted(*player.second);
    }

    engine_->drawSortedAnimationEvents();

    graphics.drawAlreadySorted(states.shader);

    lightning_->clear();

    draw_light(map_->getList<Obstacle>());
    draw_light(map_->getList<Decoration>());
    draw_light(map_->getList<Special>());
    draw_light(fire_);
    draw_light(engine_->getAnimationEvents());

    lightning_->add(*player_->getLightPoint());
    for (auto& player : players_)
    {
        lightning_->add(*player.second->getLightPoint());
    }

    graphics.setStaticView();
    graphics.draw(*lightning_);
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
    {
        spawnDecoration(player_->getPosition(), "blood");
        spawnKillEvent(player_->getPosition());
        player_->setDead();
        engine_->deleteDynamicObject(player_.get());
    }

    for (auto& player : players_)
    {
        if (player.second->isAlive() && !player.second->update(time_elapsed))
        {
            spawnDecoration(player.second->getPosition(), "blood");
            spawnKillEvent(player.second->getPosition());
            player.second->setDead();
            engine_->deleteDynamicObject(player.second.get());
        }
    }
}

void Client::establishConnection(const sf::IpAddress& ip)
{
    unsigned short port = 54000;

    auto status = events_socket_.connect(ip, port);
    if (status != sf::Socket::Done)
    {
        LOG.error("[Client] Could not establish connection with host: " + ip.toString());
        throw std::runtime_error("No server to connect!");
    }

    events_socket_.setBlocking(false);
    LOG.info("[Client] Connection with host: " + ip.toString() + " successful!");
    server_ip_ = ip;

    j3x::Parameters data;
    data["name"] = std::string("jul3x");
    data["texture"] = std::string("chaingunner");
    PlayerEventPacket packet(PlayerEventPacket::Type::NameChange, data);
    events_socket_.send(packet);
}

void Client::handleEventsFromServer()
{
    ServerEventPacket packet;
    auto status = events_socket_.receive(packet);
    switch (status)
    {
        case sf::Socket::Done:
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
                                obj, [this](Functional* functional) { this->registerFunctions(functional); });
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
                case ServerEventPacket::Type::Exit:
                {
//                    clearPlayer(&player_it->second);
//                    players_.erase(player_it);
                    break;
                }
                default:
                {
                    break;
                }
            }
            break;
        }
        case sf::Socket::NotReady:
            break;
        default:
            LOG.error("Failed connection attempt from: " + server_ip_.toString());
            break;
    }
}

void Client::sendInputs()
{
    static constexpr float packet_time_elapsed = 0.01f;
    static unsigned short port = 54001;

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

            if (sender == server_ip_ && packet.getTimestamp() >= last_received_packet_timestamp_ /*&&
                utils::timeSinceEpochMillisec() - packet.getTimestamp() < max_ping*/)
            {
                for (const auto& data : packet.getDatas())
                {
                    Player* player;
                    if (data.first == sf::IpAddress::getLocalAddress().toInteger())
                    {
                        player = player_.get();
                    }
                    else
                    {
                        player = getPlayer(data.first);
                        player->setRotation(data.second.rotation_);
                        player->setRotateTo(data.second.rotation_);
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
                    player->setCurrentWeapon(data.second.current_weapon_);

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
            LOG.error("Failed connection attempt from: " + sender.toString());
            break;
        }
    }

    for (const auto& data : cached_datas_)
    {
        if (data.first != sf::IpAddress::getLocalAddress().toInteger())
        {
            auto player = getPlayer(data.first);

            if (data.second.is_shooting_)
                player->shot();
        }
    }
}

Player* Client::getPlayer(sf::Uint32 ip)
{
    if (ip == sf::IpAddress::getLocalAddress().toInteger())
        return player_.get();

    auto it = players_.find(ip);

    if (it == players_.end())
    {
        players_[ip] = std::make_unique<Player>(sf::Vector2f{0.0f, 0.0f});
        engine_->registerDynamicObject(players_[ip].get());
        registerLight(players_[ip].get());
        registerWeapons(players_[ip].get());

        return players_[ip].get();
    }

    return it->second.get();
}

void Client::initPlayers()
{
    engine_->registerDynamicObject(player_.get());
    registerLight(player_.get());
    registerWeapons(player_.get());
}

void Client::setGameState(Framework::GameState state)
{
    // TODO
    state_ = GameState::Normal;
}

void Client::close()
{
    PlayerEventPacket packet(PlayerEventPacket::Type::Exit);
    events_socket_.setBlocking(true);
    events_socket_.send(packet);
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

