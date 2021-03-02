//
// Created by jul3x on 27.02.19.
//

#include <R3E/utils/Geometry.h>

#include <common/ResourceManager.h>
#include <common/MeleeWeapon.h>

#include <events/Event.h>
#include <misc/JournalEntries.h>

#include <Game.h>



Game::Game() : current_time_factor_(1.0f), state_(GameState::Menu), rag3_time_elapsed_(-10.0f), time_elapsed_(0.0f)
{
    engine_ = std::make_unique<Engine>();
    engine_->registerGame(this);
}

void Game::initialize()
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
    spawning_func_["Null"] = [this] (Character* user, const std::string& name, const sf::Vector2f& pos, float dir) { this->spawnNull(user, name, pos, dir); };

    player_ = std::make_unique<Player>(sf::Vector2f{0.0f, 0.0f});
    ui_ = std::make_unique<UserInterface>();
    camera_ = std::make_unique<Camera>();
    journal_ = std::make_unique<Journal>(CONF<float>("journal_max_time"), CONF<float>("journal_sampling_rate"));
    special_functions_ = std::make_unique<SpecialFunctions>();
    stats_ = std::make_unique<Stats>();
    achievements_ = std::make_unique<Achievements>(stats_.get());
    achievements_->load(CONF<std::string>("paths/achievements"));

    map_ = std::make_unique<Map>();
    agents_manager_ = std::make_unique<ai::AgentsManager>(map_->getMapBlockage(), ai::AStar::EightNeighbours,
                                                          CONF<float>("characters/max_time_without_path_recalc"),
                                                          CONF<float>("characters/min_pos_change_without_path_recalc"),
                                                          CONF<int>("characters/max_path_search_depth"));

    music_manager_ = std::make_unique<audio::MusicManager>();
    music_manager_->addToQueue(CONF<std::string>("paths/music_dir") + "/menu.ogg");
    music_manager_->setVolume(CONF<float>("sound/music_volume"));
    if (CONF<bool>("sound/sound_on"))
        music_manager_->play();

    ui_->registerCamera(camera_.get());
    ui_->registerPlayer(player_.get());

    engine_->registerCamera(camera_.get());
    engine_->registerUI(ui_.get());

    this->initDestructionParams();

    map_->loadMap("first_new_map");
    engine_->getGraphics().setBgColor(sf::Color(j3x::get<int>(map_->getParams(), "background_color")));
    lightning_ = std::make_unique<graphics::Lightning>(sf::Vector2f{static_cast<float>(CONF<int>("graphics/window_width_px")),
                                                                    static_cast<float>(CONF<int>("graphics/window_height_px"))},
                                                       sf::Color(j3x::get<int>(map_->getParams(), "lightning_color")));

//    debug_map_blockage_ = std::make_unique<DebugMapBlockage>(&map_->getMapBlockage());

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

    for (auto& character : map_->getList<NPC>())
    {
        engine_->registerDynamicObject(character.get());
        registerLight(character.get());
        character->registerAgentsManager(agents_manager_.get());
        character->registerEnemy(player_.get());
        character->registerMapBlockage(&map_->getMapBlockage());
        registerFunctions(character.get());
        registerWeapons(character.get());

        auto talkable_area = character->getTalkableArea();
        if (talkable_area != nullptr)
        {
            engine_->registerHoveringObject(talkable_area);
        }
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
}

void Game::update(float time_elapsed)
{
    if (CONF<bool>("sound/sound_on"))
    {
        Engine::changeSoundListenerPosition(player_->getPosition());
        music_manager_->update(time_elapsed);
    }

    switch (state_)
    {
        case GameState::Menu:
        case GameState::Paused:
        {
            break;
        }
        case GameState::Normal:
        {
            updateExplosions();

            agents_manager_->update();

            updateMapObjects(time_elapsed);
            updatePlayerClone(time_elapsed);
            updatePlayer(time_elapsed);
            updateBullets(time_elapsed);
            updateFire(time_elapsed);
            updateDestructionSystems(time_elapsed);

            achievements_->update(time_elapsed);
            journal_->update(time_elapsed);
            camera_->update(time_elapsed);

            if (rag3_time_elapsed_ > 0.0f)
                rag3_time_elapsed_ -= time_elapsed;
            else if (rag3_time_elapsed_ > -1.0f)
                setRag3Time(-10.0f);

            time_elapsed_ += time_elapsed;
            break;
        }
        case GameState::Reverse:
        {
            if (!journal_->executeTimeReversal(CONF<float>("time_reversal_speed_factor") * time_elapsed))
            {
                this->setGameState(GameState::Normal);
            }

            auto update_lights = [](auto& list) {
                for (auto& obj : list)
                {
                    auto light = obj->getLightPoint();

                    if (light != nullptr)
                        light->setPosition(obj->getPosition());
                }
            };

            update_lights(map_->getList<Obstacle>());
            update_lights(map_->getList<NPC>());
            update_lights(map_->getList<Special>());
            update_lights(map_->getList<Decoration>());
            update_lights(fire_);

            auto player_light = player_->getLightPoint();
            if (player_light != nullptr)
                player_light->setPosition(player_->getPosition());

            if (player_clone_ != nullptr)
            {
                auto clone_light = player_clone_->getLightPoint();

                if (clone_light != nullptr)
                    clone_light->setPosition(player_clone_->getPosition());
            }

            camera_->update(time_elapsed);
            break;
        }
    }
}

void Game::updateMapObjects(float time_elapsed)
{
    auto& blockage = map_->getMapBlockage();

    auto& obstacles =  map_->getList<Obstacle>();;
    auto& npcs =  map_->getList<NPC>();;
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
            journal_->event<DestroyObstacle>(it->get());

            this->spawnExplosionEvent((*it)->getPosition());

            auto next_it = std::next(it);
            engine_->deleteStaticObject(it->get());

            if ((*it)->getActivation() == Functional::Activation::OnKill)
            {
                (*it)->use(player_.get());
            }

            auto grid_vector_pos = (*it)->getPosition() + RMGET<sf::Vector2f>("obstacles", (*it)->getId(), "collision_offset");
            auto grid_pos = std::make_pair(std::round(grid_vector_pos.x / DecorationTile::SIZE_X_),
                                           std::round(grid_vector_pos.y / DecorationTile::SIZE_Y_));
            blockage.blockage_.at(grid_pos.first).at(grid_pos.second) = 0.0f;

            obstacles.erase(it);
            it = next_it;
            do_increment = false;
        }

        if (do_increment) ++it;
    }

    for (auto it = npcs.begin(); it != npcs.end();)
    {
        bool do_increment = true;
        (*it)->setCurrentSpecialObject(nullptr);
        (*it)->setCurrentTalkableCharacter(nullptr);
        if (!(*it)->update(time_elapsed))
        {
            this->killNPC(it->get());

            auto next_it = std::next(it);
            npcs.erase(it);
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
            journal_->event<DestroySpecial>(it->get());
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
            journal_->event<DestroyDecoration>(it->get());
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

void Game::killNPC(NPC* npc)
{
    journal_->event<DestroyCharacter>(npc);
    stats_->killEnemy(npc->getPosition());

    if (player_clone_ != nullptr)
    {
        player_clone_->removeEnemy(npc);
    }

    // draw on this place destruction
    spawnDecoration(npc->getPosition(), "blood");

    this->spawnExplosionEvent(npc->getPosition());

    engine_->deleteDynamicObject(npc);
    auto talkable_area = npc->getTalkableArea();
    if (talkable_area != nullptr)
    {
        engine_->deleteHoveringObject(talkable_area);
    }

    for (auto& weapon : npc->getWeapons())
    {
        auto melee_weapon = dynamic_cast<MeleeWeapon*>(weapon.get());

        if (melee_weapon != nullptr)
        {
            engine_->deleteHoveringObject(melee_weapon->getMeleeWeaponArea());
        }
    }

    if (npc->getActivation() == Functional::Activation::OnKill && npc->isActive())
    {
        npc->use(npc);
    }
}

void Game::draw(graphics::Graphics& graphics)
{
    if (state_ != GameState::Menu)
    {
        static sf::RenderStates states;

        sf::Shader* curr_shader = &RM.getShader(j3x::get<std::string>(map_->getParams(), "shader"));

        if (rag3_time_elapsed_ > 0.0f)
        {
            curr_shader = &RM.getShader("rag3.frag");
            curr_shader->setUniform("time", rag3_time_elapsed_);
            curr_shader->setUniform("rag3_time", CONF<float>("rag3_time"));
        }
        else
        {
            curr_shader->setUniform("time", this->time_elapsed_);
        }

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

//    debug_map_blockage_->draw(graphics);
        draw(map_->getList<DecorationTile>());
        draw(map_->getList<Decoration>());
        draw(map_->getList<Obstacle>());
        draw(map_->getList<ObstacleTile>());
        draw(map_->getList<NPC>());
        draw(map_->getList<PlacedWeapon>());
        draw(destruction_systems_);
        draw(bullets_);
        draw(fire_);

        for (auto& special : map_->getList<Special>())
            if (special->isDrawable())
                graphics.drawSorted(*special);

        if (player_->isAlive())
            graphics.drawSorted(*player_);

        if (player_clone_ != nullptr)
            graphics.drawSorted(*player_clone_);

        engine_->drawSortedAnimationEvents();

        graphics.drawAlreadySorted(states.shader);

        lightning_->clear();

        draw_light(map_->getList<NPC>());
        draw_light(map_->getList<Obstacle>());
        draw_light(map_->getList<Decoration>());
        draw_light(map_->getList<Special>());
        draw_light(fire_);
        draw_light(engine_->getAnimationEvents());

        lightning_->add(*player_->getLightPoint());
        if (player_clone_ != nullptr)
            lightning_->add(*player_clone_->getLightPoint());

        graphics.setStaticView();
        graphics.draw(*lightning_);
    }
}

void Game::start()
{
    engine_->start();
}

Map& Game::getMap()
{
    return *map_;
}

Player& Game::getPlayer()
{
    return *player_;
}

Camera& Game::getCamera()
{
    return *camera_;
}

Stats& Game::getStats()
{
    return *stats_;
}

Journal& Game::getJournal() const
{
    return *journal_;
}

UserInterface& Game::getUI()
{
    return *ui_;
}

const std::list<std::unique_ptr<Bullet>>& Game::getBullets() const
{
    return bullets_;
}

void Game::spawnEvent(const std::string& name, const sf::Vector2f& pos, float dir, float r)
{
    auto event = std::make_shared<Event>(pos, name, dir, r);
    engine_->spawnAnimationEvent(event);

    registerLight(event.get());
}

void Game::spawnSparksEvent(const sf::Vector2f& pos, const float dir, const float r)
{
    spawnEvent("sparks", pos, dir, r);
    auto ptr = spawnNewDestructionSystem(pos, dir - 90.0f, destruction_params_["debris"], 1.0f);
    journal_->event<SpawnDestructionSystem>(ptr);
}

void Game::spawnExplosionEvent(const sf::Vector2f& pos)
{
    auto number = std::to_string(utils::num::getRandom(1, 1));
    spawnEvent("explosion_" + number, pos, 0.0f, RMGET<sf::Vector2f>("animations", "explosion_" + number, "size").x);

    if (CONF<bool>("sound/sound_on"))
        engine_->spawnSoundEvent(RM.getSound("wall_explosion"), pos);
}

void Game::spawnTeleportationEvent(const sf::Vector2f& pos)
{
    spawnEvent("teleportation", pos + sf::Vector2f{0.0f, 10.0f});

    if (CONF<bool>("sound/sound_on"))
        engine_->spawnSoundEvent(RM.getSound("teleportation"), pos);
}

void Game::spawnSwirlEvent(const std::string& name, const sf::Vector2f& pos, bool flipped)
{
    auto event = std::make_shared<Event>(pos, name + "_swirl");
    engine_->spawnAnimationEvent(event);

    if (flipped)
        event->setFlipX(true);

    registerLight(event.get());
}

void Game::spawnFadeInOut()
{
    engine_->spawnEffect(std::make_shared<graphics::FadeInOut>(
            sf::Vector2f{static_cast<float>(CONF<int>("graphics/window_width_px")),
                         static_cast<float>(CONF<int>("graphics/window_height_px"))}, sf::Color::Black,
            CONF<float>("graphics/fade_in_out_duration")
    ));
}

void Game::spawnThought(Character* user, const std::string& text)
{
    ui_->spawnThought(user, text);
}

void Game::spawnBonusText(const sf::Vector2f& pos, const std::string& text)
{
    ui_->spawnBonusText(pos, text);
}

void Game::spawnAchievement(const j3x::Parameters& params)
{
    ui_->spawnAchievement(
            j3x::get<std::string>(params, "title"),
            j3x::get<std::string>(params, "description"),
            j3x::get<std::string>(params, "texture")
    );
}

void Game::spawnSpecial(const sf::Vector2f& pos, const std::string& name)
{
    auto ptr = this->spawnNewSpecial(name, -1, pos, Functional::Activation::None, {}, {});
    journal_->event<SpawnSpecial>(ptr);
}

void Game::spawnShotEvent(const std::string& name, const sf::Vector2f& pos, float dir)
{
    auto vector = sf::Vector2f{static_cast<float>(std::cos(dir)), static_cast<float>(std::sin(dir))};
    spawnEvent("shot", pos + RMGET<float>("bullets", name, "burst_offset") * vector, dir * 180.0f / M_PI, RMGET<float>("bullets", name, "burst_size"));

    if (CONF<bool>("sound/sound_on"))
        engine_->spawnSoundEvent(RM.getSound(name + "_bullet_shot"), pos);
}

void Game::spawnBloodEvent(const sf::Vector2f& pos, float dir, float deadly_factor)
{
    spawnEvent("blood", pos, dir, 0.0f);
    auto ptr = spawnNewDestructionSystem(pos, dir, destruction_params_["blood"], deadly_factor);
    journal_->event<SpawnDestructionSystem>(ptr);
}

void Game::spawnBullet(Character* user, const std::string& name, const sf::Vector2f& pos, float dir)
{
    auto ptr = this->spawnNewBullet(user, name, pos, dir);

    auto vector = sf::Vector2f{static_cast<float>(std::cos(dir)), static_cast<float>(std::sin(dir))};

    spawnNewDestructionSystem(pos - 20.0f * vector, dir * 180.0f / M_PI + 90.0f, destruction_params_["husk"], 1.0f);
    journal_->event<SpawnBullet>(ptr);
}

void Game::spawnExplosionForce(const sf::Vector2f& pos, float r)
{
    desired_explosions_.emplace_back(pos, r);
}

void Game::updateExplosions()
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

void Game::alertCollision(HoveringObject* h_obj, StaticObject* s_obj)
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
            journal_->event<ShotObstacle>(obstacle);
            obstacle->getShot(*bullet);

            spawnSparksEvent(bullet->getPosition(), bullet->getRotation() - 90.0f, 0.0f);

            bullet->setDead();
        }
        else if (obstacle_tile != nullptr)
        {
            spawnSparksEvent(bullet->getPosition(), bullet->getRotation() - 90.0f, 0.0f);
            bullet->setDead();
        }
    }
    else if (explosion != nullptr)
    {
        if (obstacle != nullptr)
        {
            journal_->event<ShotObstacle>(obstacle);
            explosion->applyForce(obstacle);
        }
    }
    else if (fire != nullptr)
    {
        fire->setDead();
    }
}

void Game::alertCollision(HoveringObject* h_obj, DynamicObject* d_obj)
{
    auto bullet = dynamic_cast<Bullet*>(h_obj);
    auto character = dynamic_cast<Character*>(d_obj);

    auto factor = rag3_time_elapsed_ > 0.0f ? CONF<float>("characters/rag3_factor") : 1.0f;

    if (bullet != nullptr && character != nullptr)
    {
        if (bullet->getUser() != character)
        {
            if (bullet->getUser() != player_.get())
            {
                factor = 1.0f;
            }

            character->getShot(*bullet, factor);

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
        else
        {
            auto player = dynamic_cast<Player*>(d_obj);
            if (player != nullptr)
            {
                player->addSpecialToBackpack(special);
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
            if (melee_weapon_area->getFather()->getUser() != player_.get())
            {
                factor = 1.0f;
            }

            float angle = utils::geo::wrapAngle0_360(std::get<1>(utils::geo::cartesianToPolar(
                    melee_weapon_area->getFather()->getUser()->getPosition() - character->getPosition())) * 180.0 / M_PI);
            spawnBloodEvent(character->getPosition() + sf::Vector2f(0.0f, angle > 0 && angle <= 180 ? 5.0 : -5.0), angle, melee_weapon_area->getFather()->getDeadlyFactor());
            melee_weapon_area->setActive(false);
            character->getCut(*melee_weapon_area->getFather(), factor);
        }
    }
}

void Game::alertCollision(DynamicObject* d_obj, StaticObject* s_obj)
{
    // Nothing to do for now (maybe sounds?)
}

void Game::alertCollision(DynamicObject* d_obj_1, DynamicObject* d_obj_2)
{
    // Nothing to do for now (maybe sounds?)
}

Bullet* Game::spawnNewBullet(Character* user, const std::string& id, const sf::Vector2f& pos, float dir)
{
    bullets_.emplace_back(std::make_unique<Bullet>(user, pos, id, dir));

    auto ptr = bullets_.back().get();

    registerFunctions(ptr);
    engine_->registerHoveringObject(ptr);

    return ptr;
}

void Game::spawnFire(Character* user, const std::string& name, const sf::Vector2f& pos, float dir)
{
    auto vector = sf::Vector2f{static_cast<float>(std::cos(dir)), static_cast<float>(std::sin(dir))};
    auto ptr = this->spawnNewFire(user, pos + 20.0f * vector, dir);
    journal_->event<SpawnFire>(ptr);
}

void Game::updateFire(float time_elapsed)
{
    for (auto it = fire_.begin(); it != fire_.end(); ++it)
    {
        if (!(*it)->update(time_elapsed))
        {
            engine_->deleteHoveringObject(it->get());
            journal_->event<DestroyFire>(it->get());
            auto next_it = std::next(it);
            fire_.erase(it);
            it = next_it;
        }
    }
}

Fire* Game::spawnNewFire(Character* user, const sf::Vector2f& pos, float dir)
{
    fire_.emplace_back(std::make_unique<Fire>(user, pos, dir));

    auto ptr = fire_.back().get();

    registerLight(ptr);
    engine_->registerHoveringObject(ptr);

    return ptr;
}


NPC* Game::spawnNewNPC(const std::string& id, int u_id, Functional::Activation activation,
                       const j3x::List& funcs, const j3x::List& datas)
{
    auto ptr = map_->spawn<NPC>({}, 0.0f, id);

    if (u_id != -1)
    {
        ptr->setUniqueId(u_id);
    }

    ptr->setActivation(activation);
    ptr->setFunctions(funcs);
    ptr->setDatas(datas);
    engine_->registerDynamicObject(ptr);
    registerLight(ptr);

    ptr->registerAgentsManager(agents_manager_.get());
    ptr->registerEnemy(player_.get());

    if (player_clone_ != nullptr)
        player_clone_->registerEnemy(ptr);

    ptr->registerMapBlockage(&map_->getMapBlockage());
    registerWeapons(ptr);
    registerFunctions(ptr);

    auto talkable_area = ptr->getTalkableArea();
    if (talkable_area != nullptr)
        engine_->registerHoveringObject(talkable_area);

    return ptr;
}

NPC* Game::spawnNewPlayerClone(const std::string& weapon_id)
{
    if (player_clone_ != nullptr)
    {
        this->cleanPlayerClone();
    }

    player_clone_ = std::make_unique<PlayerClone>(this->player_->getPosition(), player_.get(),
                                                  journal_->getDurationSaved() *
                                                  CONF<float>("player_clone_time_factor"));
    engine_->registerDynamicObject(player_clone_.get());
    registerLight(player_clone_.get());

    player_clone_->registerAgentsManager(agents_manager_.get());
    player_clone_->registerMapBlockage(&map_->getMapBlockage());

    for (auto& enemy : map_->getList<NPC>())
    {
        player_clone_->registerEnemy(enemy.get());

        enemy->registerEnemy(player_clone_.get());
    }

    player_clone_->makeOnlyOneWeapon(weapon_id, 0.0f);
    registerWeapons(player_clone_.get());

    return player_clone_.get();
}

void Game::cleanPlayerClone()
{
    for (auto& enemy : map_->getList<NPC>())
    {
        enemy->removeEnemy(player_clone_.get());
    }

    for (auto& weapon : player_clone_->getWeapons())
    {
        auto melee_weapon = dynamic_cast<MeleeWeapon*>(weapon.get());

        if (melee_weapon != nullptr)
        {
            engine_->deleteHoveringObject(melee_weapon->getMeleeWeaponArea());
        }
    }

    engine_->deleteDynamicObject(player_clone_.get());
    player_clone_.reset();
}

ObstacleTile* Game::spawnNewObstacleTile(const std::string& id, const sf::Vector2f& pos)
{
    auto new_ptr = map_->spawn<ObstacleTile>(pos, 0.0f, id);
    engine_->registerStaticObject(new_ptr);
    return new_ptr;
}

Obstacle* Game::spawnNewObstacle(const std::string& id, int u_id, const sf::Vector2f& pos,
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

void Game::findAndDeleteBullet(Bullet* ptr)
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

    LOG.error("[Game] Warning - bullet to delete not found!");
}

void Game::findAndDeleteFire(Fire* ptr)
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

    LOG.error("[Game] Warning - fire to delete not found!");
}

void Game::findAndDeleteDecoration(Decoration* ptr)
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

    LOG.error("[Game] Warning - decoration to delete not found!");
}

Special* Game::getCurrentSpecialObject() const
{
    return player_->getCurrentSpecialObject();
}

Character* Game::getCurrentTalkableCharacter() const
{
    return player_->getCurrentTalkableCharacter();
}

void Game::useSpecialObject()
{
    auto curr = player_->getCurrentSpecialObject();
    if (curr != nullptr)
    {
        curr->use(player_.get());
    }
}

void Game::talk()
{
    auto curr = player_->getCurrentTalkableCharacter();
    if (curr != nullptr)
    {
        auto still_talking = curr->talk([this](Character* character, const std::string& text) { this->spawnThought(character, text); },
                player_.get());

        if (!still_talking)
        {
            engine_->deleteHoveringObject(curr->getTalkableArea());
            curr->use(curr);
            curr->deactivate();
        }
    }
}

void Game::setBulletTime()
{
    current_time_factor_ = CONF<float>("bullet_time_factor");
    engine_->setTimeScaleFactor(current_time_factor_);

    if (CONF<bool>("sound/sound_on"))
        music_manager_->setPlaybackPitch(CONF<float>("sound/bullet_time_music_factor"));
}

void Game::setNormalTime()
{
    current_time_factor_ = 1.0f;
    engine_->setTimeScaleFactor(1.0f);

    if (CONF<bool>("sound/sound_on"))
        music_manager_->setPlaybackPitch(1.0f);
}

void Game::setGameState(Game::GameState state)
{
    switch (state)
    {
        case GameState::Normal:
            if (state_ == GameState::Reverse)
            {
                if (player_clone_ != nullptr)
                    player_clone_->updateLifeTimeDependingOnPrevious(
                            journal_->getDurationSaved() * CONF<float>("player_clone_time_factor"));

                for (auto& system : destruction_systems_)
                {
                    // TODO This is bug for those systems that were added by journal
                    system->addToLife(journal_->getTimeReversed());
                }

                journal_->clear();
            }
            else if (state_ == GameState::Menu)
            {
                music_manager_->clearQueue();
                music_manager_->addDirectoryToQueue(CONF<std::string>("paths/music_dir") + "/1/");

                camera_->setZoomTo(1.0f);
                camera_->setCenter({player_->getPosition().x, player_->getPosition().y, 0.0f});
            }

            if (rag3_time_elapsed_ < 0.0f)
                camera_->setNormal();

            engine_->turnOnCollisions();
            if (CONF<bool>("sound/sound_on"))
                music_manager_->play();
            break;
        case GameState::Reverse:
            if (this->isJournalFreezed())
                return;

            this->spawnTeleportationEvent(player_->getPosition());

            camera_->setReverse();
            engine_->turnOffCollisions();
            journal_->event<TimeReversal, void*>();
            break;
        case GameState::Menu:
            music_manager_->clearQueue();
            music_manager_->addToQueue(CONF<std::string>("paths/music_dir") + "/menu.ogg");
            if (CONF<bool>("sound/sound_on"))
                music_manager_->play();
            break;
        case GameState::Paused:
            music_manager_->pause();
            break;
    }

    state_ = state;
}

Game::GameState Game::getGameState() const
{
    return state_;
}

bool Game::isJournalFreezed() const
{
    return journal_->getDurationSaved() < CONF<float>("journal_min_time");
}

void Game::updatePlayerClone(float time_elapsed)
{
    if (player_clone_ != nullptr)
    {
        player_clone_->setCurrentSpecialObject(nullptr);
        player_clone_->setCurrentTalkableCharacter(nullptr);
        if (!player_clone_->update(time_elapsed))
        {
            // draw on this place destruction
            spawnDecoration(player_clone_->getPosition(), "blood");

            this->spawnExplosionEvent(player_clone_->getPosition());
            this->cleanPlayerClone();

            player_->setHealth(0); // player clone is dead - so do player
        }
    }

    if (player_clone_ != nullptr && !player_clone_->isLifeTime())
    {
        this->spawnTeleportationEvent(player_clone_->getPosition());

        const auto& player_clone_weapon = player_clone_->getWeapons().at(player_clone_->getCurrentWeapon());
        auto player_weapon = std::find_if(player_->getWeapons().begin(), player_->getWeapons().end(),
                                          [&player_clone_weapon](const auto& it) {
                                              return player_clone_weapon->getId() == it->getId();
                                          });

        if (player_weapon != player_->getWeapons().end())
            (*player_weapon)->setState(player_clone_weapon->getState());

        player_->setHealth(player_clone_->getHealth());

        camera_->setZoomInOut();

        this->cleanPlayerClone();
    }
}

void Game::updatePlayer(float time_elapsed)
{
    player_->setCurrentSpecialObject(nullptr);
    player_->setCurrentTalkableCharacter(nullptr);
    if (player_->isAlive() && !player_->update(time_elapsed))
    {
        spawnDecoration(player_->getPosition(), "blood");
        spawnExplosionEvent(player_->getPosition());
        player_->setDead();
        engine_->deleteDynamicObject(player_.get());
        music_manager_->stop();
    }
}

void Game::updateBullets(float time_elapsed)
{
    for (auto it = bullets_.begin(); it != bullets_.end(); ++it)
    {
        if (!(*it)->update(time_elapsed))
        {
            if ((*it)->getActivation() == Functional::Activation::OnKill)
            {
                (*it)->use(player_.get());
            }

            journal_->event<DestroyBullet>(it->get());
            engine_->deleteHoveringObject(it->get());
            auto next_it = std::next(it);
            bullets_.erase(it);
            it = next_it;
        }
    }
}

std::tuple<Game::SpawningFunction, Game::AnimationSpawningFunction> Game::getSpawningFunction(const std::string& name)
{
    static const auto swirl = [this](const std::string& name, const sf::Vector2f pos, float dir, bool flipped) { this->spawnSwirlEvent(name, pos, flipped); };
    static const auto null = [this](const std::string& name, const sf::Vector2f pos, float dir, bool flipped) { this->spawnNull(nullptr, "", pos, 0.0f); };
    static const auto sparks = [this](const std::string& name, const sf::Vector2f pos, float dir, bool flipped) { this->spawnShotEvent(name, pos, dir); };
    auto it = spawning_func_.find(name);

    if (it == spawning_func_.end())
    {
        throw std::invalid_argument("[Game] SpawningFunction " + name + " is not handled!");
    }

    if (name == "bullet")
        return std::make_tuple(it->second, sparks);
    else if (name == "fire")
        return std::make_tuple(it->second, null);

    return std::make_tuple(it->second, swirl);
}

void Game::spawnNull(Character* user, const std::string& name, const sf::Vector2f& pos, float dir)
{

}

const std::list<std::unique_ptr<Fire>>& Game::getFires() const
{
    return fire_;
}

Decoration* Game::spawnNewDecoration(const std::string& id, int u_id, const sf::Vector2f& pos)
{
    auto ptr = map_->spawn<Decoration>(pos, 0.0f, id);
    if (u_id != -1)
    {
        ptr->setUniqueId(u_id);
    }
    registerLight(ptr);

    return ptr;
}

void Game::spawnDecoration(const sf::Vector2f& pos, const std::string& name)
{
    auto ptr = this->spawnNewDecoration(name, -1, pos);
    journal_->event<SpawnDecoration>(ptr);
}

Special* Game::spawnNewSpecial(const std::string& id, int u_id,
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

void Game::findAndDeleteSpecial(Special* ptr)
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

    LOG.error("[Game] Warning - special to delete not found!");
}

void Game::findAndDeleteDestructionSystem(DestructionSystem* ptr)
{
    for (auto it = destruction_systems_.rbegin(); it != destruction_systems_.rend(); ++it)
    {
        if (it->get() == ptr)
        {
            destruction_systems_.erase((++it).base());
            return;
        }
    }

    LOG.error("[Game] Warning - destruction system to delete not found!");
}

void Game::registerWeapons(Character* character)
{
    for (auto& weapon : character->getWeapons())
    {
        registerWeapon(weapon.get());
    }
}

void Game::registerWeapon(AbstractWeapon* weapon)
{
    if (!weapon->getId().empty())
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

void Game::registerFunctions(Functional* functional) const
{
    for (const auto& function : functional->getFunctions())
    {
        auto& function_str = j3x::getObj<std::string>(function);
        functional->bindFunction(special_functions_->bindFunction(function_str),
                                 special_functions_->bindTextToUse(function_str),
                                 special_functions_->isUsableByNPC(function_str));
    }
}

void Game::registerLight(Lightable* lightable) const
{
    if (lightable->getLightPoint() != nullptr)
    {
        lightable->getLightPoint()->registerGraphics(engine_->getGraphics());
    }
}

void Game::setRag3Time(float time_elapsed)
{
    rag3_time_elapsed_ = time_elapsed;

    if (time_elapsed > 0.0f)
        camera_->setRag3();
    else
        camera_->setNormal();
}

float Game::getRag3Time() const
{
    return rag3_time_elapsed_;
}

void Game::updateDestructionSystems(float time_elapsed)
{
    utils::eraseIf<std::unique_ptr<DestructionSystem>>(destruction_systems_, [&time_elapsed, this](std::unique_ptr<DestructionSystem>& system) {
        if (!system->update(time_elapsed))
        {
            journal_->event<DestroyDestructionSystem>(system.get());
            return true;
        }
        return false;
    });
}

void Game::initDestructionParams()
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

DestructionSystem* Game::spawnNewDestructionSystem(const sf::Vector2f& pos, float dir, const DestructionParams& params, float quantity_factor)
{
    destruction_systems_.emplace_back(std::make_unique<DestructionSystem>(pos, dir, params, quantity_factor));
    return destruction_systems_.back().get();
}

void Game::close()
{
    this->engine_->getGraphics().getWindow().close();
}

