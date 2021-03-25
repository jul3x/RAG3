//
// Created by jul3x on 16.03.21.
//

#include <R3E/utils/Geometry.h>

#include <common/ResourceManager.h>
#include <common/weapons/MeleeWeapon.h>

#include <common/events/Event.h>
#include <common/misc/JournalEntries.h>

#include <server/Server.h>


Server::Server() : Framework(), last_packet_timestamp_(0.0f)
{
    setGameState(GameState::Normal);
}

void Server::initialize()
{
    Framework::initialize();

    ui_ = std::make_unique<MinimalUserInterface>(this);
    ui_->registerCamera(camera_.get());
    engine_->registerUI(ui_.get());

    for (auto& special : map_->getList<Special>())
    {
        if (special->getId() == "starting_position")
            starting_positions_.emplace_back(special->getPosition());
    }

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
}

void Server::update(float time_elapsed)
{
    checkAwaitingConnections();

    handleMessagesFromPlayers();

    updateExplosions();
    camera_->update(time_elapsed);

    if (!players_.empty())
        camera_->setPointingTo(players_.begin()->second.getPosition());

    camera_->setZoomTo(1.0f);
    updateMapObjects(time_elapsed);
    updatePlayers(time_elapsed);
    updateBullets(time_elapsed);
    updateFire(time_elapsed);

    sendMessagesToPlayers();

    for (auto& player : players_)
    {
        player.second.setCurrentSpecialObject(nullptr);
        player.second.setCurrentTalkableCharacter(nullptr);
    }

    time_elapsed_ += time_elapsed;
}

void Server::draw(graphics::Graphics& graphics)
{
    static sf::RenderStates states;

    sf::Shader* curr_shader = &RM.getShader(j3x::get<std::string>(map_->getParams(), "shader"));
    curr_shader->setUniform("time", this->time_elapsed_);

    states.shader = curr_shader;

    auto draw = [&graphics](auto& list) {
        for (auto& obj : list)
            graphics.drawSorted(*obj);
    };

    for (auto& obj : players_)
        graphics.drawSorted(obj.second);

    draw(map_->getList<DecorationTile>());
    draw(map_->getList<Decoration>());
    draw(map_->getList<Obstacle>());
    draw(map_->getList<ObstacleTile>());
    draw(map_->getList<PlacedWeapon>());
    draw(bullets_);
    draw(fire_);

    for (auto& special : map_->getList<Special>())
        if (special->isDrawable())
            graphics.drawSorted(*special);

    engine_->drawSortedAnimationEvents();
    graphics.drawAlreadySorted(states.shader);
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
        if (player.second.isAlive() && !player.second.update(time_elapsed))
        {
            player.second.setDead();
            engine_->deleteDynamicObject(&player.second);
        }
    }
}

void Server::checkAwaitingConnections()
{
    auto client = std::make_shared<sf::TcpSocket>();
    client->setBlocking(false);
    sf::Socket::Status status = connection_listener_.accept(*client);
    auto ip = client->getRemoteAddress().toString();

    switch (status)
    {
        case sf::Socket::Done:
            events_socket_[ip] = client;

            LOG.info("New connection attempt from: " + ip);
            players_.emplace(ip, starting_positions_
                    .at(utils::num::getRandom(0, static_cast<int>(starting_positions_.size() - 1))));
            engine_->registerDynamicObject(&players_.at(ip));
            registerWeapons(&players_.at(ip));

            // Awful but necessary for now
            players_.at(ip).getLightPoint()->registerGraphics(engine_->getGraphics());
            break;
        case sf::Socket::NotReady:
//            LOG.info("No connection yet.");
            break;
        default:
            LOG.error("Failed connection attempt from: " + ip);
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
                LOG.info("New data received from: " + sender.toString());

                auto player_it = players_.find(sender.toString());

                if (player_it != players_.end())
                {
                    if ((cached_packets_.count(sender.toString()) <= 0 ||
                         packet.getTimestamp() >= cached_packets_[sender.toString()].getTimestamp()) &&
                        utils::timeSinceEpochMillisec() - packet.getTimestamp() < max_ping)
                        cached_packets_[sender.toString()] = packet;
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

            float max_speed;

            if (data.isKey(sf::Keyboard::LShift))
                max_speed = RMGET<float>("characters", "player", "max_running_speed");
            else
                max_speed = RMGET<float>("characters", "player", "max_speed");

            if (data.isKey(sf::Keyboard::A))
            {
                delta.x -= max_speed;
            }
            else if (data.isKey(sf::Keyboard::D))
            {
                delta.x += max_speed;
            }

            if (data.isKey(sf::Keyboard::W))
            {
                delta.y -= max_speed;
            }
            else if (data.isKey(sf::Keyboard::S))
            {
                delta.y += max_speed;
            }

            if (player.second.isAlive())
                player.second.setVelocity(sf::Vector2f{delta.x, delta.y} * player.second.getSpeedFactor());

            player.second.setRotation(data.getRotation());
            player.second.setRotateTo(data.getRotation());

            if (data.isLeftMousePressed())
            {
                player.second.shot();
            }

            player.second.setCurrentWeapon(data.getCurrentWeapon());
        }
        else
        {
            LOG.error("No input packets received from: " + player.first);
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
            if (data_sender_socket_.send(packet, player.first, port) != sf::Socket::Done)
            {
                LOG.error("[Server] Could not send data to client: " + player.first);
            }
        }

        last_packet_timestamp_ = time_elapsed_;
    }
}

void Server::setGameState(Framework::GameState state)
{
    // TODO
    state_ = GameState::Normal;
}
