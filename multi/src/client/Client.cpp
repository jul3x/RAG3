//
// Created by jul3x on 16.03.21.
//

#include <R3E/utils/Geometry.h>
#include <R3E/utils/Misc.h>

#include <common/ResourceManager.h>
#include <common/MeleeWeapon.h>

#include <events/Event.h>
#include <misc/JournalEntries.h>

#include <packets/PlayerInputPacket.h>
#include <packets/PlayersStatePacket.h>
#include <client/Client.h>


Client::Client() : time_elapsed_(0.0f), last_packet_timestamp_(0.0f), last_received_packet_timestamp_(0)
{
    engine_ = std::make_unique<Engine>();
    engine_->registerGame(this);
}

void Client::initialize()
{
    engine_->initializeGraphics(
            sf::Vector2i{CONF<int>("graphics/window_width_px"), CONF<int>("graphics/window_height_px")},
            "Codename: Rag3",
            CONF<bool>("graphics/full_screen") ? sf::Style::Fullscreen : sf::Style::Default,
            sf::Color(CONF<int>("graphics/background_color")));
    engine_->setFPSLimit(60);

    engine_->initializeSoundManager(CONF<float>("sound/sound_attenuation"));

    spawning_func_["bullet"] = [this] (Character* user, const std::string& name, const sf::Vector2f& pos, float dir) { this->spawnBullet(user, name, pos, dir); };
    spawning_func_["fire"] = [this] (Character* user, const std::string& name, const sf::Vector2f& pos, float dir) { this->spawnFire(user, name, pos, dir); };
    spawning_func_["null"] = [this] (Character* user, const std::string& name, const sf::Vector2f& pos, float dir) { this->spawnNull(user, name, pos, dir); };

    player_ = std::make_unique<Player>(sf::Vector2f{0.0f, 0.0f});
    ui_ = std::make_unique<ClientUserInterface>();
    camera_ = std::make_unique<Camera>();
//    special_functions_ = std::make_unique<SpecialFunctions>();

    map_ = std::make_unique<Map>();

    ui_->registerCamera(camera_.get());
    ui_->registerPlayer(player_.get());

    engine_->registerCamera(camera_.get());
    engine_->registerUI(ui_.get());

    this->initDestructionParams();

    map_->loadMap("map_new");
    engine_->getGraphics().setBgColor(sf::Color(j3x::get<int>(map_->getParams(), "background_color")));
    lightning_ = std::make_unique<graphics::Lightning>(sf::Vector2f{static_cast<float>(CONF<int>("graphics/window_width_px")),
                                                                    static_cast<float>(CONF<int>("graphics/window_height_px"))},
                                                       sf::Color(j3x::get<int>(map_->getParams(), "lightning_color")));

    engine_->initializeCollisions(map_->getSize(), CONF<float>("collision_grid_size"));

    for (auto& obstacle : map_->getList<ObstacleTile>())
        engine_->registerStaticObject(obstacle.get());

    for (auto& obstacle : map_->getList<Obstacle>())
    {
        engine_->registerStaticObject(obstacle.get());
        registerLight(obstacle.get());

        registerFunctions(obstacle.get());
    }

    for (auto& decoration : map_->getList<Decoration>())
    {
        registerLight(decoration.get());
    }

    engine_->registerDynamicObject(player_.get());
    registerLight(player_.get());
    registerWeapons(player_.get());

    for (auto& weapon : map_->getList<PlacedWeapon>())
    {
        auto func = this->getSpawningFunction(RMGET<std::string>("weapons", weapon->getId(), "spawn_func"));
        weapon->registerSpawningFunction(std::get<0>(func), std::get<1>(func));
    }

    for (auto& special : map_->getList<Special>())
    {
        if (special->getId() == "starting_position")
            player_->setPosition(special->getPosition());

        registerLight(special.get());
        engine_->registerHoveringObject(special.get());
        registerFunctions(special.get());
    }

    if (data_receive_socket_.bind(54002) != sf::Socket::Done)
    {
        LOG.error("[Client] Could not bind receiving socket to desired port.");
    }
    data_receive_socket_.setBlocking(false);

    establishConnection("192.168.0.14");
}

void Client::update(float time_elapsed)
{
    sendInputs();
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

void Client::updateMapObjects(float time_elapsed)
{
    auto& blockage = map_->getMapBlockage();

    auto& obstacles =  map_->getList<Obstacle>();;
    auto& specials = map_->getList<Special>();
    auto& decorations = map_->getList<Decoration>();
    auto& weapons = map_->getList<PlacedWeapon>();

    for (auto it = obstacles.begin(); it != obstacles.end();)
    {
        bool do_increment = true;
        (*it)->updateAnimation(time_elapsed);

        auto light = (*it)->getLightPoint();

        if (light != nullptr)
        {
            light->setPosition((*it)->getPosition());
            light->update(time_elapsed);
        }

        if (!(*it)->update(time_elapsed))
        {
            this->spawnExplosionEvent((*it)->getPosition());

            auto next_it = std::next(it);
            engine_->deleteStaticObject(it->get());

            if ((*it)->getActivation() == Functional::Activation::OnKill)
            {
                (*it)->use(player_.get());
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
        (*it)->updateAnimation(time_elapsed);

        auto light = (*it)->getLightPoint();

        if (light != nullptr)
        {
            light->setPosition((*it)->getPosition());
            light->update(time_elapsed);
        }

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
        (*it)->updateAnimation(time_elapsed);

        auto light = (*it)->getLightPoint();

        if (light != nullptr)
        {
            light->setPosition((*it)->getPosition());
            light->update(time_elapsed);
        }
        
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

    auto draw_light = [&graphics, this](auto& list) {
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

void Client::start()
{
    engine_->start();
}

Map& Client::getMap()
{
    return *map_;
}

Player& Client::getPlayer()
{
    return *player_;
}

Camera& Client::getCamera()
{
    return *camera_;
}

const std::list<std::unique_ptr<Bullet>>& Client::getBullets() const
{
    return bullets_;
}

void Client::spawnEvent(const std::string& name, const sf::Vector2f& pos, float dir, float r)
{
    auto event = std::make_shared<Event>(pos, name, dir, r);
    engine_->spawnAnimationEvent(event);

    registerLight(event.get());
}

void Client::spawnSparksEvent(const sf::Vector2f& pos, const float dir, const float r)
{
    spawnEvent("sparks", pos, dir, r);
    auto ptr = spawnNewDestructionSystem(pos, dir - 90.0f, destruction_params_["debris"], 1.0f);
}

void Client::spawnExplosionEvent(const sf::Vector2f& pos)
{
    auto number = std::to_string(utils::num::getRandom(1, 1));
    spawnEvent("explosion_" + number, pos, 0.0f, RMGET<sf::Vector2f>("animations", "explosion_" + number, "size").x);

    if (CONF<bool>("sound/sound_on"))
        engine_->spawnSoundEvent(RM.getSound("wall_explosion"), pos);
}

void Client::spawnKillEvent(const sf::Vector2f& pos)
{
    auto number = std::to_string(utils::num::getRandom(1, 1));
    spawnEvent("explosion_" + number, pos, 0.0f, RMGET<sf::Vector2f>("animations", "explosion_" + number, "size").x);

//    if (CONF<bool>("sound/sound_on"))
//        engine_->spawnSoundEvent(RM.getSound("wall_explosion"), pos);
}

void Client::spawnTeleportationEvent(const sf::Vector2f& pos)
{
    spawnEvent("teleportation", pos + sf::Vector2f{0.0f, 10.0f});

    if (CONF<bool>("sound/sound_on"))
        engine_->spawnSoundEvent(RM.getSound("teleportation"), pos);
}

void Client::spawnSwirlEvent(const std::string& name, const sf::Vector2f& pos, bool flipped)
{
    auto event = std::make_shared<Event>(pos, name + "_swirl");
    engine_->spawnAnimationEvent(event);

    if (flipped)
        event->setFlipX(true);

    registerLight(event.get());
}

void Client::spawnFadeInOut()
{
    engine_->spawnEffect(std::make_shared<graphics::FadeInOut>(
            sf::Vector2f{static_cast<float>(CONF<int>("graphics/window_width_px")),
                         static_cast<float>(CONF<int>("graphics/window_height_px"))}, sf::Color::Black,
            CONF<float>("graphics/fade_in_out_duration")
    ));
}

void Client::spawnSpecial(const sf::Vector2f& pos, const std::string& name)
{
    auto ptr = this->spawnNewSpecial(name, -1, pos, Functional::Activation::None, {}, {});
}

void Client::spawnShotEvent(const std::string& name, const sf::Vector2f& pos, float dir)
{
    auto vector = sf::Vector2f{static_cast<float>(std::cos(dir)), static_cast<float>(std::sin(dir))};
    spawnEvent("shot", pos + RMGET<float>("bullets", name, "burst_offset") * vector, dir * 180.0f / M_PI, RMGET<float>("bullets", name, "burst_size"));

    if (CONF<bool>("sound/sound_on"))
        engine_->spawnSoundEvent(RM.getSound(name + "_bullet_shot"), pos);
}

void Client::spawnBloodEvent(const sf::Vector2f& pos, float dir, float deadly_factor)
{
    spawnEvent("blood", pos, dir, 0.0f);
    auto ptr = spawnNewDestructionSystem(pos, dir, destruction_params_["blood"], deadly_factor);
}

void Client::spawnBullet(Character* user, const std::string& name, const sf::Vector2f& pos, float dir)
{
    auto ptr = this->spawnNewBullet(user, name, pos, dir);

    auto vector = sf::Vector2f{static_cast<float>(std::cos(dir)), static_cast<float>(std::sin(dir))};

    spawnNewDestructionSystem(pos - 20.0f * vector, dir * 180.0f / M_PI + 90.0f, destruction_params_["husk"], 1.0f);
}

void Client::spawnExplosionForce(const sf::Vector2f& pos, float r)
{
    desired_explosions_.emplace_back(pos, r);
}

void Client::updateExplosions()
{
    for (const auto& explosion : explosions_)
    {
        engine_->deleteHoveringObject(explosion.get());
    }
    explosions_.clear();

    for (const auto& desired_explosion : desired_explosions_)
    {
        camera_->setShaking(2.0f);
        explosions_.emplace_back(std::make_unique<Explosion>(desired_explosion.first, desired_explosion.second));
        engine_->registerHoveringObject(explosions_.back().get());
    }

    desired_explosions_.clear();
}

void Client::alertCollision(HoveringObject* h_obj, StaticObject* s_obj)
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
            spawnSparksEvent(bullet->getPosition(), bullet->getRotation() - 90.0f, 0.0f);
            bullet->setDead();
        }
        else if (obstacle_tile != nullptr)
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
            spawnBloodEvent(character->getPosition() + sf::Vector2f(0.0f, offset), bullet->getRotation() + 180.0f, bullet->getDeadlyFactor());

            bullet->setDead();
        }
        return;
    }

    auto special = dynamic_cast<Special*>(h_obj);

    if (special != nullptr && character != nullptr && special->isActive())
    {
        if (special->getActivation() == Functional::Activation::OnEnter)
        {
            auto player = dynamic_cast<Player*>(d_obj);
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
    }

    auto explosion = dynamic_cast<Explosion*>(h_obj);

    if (character != nullptr && explosion != nullptr)
    {
        // TODO this should not be invoked
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
                    melee_weapon_area->getFather()->getUser()->getPosition() - character->getPosition())) * 180.0 / M_PI);
            spawnBloodEvent(character->getPosition() + sf::Vector2f(0.0f, angle > 0 && angle <= 180 ? 5.0 : -5.0), angle, melee_weapon_area->getFather()->getDeadlyFactor());
            melee_weapon_area->setActive(false);
        }
    }
}

void Client::alertCollision(DynamicObject* d_obj, StaticObject* s_obj)
{
    // Nothing to do for now (maybe sounds?)
}

void Client::alertCollision(DynamicObject* d_obj_1, DynamicObject* d_obj_2)
{
    // Nothing to do for now (maybe sounds?)
}

Bullet* Client::spawnNewBullet(Character* user, const std::string& id, const sf::Vector2f& pos, float dir)
{
    bullets_.emplace_back(std::make_unique<Bullet>(user, pos, id, dir));

    auto ptr = bullets_.back().get();

    registerFunctions(ptr);
    engine_->registerHoveringObject(ptr);

    return ptr;
}

void Client::spawnFire(Character* user, const std::string& name, const sf::Vector2f& pos, float dir)
{
    auto vector = sf::Vector2f{static_cast<float>(std::cos(dir)), static_cast<float>(std::sin(dir))};
    auto ptr = this->spawnNewFire(user, pos + 20.0f * vector, dir);
}

void Client::updateFire(float time_elapsed)
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

Fire* Client::spawnNewFire(Character* user, const sf::Vector2f& pos, float dir)
{
    fire_.emplace_back(std::make_unique<Fire>(user, pos, dir));

    auto ptr = fire_.back().get();

    registerLight(ptr);
    engine_->registerHoveringObject(ptr);

    return ptr;
}

ObstacleTile* Client::spawnNewObstacleTile(const std::string& id, const sf::Vector2f& pos)
{
    auto new_ptr = map_->spawn<ObstacleTile>(pos, 0.0f, id);
    engine_->registerStaticObject(new_ptr);
    return new_ptr;
}

Obstacle* Client::spawnNewObstacle(const std::string& id, int u_id, const sf::Vector2f& pos,
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

    registerLight(new_ptr);
    engine_->registerStaticObject(new_ptr);

    registerFunctions(new_ptr);

    return new_ptr;
}

void Client::findAndDeleteBullet(Bullet* ptr)
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

    LOG.error("[Client] Warning - bullet to delete not found!");
}

void Client::findAndDeleteFire(Fire* ptr)
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

    LOG.error("[Client] Warning - fire to delete not found!");
}

void Client::findAndDeleteDecoration(Decoration* ptr)
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

    LOG.error("[Client] Warning - decoration to delete not found!");
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
        curr->use(player_.get());
    }
}

void Client::updatePlayers(float time_elapsed)
{
    player_->setCurrentSpecialObject(nullptr);
    player_->setCurrentTalkableCharacter(nullptr);
    if (player_->isAlive() && !player_->update(time_elapsed))
    {
        spawnDecoration(player_->getPosition(), "blood");
        spawnKillEvent(player_->getPosition());
        player_->setDead();
        engine_->deleteDynamicObject(player_.get());
    }

    for (auto& player : players_)
    {
        player.second->setCurrentSpecialObject(nullptr);
        player.second->setCurrentTalkableCharacter(nullptr);
        if (player.second->isAlive() && !player.second->update(time_elapsed))
        {
            spawnDecoration(player.second->getPosition(), "blood");
            spawnKillEvent(player.second->getPosition());
            player.second->setDead();
            engine_->deleteDynamicObject(player.second.get());
        }
    }
}

void Client::updateBullets(float time_elapsed)
{
    for (auto it = bullets_.begin(); it != bullets_.end(); ++it)
    {
        if (!(*it)->update(time_elapsed))
        {
            if ((*it)->getActivation() == Functional::Activation::OnKill)
            {
                (*it)->use(player_.get());
            }

            engine_->deleteHoveringObject(it->get());
            auto next_it = std::next(it);
            bullets_.erase(it);
            it = next_it;
        }
    }
}

std::tuple<Client::SpawningFunction, Client::AnimationSpawningFunction> Client::getSpawningFunction(const std::string& name)
{
    static const auto swirl = [this](const std::string& name, const sf::Vector2f pos, float dir, bool flipped) { this->spawnSwirlEvent(name, pos, flipped); };
    static const auto null = [this](const std::string& name, const sf::Vector2f pos, float dir, bool flipped) { this->spawnNull(nullptr, "", pos, 0.0f); };
    static const auto sparks = [this](const std::string& name, const sf::Vector2f pos, float dir, bool flipped) { this->spawnShotEvent(name, pos, dir); };
    auto it = spawning_func_.find(name);

    if (it == spawning_func_.end())
    {
        throw std::invalid_argument("[Client] SpawningFunction " + name + " is not handled!");
    }

    if (name == "bullet")
        return std::make_tuple(it->second, sparks);
    else if (name == "fire")
        return std::make_tuple(it->second, null);

    return std::make_tuple(it->second, swirl);
}

void Client::spawnNull(Character* user, const std::string& name, const sf::Vector2f& pos, float dir)
{

}

const std::list<std::unique_ptr<Fire>>& Client::getFires() const
{
    return fire_;
}

Decoration* Client::spawnNewDecoration(const std::string& id, int u_id, const sf::Vector2f& pos)
{
    auto ptr = map_->spawn<Decoration>(pos, 0.0f, id);
    if (u_id != -1)
    {
        ptr->setUniqueId(u_id);
    }
    registerLight(ptr);

    return ptr;
}

void Client::spawnDecoration(const sf::Vector2f& pos, const std::string& name)
{
    auto ptr = this->spawnNewDecoration(name, -1, pos);
}

Special* Client::spawnNewSpecial(const std::string& id, int u_id,
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

    registerLight(ptr);
    registerFunctions(ptr);

    return ptr;
}

void Client::findAndDeleteSpecial(Special* ptr)
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

    LOG.error("[Client] Warning - special to delete not found!");
}

void Client::findAndDeleteDestructionSystem(DestructionSystem* ptr)
{
    for (auto it = destruction_systems_.rbegin(); it != destruction_systems_.rend(); ++it)
    {
        if (it->get() == ptr)
        {
            destruction_systems_.erase((++it).base());
            return;
        }
    }

    LOG.error("[Client] Warning - destruction system to delete not found!");
}

void Client::registerWeapons(Character* character)
{
    for (auto& weapon : character->getWeapons())
    {
        registerWeapon(weapon.get());
    }
}

void Client::registerWeapon(AbstractWeapon* weapon)
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

void Client::registerFunctions(Functional* functional) const
{
//    for (const auto& function : functional->getFunctions())
//    {
//        auto& function_str = j3x::getObj<std::string>(function);
//        functional->bindFunction(special_functions_->bindFunction(function_str),
//                                 special_functions_->bindTextToUse(function_str),
//                                 special_functions_->isUsableByNPC(function_str));
//    }
}

void Client::registerLight(Lightable* lightable) const
{
    if (lightable->getLightPoint() != nullptr)
    {
        lightable->getLightPoint()->registerGraphics(engine_->getGraphics());
    }
}

void Client::updateDestructionSystems(float time_elapsed)
{
    utils::eraseIf<std::unique_ptr<DestructionSystem>>(destruction_systems_, [&time_elapsed, this](std::unique_ptr<DestructionSystem>& system) {
        if (!system->update(time_elapsed))
        {
            return true;
        }
        return false;
    });
}

void Client::initDestructionParams()
{
    /* blood */
    destruction_params_["blood"] = {};
    destruction_params_["debris"] = {};
    destruction_params_["husk"] = {};

    for (auto& param : destruction_params_)
    {
        param.second.vel = CONF<float>("graphics/" + param.first + "_system_vel");
        param.second.acc = CONF<float>("graphics/" + param.first + "_system_acc");
        param.second.time = CONF<float>("graphics/" + param.first + "_system_time");
        param.second.count = CONF<int>("graphics/" + param.first + "_system_count");
        param.second.base_color = sf::Color(CONF<int>("graphics/" + param.first + "_system_base_color"));

        param.second.spread_degree = CONF<float>("graphics/" + param.first + "_system_spread_degree");
        param.second.acceleration_spread = CONF<float>("graphics/" + param.first + "_system_acceleration_spread");

        param.second.vel_fac = CONF<float>("graphics/" + param.first + "_system_vel_fac");
        param.second.acc_fac = CONF<float>("graphics/" + param.first + "_system_acc_fac");
        param.second.time_fac = CONF<float>("graphics/" + param.first + "_system_time_fac");

        param.second.min_size = CONF<float>("graphics/" + param.first + "_system_min_size");
        param.second.max_size = CONF<float>("graphics/" + param.first + "_system_max_size");

        param.second.shader = CONF<std::string>("graphics/" + param.first + "_system_shader");
        param.second.full_color_fac = CONF<float>("graphics/" + param.first + "_system_full_color_fac");
        param.second.r_fac = CONF<float>("graphics/" + param.first + "_system_r_fac");
        param.second.g_fac = CONF<float>("graphics/" + param.first + "_system_g_fac");
        param.second.b_fac = CONF<float>("graphics/" + param.first + "_system_b_fac");

    }
}

DestructionSystem* Client::spawnNewDestructionSystem(const sf::Vector2f& pos, float dir, const DestructionParams& params, float quantity_factor)
{
    destruction_systems_.emplace_back(std::make_unique<DestructionSystem>(pos, dir, params, quantity_factor));
    return destruction_systems_.back().get();
}

void Client::close()
{
    this->engine_->getGraphics().getWindow().close();
}

void Client::establishConnection(const sf::IpAddress& ip)
{
    unsigned short port = 54000;
    char data[4] = "YES";
    if (data_send_socket_.send(data, 4, ip, port) != sf::Socket::Done)
    {
        LOG.error("[Client] Could not establish connection with host: " + ip.toString());
    }
    else
    {
        LOG.info("[Client] Connection with host: " + ip.toString() + " successful!");
        server_ip_ = ip;
    }
}

void Client::sendInputs()
{
    static constexpr float packet_time_elapsed = 0.01f;
    static unsigned short port = 54001;

    if (time_elapsed_ - last_packet_timestamp_ >= packet_time_elapsed)
    {
        PlayerInputPacket packet(ui_->getKeysPressed(), ui_->isLeftMousePressed(), player_->getRotation(), player_->getCurrentWeapon());

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

            static constexpr auto max_ping = 300;

            if (sender == server_ip_ && packet.getTimestamp() >= last_received_packet_timestamp_ && utils::timeSinceEpochMillisec() - packet.getTimestamp() < max_ping)
            {
                for (const auto& data : packet.getDatas())
                {
                    Player* player = nullptr;
                    if (data.first == sf::IpAddress::getLocalAddress().toString())
                        player = player_.get();
                    else
                    {
                        player = getPlayer(data.first);
                        player->setRotation(data.second.rotation_);
                        player->setRotateTo(data.second.rotation_);
                    }

                    player->setPosition(data.second.pos_);
                    player->setVelocity(data.second.vel_);
                    player->setForcedVelocity(data.second.vel_);
                    player->setHealth(data.second.health_);

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
                LOG.error("This server is not registered or packet is old!"
                          "\nPacket timestamp difference: " + std::to_string(utils::timeSinceEpochMillisec() - packet.getTimestamp()));

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
        if (data.first != sf::IpAddress::getLocalAddress().toString())
        {
            auto player = getPlayer(data.first);

            if (data.second.is_shooting_)
                player->shot();
        }
    }
}

Player* Client::getPlayer(const std::string& ip)
{
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
