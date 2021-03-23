//
// Created by jul3x on 16.03.21.
//

#include <R3E/utils/Geometry.h>

#include <common/ResourceManager.h>
#include <common/MeleeWeapon.h>

#include <events/Event.h>
#include <misc/JournalEntries.h>

#include <server/Server.h>



Server::Server() : time_elapsed_(0.0f), last_packet_timestamp_(0.0f)
{
    engine_ = std::make_unique<Engine>();
    engine_->registerGame(this);
}

void Server::initialize()
{
    engine_->initializeGraphics(
            sf::Vector2i{CONF<int>("graphics/window_width_px"), CONF<int>("graphics/window_height_px")},
            "Codename: Rag3",
            CONF<bool>("graphics/full_screen") ? sf::Style::Fullscreen : sf::Style::Default,
            sf::Color(CONF<int>("graphics/background_color")));
    engine_->setFPSLimit(60);

    spawning_func_["bullet"] = [this] (Character* user, const std::string& name, const sf::Vector2f& pos, float dir) { this->spawnBullet(user, name, pos, dir); };
    spawning_func_["fire"] = [this] (Character* user, const std::string& name, const sf::Vector2f& pos, float dir) { this->spawnFire(user, name, pos, dir); };
    spawning_func_["null"] = [this] (Character* user, const std::string& name, const sf::Vector2f& pos, float dir) { this->spawnNull(user, name, pos, dir); };

    // TODO
//    special_functions_ = std::make_unique<SpecialFunctions>();
    camera_ = std::make_unique<Camera>();
    camera_->setViewNormalSize(engine_->getGraphics().getWindow().getView().getSize());

    ui_ = std::make_unique<MinimalUserInterface>();
    engine_->registerUI(ui_.get());
    engine_->registerCamera(camera_.get());

    map_ = std::make_unique<Map>();
    map_->loadMap("map_new");
    engine_->initializeCollisions(map_->getSize(), CONF<float>("collision_grid_size"));

    for (auto& obstacle : map_->getList<ObstacleTile>())
        engine_->registerStaticObject(obstacle.get());

    for (auto& obstacle : map_->getList<Obstacle>())
    {
        engine_->registerStaticObject(obstacle.get());
        registerFunctions(obstacle.get());
    }

    for (auto& weapon : map_->getList<PlacedWeapon>())
    {
        auto func = this->getSpawningFunction(RMGET<std::string>("weapons", weapon->getId(), "spawn_func"));
        weapon->registerSpawningFunction(std::get<0>(func), std::get<1>(func));
    }

    for (auto& special : map_->getList<Special>())
    {
        if (special->getId() == "starting_position")
            starting_positions_.emplace_back(special->getPosition());

        engine_->registerHoveringObject(special.get());
        registerFunctions(special.get());
    }

    // bind sockets
    if (connection_receiver_socket_.bind(54000) != sf::Socket::Done)
    {
        throw std::invalid_argument("[Server] Cannot bind connection socket to desired port!");
    }
    if (data_receiver_socket_.bind(54001) != sf::Socket::Done)
    {
        throw std::invalid_argument("[Server] Cannot bind data receiver socket to desired port!");
    }
    connection_receiver_socket_.setBlocking(false);
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

    time_elapsed_ += time_elapsed;
}

void Server::updateMapObjects(float time_elapsed)
{
    auto& blockage = map_->getMapBlockage();

    auto& obstacles =  map_->getList<Obstacle>();;
    auto& specials = map_->getList<Special>();
    auto& decorations = map_->getList<Decoration>();
    auto& weapons = map_->getList<PlacedWeapon>();

    for (auto it = obstacles.begin(); it != obstacles.end();)
    {
        bool do_increment = true;

        if (!(*it)->update(time_elapsed))
        {
            auto next_it = std::next(it);
            engine_->deleteStaticObject(it->get());

            if ((*it)->getActivation() == Functional::Activation::OnKill)
            {
                // TODO
//                (*it)->use(player_.get());
            }

            Map::markBlocked(blockage.blockage_, (*it)->getPosition() + RMGET<sf::Vector2f>("obstacles", (*it)->getId(), "collision_offset"),
                             RMGET<sf::Vector2f>("obstacles", (*it)->getId(), "collision_size"), 0.0f);

            obstacles.erase(it);
            it = next_it;
            do_increment = false;
        }

        if (do_increment) ++it;
    }

    for (auto it = specials.begin(); it != specials.end();)
    {
        bool do_increment = true;

        if ((*it)->isDestroyed())
        {
            auto next_it = std::next(it);
            engine_->deleteHoveringObject(it->get());

            specials.erase(it);
            it = next_it;
            do_increment = false;
        }

        if (do_increment) ++it;
    }

    for (auto it = decorations.begin(); it != decorations.end();)
    {
        bool do_increment = true;

        if (!(*it)->isActive())
        {
            auto next_it = std::next(it);

            decorations.erase(it);
            it = next_it;
            do_increment = false;
        }

        if (do_increment) ++it;
    }

    for (auto& weapon : weapons)
    {
        weapon->update(time_elapsed);
    }
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

void Server::start()
{
    engine_->start();
}

Map& Server::getMap()
{
    return *map_;
}

const std::list<std::unique_ptr<Bullet>>& Server::getBullets() const
{
    return bullets_;
}

void Server::spawnBullet(Character* user, const std::string& name, const sf::Vector2f& pos, float dir)
{
    auto ptr = this->spawnNewBullet(user, name, pos, dir);
}

void Server::spawnSpecial(const sf::Vector2f& pos, const std::string& name)
{
    auto ptr = this->spawnNewSpecial(name, -1, pos, Functional::Activation::None, {}, {});

}

void Server::spawnExplosionForce(const sf::Vector2f& pos, float r)
{
    desired_explosions_.emplace_back(pos, r);
}

void Server::updateExplosions()
{
    for (const auto& explosion : explosions_)
    {
        engine_->deleteHoveringObject(explosion.get());
    }
    explosions_.clear();

    for (const auto& desired_explosion : desired_explosions_)
    {
        explosions_.emplace_back(std::make_unique<Explosion>(desired_explosion.first, desired_explosion.second));
        engine_->registerHoveringObject(explosions_.back().get());
    }

    desired_explosions_.clear();
}

void Server::alertCollision(HoveringObject* h_obj, StaticObject* s_obj)
{
    auto bullet = dynamic_cast<Bullet*>(h_obj);
    auto fire = dynamic_cast<Fire*>(h_obj);
    auto explosion = dynamic_cast<Explosion*>(h_obj);
    auto obstacle = dynamic_cast<Obstacle*>(s_obj);
    auto obstacle_tile = dynamic_cast<ObstacleTile*>(s_obj);

    if (bullet != nullptr)
    {
        if (obstacle != nullptr)
        {
            obstacle->getShot(*bullet);
            bullet->setDead();
        }
        else if (obstacle_tile != nullptr)
        {
            bullet->setDead();
        }
    }
    else if (explosion != nullptr)
    {
        if (obstacle != nullptr)
        {
            explosion->applyForce(obstacle);
        }
    }
    else if (fire != nullptr)
    {
        fire->setDead();
    }
}

void Server::alertCollision(HoveringObject* h_obj, DynamicObject* d_obj)
{
    auto bullet = dynamic_cast<Bullet*>(h_obj);
    auto character = dynamic_cast<Character*>(d_obj);

    if (bullet != nullptr && character != nullptr)
    {
        if (bullet->getUser() != character)
        {
            character->getShot(*bullet, 1);
            bullet->setDead();
        }
        return;
    }

    auto special = dynamic_cast<Special*>(h_obj);

    if (special != nullptr && character != nullptr && special->isActive())
    {
        if (special->getActivation() == Functional::Activation::OnEnter)
        {
//            auto player = dynamic_cast<Player*>(d_obj);
            if (special->isUsableByNPC())
            {
                special->use(character);
            }
//            else if (player != nullptr)
//            {
//                special->use(player);
//            }
        }
        else if (special->getActivation() == Functional::Activation::OnUse)
        {
            character->setCurrentSpecialObject(special);
        }
        else
        {
//            auto player = dynamic_cast<Player*>(d_obj);
//            if (player != nullptr)
//            {
//                player->addSpecialToBackpack(special);
                //TODO
//                special_functions_->destroy(special, {}, player);
//            }
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
            melee_weapon_area->setActive(false);
            character->getCut(*melee_weapon_area->getFather(), 1);
        }
    }
}

void Server::alertCollision(DynamicObject* d_obj, StaticObject* s_obj)
{
}

void Server::alertCollision(DynamicObject* d_obj_1, DynamicObject* d_obj_2)
{
}

Bullet* Server::spawnNewBullet(Character* user, const std::string& id, const sf::Vector2f& pos, float dir)
{
    bullets_.emplace_back(std::make_unique<Bullet>(user, pos, id, dir));

    auto ptr = bullets_.back().get();

    registerFunctions(ptr);
    engine_->registerHoveringObject(ptr);

    return ptr;
}

void Server::spawnFire(Character* user, const std::string& name, const sf::Vector2f& pos, float dir)
{
    auto vector = sf::Vector2f{static_cast<float>(std::cos(dir)), static_cast<float>(std::sin(dir))};
    auto ptr = this->spawnNewFire(user, pos + 20.0f * vector, dir);
}

void Server::updateFire(float time_elapsed)
{
    for (auto it = fire_.begin(); it != fire_.end(); ++it)
    {
        if (!(*it)->update(time_elapsed))
        {
            engine_->deleteHoveringObject(it->get());
            auto next_it = std::next(it);
            fire_.erase(it);
            it = next_it;
        }
    }
}

Fire* Server::spawnNewFire(Character* user, const sf::Vector2f& pos, float dir)
{
    fire_.emplace_back(std::make_unique<Fire>(user, pos, dir));

    auto ptr = fire_.back().get();
    engine_->registerHoveringObject(ptr);

    return ptr;
}

ObstacleTile* Server::spawnNewObstacleTile(const std::string& id, const sf::Vector2f& pos)
{
    auto new_ptr = map_->spawn<ObstacleTile>(pos, 0.0f, id);
    engine_->registerStaticObject(new_ptr);
    return new_ptr;
}

Obstacle* Server::spawnNewObstacle(const std::string& id, int u_id, const sf::Vector2f& pos,
                                 Functional::Activation activation,
                                 const j3x::List& funcs, const j3x::List& datas)
{
    auto new_ptr = map_->spawn<Obstacle>(pos, 0.0f, id);

    if (u_id != -1)
    {
        new_ptr->setUniqueId(u_id);
    }

    if (activation != Functional::Activation::None)
    {
        new_ptr->setActivation(activation);
        new_ptr->setFunctions(funcs);
        new_ptr->setDatas(datas);
    }

    engine_->registerStaticObject(new_ptr);
    registerFunctions(new_ptr);

    return new_ptr;
}

void Server::findAndDeleteBullet(Bullet* ptr)
{
    for (auto it = bullets_.rbegin(); it != bullets_.rend(); ++it)
    {
        if (it->get() == ptr)
        {
            engine_->deleteHoveringObject(ptr);
            bullets_.erase((++it).base());
            return;
        }
    }

    LOG.error("[Server] Warning - bullet to delete not found!");
}

void Server::findAndDeleteFire(Fire* ptr)
{
    for (auto it = fire_.rbegin(); it != fire_.rend(); ++it)
    {
        if (it->get() == ptr)
        {
            engine_->deleteHoveringObject(ptr);
            fire_.erase((++it).base());
            return;
        }
    }

    LOG.error("[Server] Warning - fire to delete not found!");
}

void Server::findAndDeleteDecoration(Decoration* ptr)
{
    auto& decorations = map_->getList<Decoration>();
    for (auto it = decorations.rbegin(); it != decorations.rend(); ++it)
    {
        if (it->get() == ptr)
        {
            decorations.erase((++it).base());
            return;
        }
    }

    LOG.error("[Server] Warning - decoration to delete not found!");
}

Special* Server::getCurrentSpecialObject() const
{
    // TODO
//    return player_->getCurrentSpecialObject();
}

Character* Server::getCurrentTalkableCharacter() const
{
    // TODO
//    return player_->getCurrentTalkableCharacter();
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
        player.second.setCurrentSpecialObject(nullptr);
        player.second.setCurrentTalkableCharacter(nullptr);
        if (player.second.isAlive() && !player.second.update(time_elapsed))
        {
            player.second.setDead();
            engine_->deleteDynamicObject(&player.second);
        }
    }
}

void Server::updateBullets(float time_elapsed)
{
    for (auto it = bullets_.begin(); it != bullets_.end(); ++it)
    {
        if (!(*it)->update(time_elapsed))
        {
            if ((*it)->getActivation() == Functional::Activation::OnKill)
            {
                // TODO
//                (*it)->use(player_.get());
            }

            engine_->deleteHoveringObject(it->get());
            auto next_it = std::next(it);
            bullets_.erase(it);
            it = next_it;
        }
    }
}

std::tuple<Server::SpawningFunction, Server::AnimationSpawningFunction> Server::getSpawningFunction(const std::string& name)
{
    static const auto null = [this](const std::string& name, const sf::Vector2f pos, float dir, bool flipped) { this->spawnNull(nullptr, "", pos, 0.0f); };
    auto it = spawning_func_.find(name);

    if (it == spawning_func_.end())
    {
        throw std::invalid_argument("[Server] SpawningFunction " + name + " is not handled!");
    }

    if (name == "bullet")
        return std::make_tuple(it->second, null);
    else if (name == "fire")
        return std::make_tuple(it->second, null);

    return std::make_tuple(it->second, null);
}

void Server::spawnNull(Character* user, const std::string& name, const sf::Vector2f& pos, float dir)
{

}

const std::list<std::unique_ptr<Fire>>& Server::getFires() const
{
    return fire_;
}

Decoration* Server::spawnNewDecoration(const std::string& id, int u_id, const sf::Vector2f& pos)
{
    auto ptr = map_->spawn<Decoration>(pos, 0.0f, id);
    if (u_id != -1)
    {
        ptr->setUniqueId(u_id);
    }

    return ptr;
}

void Server::spawnDecoration(const sf::Vector2f& pos, const std::string& name)
{
    auto ptr = this->spawnNewDecoration(name, -1, pos);
}

Special* Server::spawnNewSpecial(const std::string& id, int u_id,
                               const sf::Vector2f& pos, Functional::Activation activation,
                               const j3x::List& funcs, const j3x::List& datas)
{
    auto ptr = map_->spawn<Special>(pos, 0.0f, id);
    engine_->registerHoveringObject(ptr);

    if (u_id != -1)
    {
        ptr->setUniqueId(u_id);
    }

    if (activation != Functional::Activation::None)
    {
        ptr->setActivation(activation);
        ptr->setFunctions(funcs);
        ptr->setDatas(datas);
    }

    registerFunctions(ptr);

    return ptr;
}

void Server::findAndDeleteSpecial(Special* ptr)
{
    auto& specials = map_->getList<Special>();
    for (auto it = specials.rbegin(); it != specials.rend(); ++it)
    {
        if (it->get() == ptr)
        {
            engine_->deleteHoveringObject(ptr);
            specials.erase((++it).base());
            return;
        }
    }

    LOG.error("[Server] Warning - special to delete not found!");
}

void Server::registerWeapons(Character* character)
{
    for (auto& weapon : character->getWeapons())
    {
        registerWeapon(weapon.get());
    }
}

void Server::registerWeapon(AbstractWeapon* weapon)
{
    if (!weapon->getId().empty() && weapon->getId() != "null")
    {
        auto func = this->getSpawningFunction(RMGET<std::string>("weapons", weapon->getId(), "spawn_func"));
        weapon->registerSpawningFunction(std::get<0>(func), std::get<1>(func));

        auto melee_weapon = dynamic_cast<MeleeWeapon*>(weapon);
        if (melee_weapon != nullptr)
        {
            engine_->registerHoveringObject(melee_weapon->getMeleeWeaponArea());
        }
    }
}

void Server::registerFunctions(Functional* functional) const
{
    for (const auto& function : functional->getFunctions())
    {
        auto& function_str = j3x::getObj<std::string>(function);
        // TODO
//        functional->bindFunction(special_functions_->bindFunction(function_str),
//                                 special_functions_->bindTextToUse(function_str),
//                                 special_functions_->isUsableByNPC(function_str));
    }
}

void Server::close()
{

}

void Server::checkAwaitingConnections()
{
    static constexpr size_t MESSAGE_SIZE = 4;
    size_t received;
    char data[MESSAGE_SIZE];
    sf::IpAddress sender;
    unsigned short port;
    sf::Socket::Status status = connection_receiver_socket_.receive(data, MESSAGE_SIZE, received, sender, port);

    switch (status)
    {
        case sf::Socket::Done:
            LOG.info("New connection attempt from: " + sender.toString());
            players_.emplace(sender.toString(),
                             starting_positions_.at(utils::num::getRandom(0, static_cast<int>(starting_positions_.size() - 1))));
            engine_->registerDynamicObject(&players_.at(sender.toString()));
            registerWeapons(&players_.at(sender.toString()));
            // Awful but necessary for now
            players_.at(sender.toString()).getLightPoint()->registerGraphics(engine_->getGraphics());
            break;
        case sf::Socket::NotReady:
//            LOG.info("No connection yet.");
            break;
        default:
            LOG.error("Failed connection attempt from: " + sender.toString());
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
        switch (status)
        {
            case sf::Socket::Done:
            {
                LOG.info("New data received from: " + sender.toString());

                auto player_it = players_.find(sender.toString());

                if (player_it != players_.end())
                {
                    cached_packets_[sender.toString()] = packet;
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
