//
// Created by jul3x on 27.02.19.
//

#include <R3E/utils/Geometry.h>

#include <common/ResourceManager.h>
#include <common/weapons/MeleeWeapon.h>
#include <common/events/Event.h>
#include <common/misc/JournalEntries.h>

#include <misc/Stats.h>
#include <Game.h>


Game::Game() : Framework(),
               current_time_factor_(1.0f), rag3_time_elapsed_(-10.0f),
               forced_zoom_to_time_elapsed_(-10.0f), current_obj_zoom_(nullptr),
               time_manipulation_fuel_(0.0f)
{
}

void Game::initialize()
{
    player_ = std::make_unique<Player>(sf::Vector2f{0.0f, 0.0f});
    player_->setName(CONF<std::string>("general/player_name"));
    time_manipulation_fuel_ = player_->getMaxTimeManipulation();
    Framework::initialize();
    engine_->initializeSoundManager(CONF<float>("sound/sound_attenuation"));
    ui_ = std::make_unique<GameUserInterface>(this);
    journal_ = std::make_unique<Journal>(this, CONF<float>("journal_max_time"), CONF<float>("journal_sampling_rate"));

    stats_ = std::make_unique<Stats>(this);
    achievements_ = std::make_unique<Achievements>(dynamic_cast<Stats*>(stats_.get()));
    achievements_->load(CONF<std::string>("paths/achievements"));

    music_manager_ = std::make_unique<audio::MusicManager>();
    music_manager_->addToQueue(CONF<std::string>("paths/music_dir") + "/menu.ogg");
    music_manager_->setVolume(CONF<float>("sound/music_volume"));
    if (CONF<bool>("sound/sound_on"))
        music_manager_->play();

    ui_->registerCamera(camera_.get());
    ui_->registerPlayer(player_.get());
    engine_->registerUI(ui_.get());

    this->setStartingPosition();
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

            weather_->update(time_elapsed);

            if (rag3_time_elapsed_ > 0.0f)
                rag3_time_elapsed_ -= time_elapsed;
            else if (rag3_time_elapsed_ > -1.0f)
                setRag3Time(-10.0f);

            if (forced_zoom_to_time_elapsed_ > 0.0f)
            {
                forced_zoom_to_time_elapsed_ -= time_elapsed;
                forceZoomTo(current_obj_zoom_);
            }
            else if (forced_zoom_to_time_elapsed_ > -1.0f)
            {
                camera_->setPointingTo(player_->getPosition());
                camera_->setZoomTo(1.0f);
                this->setNormalTime();
                forced_zoom_to_time_elapsed_ = -10.0f;
                current_obj_zoom_ = nullptr;
            }

            if (current_time_factor_ == 1.0f)
                time_manipulation_fuel_ =
                        std::min(time_manipulation_fuel_ + player_->getTimeManipulationFuelSpeed() * time_elapsed,
                                 player_->getMaxTimeManipulation());
            else
                time_manipulation_fuel_ -= CONF<float>("characters/time_manipulation_slow_use_speed") * time_elapsed;

            time_elapsed_ += time_elapsed;
            break;
        }
        case GameState::Reverse:
        {
            time_manipulation_fuel_ -= CONF<float>("time_reversal_speed_factor") * time_elapsed;
            if (!journal_->executeTimeReversal(CONF<float>("time_reversal_speed_factor") * time_elapsed) ||
                time_manipulation_fuel_ <= 0.0f)
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
    Framework::updateMapObjects(time_elapsed);
    auto& npcs = map_->getList<NPC>();;

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

        if (do_increment)
            ++it;
    }
}

void Game::killNPC(NPC* npc)
{
    journal_->event<DestroyCharacter>(npc);
    stats_->killEnemy(npc->getId(), npc->getPosition());
    ui_->removeArrowIfExists(npc);
    if (player_clone_ != nullptr)
    {
        player_clone_->removeEnemy(npc);
    }

    // draw on this place destruction
    spawnDecoration(npc->getPosition(), "blood");
    spawnKillEvent(npc->getPosition());
    spawnSound(RM.getSound(npc->getId() + "_dead"), npc->getPosition());

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
                    this->lighting_->add(*light);
            }
        };

//        debug_map_blockage_->draw(graphics);
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

        if (player_->isAlive() and state_ != GameState::Reverse)
            graphics.drawSorted(*player_);

        if (player_clone_ != nullptr)
            graphics.drawSorted(*player_clone_);

        engine_->drawSortedAnimationEvents();
        graphics.drawSorted(*weather_);

        graphics.drawAlreadySorted(states.shader);


        lighting_->clear();

        draw_light(map_->getList<NPC>());
        draw_light(map_->getList<Obstacle>());
        draw_light(map_->getList<Decoration>());
        draw_light(map_->getList<Special>());
        draw_light(fire_);
        draw_light(engine_->getAnimationEvents());

        if (player_->getLightPoint() != nullptr)
            lighting_->add(*player_->getLightPoint());
        if (player_clone_ != nullptr && player_clone_->getLightPoint() != nullptr)
            lighting_->add(*player_clone_->getLightPoint());

        graphics.setStaticView();
        graphics.draw(*lighting_);
    }
}

Player* Game::getPlayer()
{
    return player_.get();
}

Journal* Game::getJournal()
{
    return journal_.get();
}

DestructionSystem* Game::spawnSparksEvent(const sf::Vector2f& pos, const float dir, const float r)
{
    auto ptr = Framework::spawnSparksEvent(pos, dir, r);
    journal_->event<SpawnDestructionSystem>(ptr);
    return ptr;
}

void Game::spawnAchievement(const j3x::Parameters& params)
{
    ui_->spawnAchievement(
            j3x::get<std::string>(params, "title"),
            j3x::get<std::string>(params, "description"),
            j3x::get<std::string>(params, "texture")
    );
}

Special* Game::spawnSpecial(const sf::Vector2f& pos, const std::string& name)
{
    auto ptr = Framework::spawnSpecial(pos, name);
    journal_->event<SpawnSpecial>(ptr);
    return ptr;
}

DestructionSystem* Game::spawnBloodEvent(const sf::Vector2f& pos, float dir, float deadly_factor)
{
    auto ptr = Framework::spawnBloodEvent(pos, dir, deadly_factor);
    journal_->event<SpawnDestructionSystem>(ptr);
    return ptr;
}

Bullet* Game::spawnBullet(Character* user, const std::string& name, const sf::Vector2f& pos, float dir)
{
    auto ptr = Framework::spawnBullet(user, name, pos, dir);
    journal_->event<SpawnBullet>(ptr);
    return ptr;
}

Fire* Game::spawnFire(Character* user, const std::string& name, const sf::Vector2f& pos, float dir)
{
    auto ptr = Framework::spawnFire(user, name, pos, dir);
    journal_->event<SpawnFire>(ptr);
    return ptr;
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
    if (player_clone_ != nullptr)
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
        ui_->removeArrowIfExists(curr);
    }
}

void Game::talk()
{
    auto curr = player_->getCurrentTalkableCharacter();
    if (curr != nullptr)
    {
        auto still_talking = curr->talk(
                [this](Character* character, const std::string& text) { this->spawnThought(character, text); },
                player_.get());

        if (!still_talking)
        {
            engine_->deleteHoveringObject(curr->getTalkableArea());
            curr->use(curr);
            curr->deactivate();
            ui_->removeArrowIfExists(curr);
        }
    }
}

bool Game::setBulletTime()
{
    if (current_time_factor_ == 1.0f && time_manipulation_fuel_ < CONF<float>("bullet_time_min_time"))
        return false;
    current_time_factor_ = CONF<float>("bullet_time_factor");
    engine_->setTimeScaleFactor(current_time_factor_);

    if (CONF<bool>("sound/sound_on"))
        music_manager_->setPlaybackPitch(CONF<float>("sound/bullet_time_music_factor"));
    return true;
}

void Game::setNormalTime()
{
    current_time_factor_ = 1.0f;
    engine_->setTimeScaleFactor(1.0f);

    if (CONF<bool>("sound/sound_on"))
        music_manager_->setPlaybackPitch(1.0f);
}

void Game::setGameState(Framework::GameState state)
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

                this->spawnTeleportationEvent(player_->getPosition());

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
            {
                music_manager_->setPlaybackPitch(1.0f);
                music_manager_->play();
            }

            break;
        case GameState::Reverse:
            if (this->isJournalFreezed() || current_time_factor_ != 1.0f || this->getTimeManipulationFuel() <= 0.0f)
                return;

            this->spawnTeleportationEvent(player_->getPosition());

            camera_->setReverse();
            engine_->turnOffCollisions();
            journal_->event<TimeReversal, void*>();
            break;
        case GameState::Menu:
            if (CONF<bool>("sound/sound_on"))
            {
                music_manager_->clearQueue();
                music_manager_->addToQueue(CONF<std::string>("paths/music_dir") + "/menu.ogg");
                music_manager_->play();
                music_manager_->setPlaybackPitch(1.0f);
            }

            break;
        case GameState::Paused:
            if (state_ != GameState::Paused)
                if (CONF<bool>("sound/sound_on"))
                    music_manager_->setPlaybackPitch(CONF<float>("sound/bullet_time_music_factor"));
            break;
    }

    state_ = state;
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
            spawnKillEvent(player_clone_->getPosition());

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
        spawnKillEvent(player_->getPosition());
        player_->setDead();
        engine_->deleteDynamicObject(player_.get());
        music_manager_->setPlaybackPitch(CONF<float>("sound/bullet_time_music_factor"));
    }
}

Decoration* Game::spawnDecoration(const sf::Vector2f& pos, const std::string& name)
{
    auto ptr = Framework::spawnDecoration(pos, name);
    journal_->event<SpawnDecoration>(ptr);
    return ptr;
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
    utils::eraseIf<std::unique_ptr<DestructionSystem>>(destruction_systems_, [&time_elapsed, this](
            std::unique_ptr<DestructionSystem>& system) {
        if (!system->update(time_elapsed))
        {
            journal_->event<DestroyDestructionSystem>(system.get());
            return true;
        }
        return false;
    });
}

void Game::forceZoomTo(AbstractPhysicalObject* current_zoom)
{
    camera_->setPointingTo(current_zoom->getPosition());
    camera_->setZoomTo(CONF<float>("graphics/camera_right_click_zoom_factor"));

    this->setBulletTime();

    if (current_obj_zoom_ == nullptr)
        forced_zoom_to_time_elapsed_ = CONF<float>("forced_zoom_to_time");

    current_obj_zoom_ = current_zoom;
}

float Game::getForcedZoomTime() const
{
    return forced_zoom_to_time_elapsed_;
}

void Game::initPlayers()
{
    engine_->registerDynamicObject(player_.get());
    registerLight(player_.get());
    registerWeapons(player_.get());
}

void Game::initNPCs()
{
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
}

void Game::close()
{
    map_->getList<NPC>().clear();
    Framework::close();
}

DestructionSystem* Game::spawnSparksEvent2(const sf::Vector2f& pos, float dir, float r)
{
    auto ptr = Framework::spawnSparksEvent2(pos, dir, r);
    journal_->event<SpawnDestructionSystem>(ptr);
    return ptr;
}

float Game::getTimeManipulationFuel() const
{
    return time_manipulation_fuel_;
}

void Game::respawn(const std::string& map_name)
{
    player_ = std::make_unique<Player>(sf::Vector2f{0.0f, 0.0f});
    player_->setName(CONF<std::string>("general/player_name"));
    ui_->clearThoughts();
    ui_->registerPlayer(player_.get());
    time_manipulation_fuel_ = player_->getMaxTimeManipulation();
    setRag3Time(0.0f);
    map_->getList<NPC>().clear();

    map_->loadMap(map_name.empty() ? map_->getMapName() : map_name);
    agents_manager_ = std::make_unique<ai::AgentsManager>(&map_->getMapBlockage(), ai::AStar::EightNeighbours,
                                                          CONF<float>("characters/max_time_without_path_recalc"),
                                                          CONF<float>("characters/min_pos_change_without_path_recalc"),
                                                          CONF<int>("characters/max_path_search_depth"));
    engine_->getGraphics().setBgColor(sf::Color(j3x::get<int>(map_->getParams(), "background_color")));
    lighting_ = std::make_unique<graphics::Lighting>(
            sf::Vector2f{static_cast<float>(CONF<int>("graphics/window_width_px")),
                         static_cast<float>(CONF<int>("graphics/window_height_px"))},
            sf::Color(j3x::get<int>(map_->getParams(), "lighting_color")));

//    debug_map_blockage_ = std::make_unique<DebugMapBlockage>(&map_->getMapBlockage());

    engine_->initializeCollisions(map_->getSize(), CONF<float>("collision_grid_size"));
    initObstacles();
    initDecorations();
    initPlayers();
    initNPCs();
    initWeapons();
    initSpecials();

    this->setStartingPosition();

    journal_->clear();
    this->cleanPlayerClone();

    // TODO - after saving, stats should not be zeroed but restored to previous state
    stats_->setEnemiesKilled(0);
    stats_->setCrystalsPicked(0);
    stats_->setExplosions(0);
    stats_->setExp(0);
    stats_->setLevel(0);

    ui_->initializeTutorialArrows();
    setGameState(Game::GameState::Normal);
}

void Game::setStartingPosition()
{
    for (auto& special : map_->getList<Special>())
    {
        if (special->getId() == "starting_position")
            player_->setPosition(special->getPosition());
    }
}
