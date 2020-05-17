//
// Created by jul3x on 27.02.19.
//

#include <chrono>

#include <R3E/utils/Geometry.h>

#include <common/ResourceManager.h>
#include <common/MeleeWeapon.h>

#include <events/Event.h>
#include <misc/JournalEntries.h>

#include <Game.h>



Game::Game() : current_time_factor_(1.0f), state_(GameState::Normal)
{
    engine_ = std::make_unique<Engine>();
    engine_->registerGame(this);
}

void Game::initialize()
{
    spawning_func_["bullet"] = std::bind(&Game::spawnBullet, this, std::placeholders::_1, std::placeholders::_2,
            std::placeholders::_3, std::placeholders::_4);
    spawning_func_["fire"] = std::bind(&Game::spawnFire, this, std::placeholders::_1, std::placeholders::_2,
                                       std::placeholders::_3, std::placeholders::_4);
    spawning_func_["Null"] = std::bind(&Game::spawnNull, this, std::placeholders::_1, std::placeholders::_2,
                                       std::placeholders::_3, std::placeholders::_4);

    player_ = std::make_unique<Player>(sf::Vector2f{0.0f, 0.0f});
    ui_ = std::make_unique<UserInterface>();
    camera_ = std::make_unique<Camera>();
    journal_ = std::make_unique<Journal>(CFG.get<float>("journal_max_time"), CFG.get<float>("journal_sampling_rate"));
    special_functions_ = std::make_unique<SpecialFunctions>();
    stats_ = std::make_unique<Stats>();
    achievements_ = std::make_unique<Achievements>();

    map_ = std::make_unique<Map>();
    agents_manager_ = std::make_unique<ai::AgentsManager>(map_->getMapBlockage(), ai::AStar::EightNeighbours,
                                                          1000.0f, // max time without recalculation of path in ms
                                                          20.0f, // min change of goal to trigger recalculation
                                                          1000); // max search of path

    music_manager_ = std::make_unique<audio::MusicManager>();
    music_manager_->addDirectoryToQueue(CFG.get<std::string>("paths/music_dir"));

    music_manager_->setVolume(CFG.get<float>("sound/music_volume"));
    music_manager_->play();

    ui_->registerCamera(camera_.get());
    ui_->registerPlayer(player_.get());

    engine_->initializeGraphics(
            sf::Vector2i{CFG.get<int>("graphics/window_width_px"), CFG.get<int>("graphics/window_height_px")},
            "Codename: Rag3",
            CFG.get<int>("graphics/full_screen") ? sf::Style::Fullscreen : sf::Style::Default,
            sf::Color(CFG.get<int>("graphics/background_color")));
    engine_->setFPSLimit(0);

    engine_->initializeSoundManager(CFG.get<float>("sound/sound_attenuation"));

    engine_->registerCamera(camera_.get());
    engine_->registerUI(ui_.get());

    map_->loadMap("test_map");

//    debug_map_blockage_ = std::make_unique<DebugMapBlockage>(&map_->getMapBlockage());

    engine_->initializeCollisions(map_->getSize(), CFG.get<float>("collision_grid_size"));

    for (auto& obstacle : map_->getList<ObstacleTile>())
        engine_->registerStaticObject(obstacle.get());

    for (auto& obstacle : map_->getList<Obstacle>())
    {
        engine_->registerStaticObject(obstacle.get());

        for (const auto& function : obstacle->getFunctions())
        {
            obstacle->bindFunction(special_functions_->bindFunction(function),
                                   special_functions_->bindTextToUse(function),
                                   special_functions_->isUsableByNPC(function));
        }
    }

    for (auto& character : map_->getList<NPC>())
    {
        engine_->registerDynamicObject(character.get());

        character->registerAgentsManager(agents_manager_.get());
        character->registerEnemy(player_.get());
        character->registerMapBlockage(&map_->getMapBlockage());

        for (const auto& function : character->getFunctions())
        {
            character->bindFunction(special_functions_->bindFunction(function),
                                    special_functions_->bindTextToUse(function),
                                    special_functions_->isUsableByNPC(function));
        }

        for (auto& weapon : character->getWeapons())
        {
            if (!weapon->getId().empty())
            {
                weapon->registerSpawningFunction(
                        this->getSpawningFunction(utils::j3x::get<std::string>(
                                RM.getObjectParams("weapons", weapon->getId()), "spawn_func")));

                auto melee_weapon = dynamic_cast<MeleeWeapon*>(weapon.get());

                if (melee_weapon != nullptr)
                {
                    engine_->registerHoveringObject(melee_weapon->getMeleeWeaponArea());
                }
            }
        }

        auto talkable_area = character->getTalkableArea();
        if (talkable_area != nullptr)
        {
            engine_->registerHoveringObject(talkable_area);
        }
    }

    engine_->registerDynamicObject(player_.get());
    for (auto& weapon : player_->getWeapons())
    {
        if (!weapon->getId().empty())
            weapon->registerSpawningFunction(
                    this->getSpawningFunction(utils::j3x::get<std::string>(
                            RM.getObjectParams("weapons", weapon->getId()), "spawn_func")));

        auto melee_weapon = dynamic_cast<MeleeWeapon*>(weapon.get());

        if (melee_weapon != nullptr)
        {
            engine_->registerHoveringObject(melee_weapon->getMeleeWeaponArea());
        }
    }

    for (auto& weapon : map_->getList<PlacedWeapon>())
    {
        weapon->registerSpawningFunction(
                this->getSpawningFunction(utils::j3x::get<std::string>(RM.getObjectParams("weapons",
                                                                                          weapon->getId()), "spawn_func")));
    }

    for (auto& special : map_->getList<Special>())
    {
        if (special->getId() == "starting_position")
            player_->setPosition(special->getPosition());

        engine_->registerHoveringObject(special.get());

        for (const auto& function : special->getFunctions())
        {
            special->bindFunction(special_functions_->bindFunction(function),
                                  special_functions_->bindTextToUse(function),
                                  special_functions_->isUsableByNPC(function));
        }
    }
}

void Game::update(float time_elapsed)
{
    switch (state_)
    {
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

            journal_->update(time_elapsed);
            camera_->update(time_elapsed);

            if (CFG.get<int>("sound/sound_on"))
            {
                Engine::changeSoundListenerPosition(player_->getPosition());
                music_manager_->update(time_elapsed);
            }
            break;
        }
        case GameState::Reverse:
        {
            if (!journal_->executeTimeReversal(CFG.get<float>("time_reversal_speed_factor") * time_elapsed))
            {
                this->setGameState(GameState::Normal);
            }

            camera_->update(time_elapsed);
            break;
        }
    }
}

void Game::updateMapObjects(float time_elapsed)
{
    auto& blockage = map_->getMapBlockage();

    auto& obstacles_tiles = map_->getList<ObstacleTile>();
    auto& obstacles =  map_->getList<Obstacle>();;
    auto& npcs =  map_->getList<NPC>();;
    auto& specials = map_->getList<Special>();
    auto& decorations = map_->getList<Decoration>();
    auto& weapons = map_->getList<PlacedWeapon>();

    for (auto it = obstacles_tiles.begin(); it != obstacles_tiles.end();)
    {
        bool do_increment = true;
        if (!(*it)->update(time_elapsed))
        {
            journal_->event<DestroyObstacleTile>(it->get());
            // draw on this place destruction
            spawnDecoration((*it)->getPosition(), "destroyed_wall");
            this->spawnExplosionEvent((*it)->getPosition(), 250.0f);

            auto next_it = std::next(it);
            engine_->deleteStaticObject(it->get());

            auto grid_pos = std::make_pair(std::round((*it)->getPosition().x / DecorationTile::SIZE_X_),
                                           std::round((*it)->getPosition().y / DecorationTile::SIZE_Y_));
            blockage.blockage_.at(grid_pos.first).at(grid_pos.second) = false;

            obstacles_tiles.erase(it);
            it = next_it;
            do_increment = false;
        }

        if (do_increment) ++it;
    }

    for (auto it = obstacles.begin(); it != obstacles.end();)
    {
        bool do_increment = true;
        (*it)->updateAnimation(time_elapsed);
        if (!(*it)->update(time_elapsed))
        {
            journal_->event<DestroyObstacle>(it->get());

            this->spawnExplosionEvent((*it)->getPosition(), 250.0f);

            auto next_it = std::next(it);
            engine_->deleteStaticObject(it->get());

            if ((*it)->getActivation() == "OnKill")
            {
                (*it)->use(player_.get());
            }

            auto grid_pos = std::make_pair(std::round((*it)->getPosition().x / DecorationTile::SIZE_X_),
                                           std::round((*it)->getPosition().y / DecorationTile::SIZE_Y_));
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
    stats_->killEnemy();

    if (player_clone_ != nullptr)
    {
        player_clone_->removeEnemy(npc);
    }

    // draw on this place destruction
    spawnDecoration(npc->getPosition(), "blood");

    this->spawnExplosionEvent(npc->getPosition(), 250.0f);

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

    if (npc->getActivation() == "OnKill" && npc->isActive())
    {
        npc->use(npc);
    }
}

void Game::draw(graphics::Graphics& graphics)
{
    auto draw = [&graphics](auto& list) {
        for (auto& obj : list)
            graphics.drawSorted(*obj);
    };

//    debug_map_blockage_->draw(graphics);
    draw(map_->getList<DecorationTile>());
    draw(map_->getList<Decoration>());
    draw(map_->getList<Obstacle>());
    draw(map_->getList<ObstacleTile>());
    draw(map_->getList<NPC>());
    draw(map_->getList<PlacedWeapon>());
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

    graphics.drawAlreadySorted();
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

const std::list<std::unique_ptr<Bullet>>& Game::getBullets() const
{
    return bullets_;
}

void Game::spawnSparksEvent(const sf::Vector2f& pos, const float dir, const float r)
{
    engine_->spawnAnimationEvent(std::make_shared<Event>(pos, "sparks", dir, r));
}

void Game::spawnExplosionEvent(const sf::Vector2f& pos, const float r)
{
    auto number = std::to_string(utils::num::getRandom(1, 3));
    engine_->spawnAnimationEvent(std::make_shared<Event>(pos, "explosion_" + number, 0.0f, r));

    if (CFG.get<int>("sound/sound_on"))
        engine_->spawnSoundEvent(RM.getSound("wall_explosion"), pos);
}

void Game::spawnTeleportationEvent(const sf::Vector2f& pos)
{
    engine_->spawnAnimationEvent(std::make_shared<Event>(pos + sf::Vector2f{0.0f, 10.0f}, "teleportation"));

    if (CFG.get<int>("sound/sound_on"))
        engine_->spawnSoundEvent(RM.getSound("teleportation"), pos);
}

void Game::spawnFadeInOut()
{
    engine_->spawnEffect(std::make_shared<graphics::FadeInOut>(
            sf::Vector2f{static_cast<float>(CFG.get<int>("graphics/window_width_px")),
                         static_cast<float>(CFG.get<int>("graphics/window_height_px"))}, sf::Color::Black,
            CFG.get<float>("graphics/fade_in_out_duration")
    ));
}

void Game::spawnThought(Character* user, const std::string& text)
{
    ui_->spawnThought(user, text);
}

void Game::spawnAchievement(Achievements::Type type)
{
    ui_->spawnAchievement(
            achievements_->getAchievementTitle(type),
            achievements_->getAchievementText(type),
            achievements_->getAchievementTexture(type)
    );
}

void Game::spawnSpecial(const sf::Vector2f& pos, const std::string& name)
{
    auto ptr = this->spawnNewSpecial(name, -1, pos, "", {}, {});
    journal_->event<SpawnSpecial>(ptr);
}

void Game::spawnShotEvent(const std::string& name, const sf::Vector2f& pos, float dir)
{
    auto shot_event = std::make_shared<Event>(pos, "shot", dir * 180.0f / M_PI,
                                              utils::j3x::get<float>(RM.getObjectParams("bullets", name),
                                                                     "burst_size"));
    engine_->spawnAnimationEvent(shot_event);

    if (CFG.get<int>("sound/sound_on"))
        engine_->spawnSoundEvent(RM.getSound(name + "_bullet_shot"), pos);
}

void Game::spawnBullet(Character* user, const std::string& name, const sf::Vector2f& pos, float dir)
{
    auto ptr = this->spawnNewBullet(user, name, pos, dir);
    journal_->event<SpawnBullet>(ptr);
    this->spawnShotEvent(name, pos, dir);
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

            spawnSparksEvent(bullet->getPosition(), bullet->getRotation() - 90.0f,
                             static_cast<float>(std::pow(
                                     CFG.get<float>("graphics/sparks_size_factor") * bullet->getDeadlyFactor(), 0.4f)));

            bullet->setDead();
        }
        else if (obstacle_tile != nullptr)
        {
            journal_->event<ShotObstacleTile>(obstacle_tile);
            obstacle_tile->getShot(*bullet);

            spawnSparksEvent(bullet->getPosition(), bullet->getRotation() - 90.0f,
                             static_cast<float>(std::pow(
                                     CFG.get<float>("graphics/sparks_size_factor") * bullet->getDeadlyFactor(), 0.4f)));

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
        else if (obstacle_tile != nullptr)
        {
            journal_->event<ShotObstacleTile>(obstacle_tile);
            explosion->applyForce(obstacle_tile);
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

    if (bullet != nullptr && character != nullptr)
    {
        if (bullet->getUser() != character)
        {
            character->getShot(*bullet);
            spawnSparksEvent(bullet->getPosition(), bullet->getRotation() - 90.0f,
                             static_cast<float>(std::pow(
                                     CFG.get<float>("graphics/sparks_size_factor") * bullet->getDeadlyFactor(), 0.4f)));

            bullet->setDead();
        }
        return;
    }

    auto special = dynamic_cast<Special*>(h_obj);

    if (special != nullptr && character != nullptr && special->isActive())
    {
        if (special->getActivation() == "OnEnter")
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
        else
        {
            character->setCurrentSpecialObject(special);
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
        explosion->applyForce(character);
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
        //character->setGlobalState(Character::GlobalState::OnFire);
        if (character != melee_weapon_area->getFather()->getUser())
        {
            spawnSparksEvent(character->getPosition(), 0.0f, 100.0f);
            melee_weapon_area->setActive(false);
            character->getCut(*melee_weapon_area->getFather());
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

    for (const auto& function : ptr->getFunctions())
    {
        ptr->bindFunction(special_functions_->bindFunction(function),
                          special_functions_->bindTextToUse(function),
                          special_functions_->isUsableByNPC(function));
    }

    engine_->registerHoveringObject(ptr);

    return ptr;
}

void Game::spawnFire(Character* user, const std::string& name, const sf::Vector2f& pos, float dir)
{
    auto ptr = this->spawnNewFire(user, pos, dir);
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

    engine_->registerHoveringObject(ptr);

    return ptr;
}


NPC* Game::spawnNewNPC(const std::string& id, int u_id, const std::string& activation,
                       const std::vector<std::string>& funcs, const std::vector<std::string>& datas)
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

    ptr->registerAgentsManager(agents_manager_.get());
    ptr->registerEnemy(player_.get());

    if (player_clone_ != nullptr)
        player_clone_->registerEnemy(ptr);

    ptr->registerMapBlockage(&map_->getMapBlockage());

    for (auto& weapon : ptr->getWeapons())
    {
        if (!weapon->getId().empty())
            weapon->registerSpawningFunction(
                    this->getSpawningFunction(utils::j3x::get<std::string>(
                            RM.getObjectParams("weapons", weapon->getId()), "spawn_func")));

        auto melee_weapon = dynamic_cast<MeleeWeapon*>(weapon.get());

        if (melee_weapon != nullptr)
        {
            engine_->registerHoveringObject(melee_weapon->getMeleeWeaponArea());
        }
    }

    for (const auto& function : ptr->getFunctions())
    {
        ptr->bindFunction(special_functions_->bindFunction(function),
                          special_functions_->bindTextToUse(function),
                          special_functions_->isUsableByNPC(function));
    }

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
                                                  CFG.get<float>("player_clone_time_factor"));
    engine_->registerDynamicObject(player_clone_.get());

    player_clone_->registerAgentsManager(agents_manager_.get());
    player_clone_->registerMapBlockage(&map_->getMapBlockage());

    for (auto& enemy : map_->getList<NPC>())
    {
        player_clone_->registerEnemy(enemy.get());

        enemy->registerEnemy(player_clone_.get());
    }

    player_clone_->makeOnlyOneWeapon(weapon_id, 0.0f);

    for (auto& weapon : player_clone_->getWeapons())
    {
        if (!weapon->getId().empty())
            weapon->registerSpawningFunction(
                    this->getSpawningFunction(utils::j3x::get<std::string>(
                            RM.getObjectParams("weapons", weapon->getId()), "spawn_func")));

        auto melee_weapon = dynamic_cast<MeleeWeapon*>(weapon.get());

        if (melee_weapon != nullptr)
        {
            engine_->registerHoveringObject(melee_weapon->getMeleeWeaponArea());
        }
    }

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
                                 const std::string& activation,
                                 const std::vector<std::string>& funcs, const std::vector<std::string>& datas)
{
    auto new_ptr = map_->spawn<Obstacle>(pos, 0.0f, id);

    if (u_id != -1)
    {
        new_ptr->setUniqueId(u_id);
    }

    if (!activation.empty())
    {
        new_ptr->setActivation(activation);
        new_ptr->setFunctions(funcs);
        new_ptr->setDatas(datas);
    }

    engine_->registerStaticObject(new_ptr);

    for (const auto& function : new_ptr->getFunctions())
    {
        new_ptr->bindFunction(special_functions_->bindFunction(function),
                              special_functions_->bindTextToUse(function),
                              special_functions_->isUsableByNPC(function));
    }

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

    std::cerr << "[Game] Warning - bullet to delete not found!" << std::endl;
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

    std::cerr << "[Game] Warning - fire to delete not found!" << std::endl;
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

    std::cerr << "[Game] Warning - decoration to delete not found!" << std::endl;
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
        auto still_talking = curr->talk(std::bind(&Game::spawnThought, this, std::placeholders::_1,
                std::placeholders::_2),
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
    current_time_factor_ = CFG.get<float>("bullet_time_factor");
    engine_->setTimeScaleFactor(current_time_factor_);

    if (CFG.get<int>("sound/sound_on"))
        music_manager_->setPlaybackPitch(CFG.get<float>("sound/bullet_time_music_factor"));
}

void Game::setNormalTime()
{
    current_time_factor_ = 1.0f;
    engine_->setTimeScaleFactor(1.0f);

    if (CFG.get<int>("sound/sound_on"))
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
                            journal_->getDurationSaved() * CFG.get<float>("player_clone_time_factor"));
                journal_->clear();
            }

            camera_->setNormal();
            engine_->turnOnCollisions();
            break;
        case GameState::Reverse:
            if (this->isJournalFreezed())
                return;

            this->spawnTeleportationEvent(player_->getPosition());

            camera_->setReverse();
            engine_->turnOffCollisions();
            journal_->event<TimeReversal, void*>();
            break;
        case GameState::Paused:
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
    return journal_->getDurationSaved() < CFG.get<float>("journal_min_time");
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

            this->spawnExplosionEvent(player_clone_->getPosition(), 250.0f);
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
        spawnExplosionEvent(player_->getPosition(), 25.0f);
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
            if ((*it)->getActivation() == "OnKill")
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

const Game::SpawningFunction& Game::getSpawningFunction(const std::string& name) const
{
    auto it = spawning_func_.find(name);

    if (it == spawning_func_.end())
    {
        throw std::invalid_argument("[Game] SpawningFunction " + name + " is not handled!");
    }

    return it->second;
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
    return ptr;


}

void Game::spawnDecoration(const sf::Vector2f& pos, const std::string& name)
{
    auto ptr = this->spawnNewDecoration(name, -1, pos);
    journal_->event<SpawnDecoration>(ptr);
}

Special* Game::spawnNewSpecial(const std::string& id, int u_id,
                               const sf::Vector2f& pos, const std::string& activation,
                               const std::vector<std::string>& funcs, const std::vector<std::string>& datas)
{
    auto ptr = map_->spawn<Special>(pos, 0.0f, id);
    engine_->registerHoveringObject(ptr);

    if (u_id != -1)
    {
        ptr->setUniqueId(u_id);
    }

    if (!activation.empty())
    {
        ptr->setActivation(activation);
        ptr->setFunctions(funcs);
        ptr->setDatas(datas);
    }

    for (const auto& function : ptr->getFunctions())
    {
        ptr->bindFunction(special_functions_->bindFunction(function),
                          special_functions_->bindTextToUse(function),
                          special_functions_->isUsableByNPC(function));
    }

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

    std::cerr << "[Game] Warning - special to delete not found!" << std::endl;
}

void Game::registerHoveringObject(HoveringObject* obj)
{
    engine_->registerHoveringObject(obj);
}
