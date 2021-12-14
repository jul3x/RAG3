//
// Created by jul3x on 27.02.19.
//

#include <R3E/utils/Geometry.h>

#include <common/ResourceManager.h>
#include <common/weapons/MeleeWeapon.h>
#include <common/weapons/NoWeapon.h>
#include <common/events/Event.h>
#include <common/misc/JournalEntries.h>

#include <misc/Stats.h>
#include <Game.h>


Game::Game() : Framework(),
               current_time_factor_(1.0f), rag3_time_elapsed_(-10.0f),
               forced_zoom_to_time_elapsed_(-10.0f), current_obj_zoom_(nullptr),
               time_manipulation_fuel_(0.0f), is_playing_previous_map_(false)
{
}

void Game::initialize()
{
    player_ = std::make_unique<Player>(sf::Vector2f{0.0f, 0.0f});
    Framework::initialize();

    ui_ = std::make_unique<GameUserInterface>(this);
    ui_->registerCamera(camera_.get());
    ui_->registerPlayer(player_.get());
    engine_->registerUI(ui_.get());

    journal_ = std::make_unique<Journal>(this, CONF<float>("journal_max_time"), CONF<float>("journal_sampling_rate"));

    stats_ = std::make_unique<Stats>(this);
    achievements_ = std::make_unique<Achievements>(dynamic_cast<Stats*>(stats_.get()));
    achievements_->load(CONF<std::string>("paths/achievements"));

    this->preloadSave();
}

void Game::beforeUpdate(float time_elapsed)
{
    Framework::beforeUpdate(time_elapsed);
    agents_manager_->update();
}

void Game::afterUpdate(float time_elapsed)
{
    Framework::afterUpdate(time_elapsed);
    achievements_->update(time_elapsed);
    journal_->update(time_elapsed);
    weather_->update(time_elapsed);

    if (rag3_time_elapsed_ > 0.0f)
        rag3_time_elapsed_ -= time_elapsed;
    else if (rag3_time_elapsed_ > -1.0f)
        setRag3Time(-10.0f);

    if (current_time_factor_ == 1.0f)
        time_manipulation_fuel_ =
                std::min(time_manipulation_fuel_ + player_->getTimeManipulationFuelSpeed() * time_elapsed,
                         player_->getMaxTimeManipulation());
    else
        time_manipulation_fuel_ -= CONF<float>("characters/time_manipulation_slow_use_speed") * time_elapsed;
}

void Game::preupdate(float time_elapsed)
{
    Framework::preupdate(time_elapsed);
    if (CONF<bool>("sound/sound_on"))
    {
        music_manager_->update(time_elapsed);
    }
}

void Game::updateCamera(float time_elapsed)
{
    Framework::updateCamera(time_elapsed);

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
}

void Game::updateTimeReversal(float time_elapsed)
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

    auto update_animations = [time_elapsed](auto& list) {
        for (auto& obj : list)
        {
            obj->updateAnimation(time_elapsed, -CONF<float>("time_reversal_speed_factor"));
        }
    };

    update_lights(map_->getList<Obstacle>());
    update_lights(map_->getList<NPC>());
    update_lights(map_->getList<Special>());
    update_lights(map_->getList<Decoration>());
    update_lights(fire_);

    update_animations(map_->getList<Special>());
    update_animations(map_->getList<Decoration>());

    auto player_light = player_->getLightPoint();
    if (player_light != nullptr)
        player_light->setPosition(player_->getPosition());

    if (player_clone_ != nullptr)
    {
        auto clone_light = player_clone_->getLightPoint();

        if (clone_light != nullptr)
            clone_light->setPosition(player_clone_->getPosition());
    }

    Framework::updateTimeReversal(time_elapsed);
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

    this->unregisterCharacter(npc);

    // draw on this place destruction
    spawnDecoration(npc->getPosition(), "blood");
    spawnKillEvent(npc->getPosition());
    spawnSound(RM.getSound(npc->getId() + "_dead"), npc->getPosition());

    if (npc->getActivation() == Functional::Activation::OnKill && npc->isActive())
    {
        npc->use(npc);
    }
}

Player* Game::getPlayer()
{
    return player_.get();
}

PlayerClone* Game::getPlayerClone()
{
    return player_clone_.get();
}

Journal* Game::getJournal()
{
    return journal_.get();
}

DestructionSystem* Game::spawnSparksEvent(const sf::Vector2f& pos, const float dir, const float r)
{
    auto ptr = Framework::spawnSparksEvent(pos, dir, r);
    journal_->event<SpawnEntry<DestructionSystem>>(ptr);
    return ptr;
}

DestructionSystem* Game::spawnSparksEvent2(const sf::Vector2f& pos, float dir, float r)
{
    auto ptr = Framework::spawnSparksEvent2(pos, dir, r);
    journal_->event<SpawnEntry<DestructionSystem>>(ptr);
    return ptr;
}

Decoration* Game::spawnDecoration(const sf::Vector2f& pos, const std::string& name)
{
    auto ptr = Framework::spawnDecoration(pos, name);
    journal_->event<SpawnEntry<Decoration>>(ptr);
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

Special* Game::spawnSpecial(const sf::Vector2f& pos, const std::string& name, int uid)
{
    auto ptr = Framework::spawnSpecial(pos, name, uid);
    journal_->event<SpawnEntry<Special>>(ptr);
    return ptr;
}

DestructionSystem* Game::spawnBloodEvent(const sf::Vector2f& pos, float dir, float deadly_factor)
{
    auto ptr = Framework::spawnBloodEvent(pos, dir, deadly_factor);
    journal_->event<SpawnEntry<DestructionSystem>>(ptr);
    return ptr;
}

Bullet* Game::spawnBullet(Character* user, const std::string& name, const sf::Vector2f& pos, float dir)
{
    auto ptr = Framework::spawnBullet(user, name, pos, dir);
    journal_->event<SpawnEntry<Bullet>>(ptr);
    return ptr;
}

Fire* Game::spawnFire(Character* user, const std::string& name, const sf::Vector2f& pos, float dir)
{
    auto ptr = Framework::spawnFire(user, name, pos, dir);
    journal_->event<SpawnEntry<Fire>>(ptr);
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
    engine_->registerObj<DynamicObject>(ptr);
    registerLight(ptr);

    ptr->registerAgentsManager(agents_manager_);
    ptr->registerEnemy(player_.get());

    if (player_clone_ != nullptr)
        player_clone_->registerEnemy(ptr);

    ptr->registerMapBlockage(&map_->getMapBlockage());
    registerWeapons(ptr);
    registerFunctions(ptr);

    auto talkable_area = ptr->getTalkableArea();
    if (talkable_area != nullptr)
        engine_->registerObj<HoveringObject>(talkable_area);

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
    engine_->registerObj<DynamicObject>(player_clone_.get());
    registerLight(player_clone_.get());

    player_clone_->registerAgentsManager(agents_manager_);
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
        this->unregisterCharacter(player_clone_.get(), false);

        for (auto& enemy : map_->getList<NPC>())
        {
            enemy->removeEnemy(player_clone_.get());
        }
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
            unregisterTalkableArea(curr);
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
                this->spawnEvent("melee_giorgio_swirl", player_->getPosition(), 0.0f, 1.0f, false);

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

            if (state_ == GameState::Reverse)
                camera_->setShaking();

            engine_->turnOnCollisions();
            if (CONF<bool>("sound/sound_on"))
            {
                music_manager_->setPlaybackPitch(1.0f);
                music_manager_->play();
            }

            break;
        case GameState::Reverse:
            if (!this->isNormalGameplay())
                return;

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

            // Needs to be done to avoid segfault
            this->cleanPlayerClone();
            for (const auto& character : map_->getList<NPC>())
                this->unregisterCharacter(character.get());
            map_->getList<NPC>().clear();

            break;
        case GameState::Paused:
            if (state_ != GameState::Paused)
                if (CONF<bool>("sound/sound_on"))
                    music_manager_->setPlaybackPitch(CONF<float>("sound/bullet_time_music_factor"));
            break;
    }

    state_ = state;
}

void Game::updatePlayers(float time_elapsed)
{
    player_->setCurrentSpecialObject(nullptr);
    player_->setCurrentTalkableCharacter(nullptr);
    if (player_->isAlive() && !player_->update(time_elapsed))
    {
        this->killPlayer(player_.get());
        music_manager_->setPlaybackPitch(CONF<float>("sound/bullet_time_music_factor"));
    }

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

void Game::updateFire(float time_elapsed)
{
    for (auto it = fire_.begin(); it != fire_.end(); ++it)
    {
        if (!(*it)->update(time_elapsed))
        {
            engine_->unregisterObj<HoveringObject>(it->get());
            journal_->event<DestroyFire>(it->get());
            auto next_it = std::next(it);
            fire_.erase(it);
            it = next_it;
        }
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

void Game::initNPCs()
{
    for (auto& character : map_->getList<NPC>())
    {
        character->clearEnemies();
        engine_->registerObj<DynamicObject>(character.get());
        registerLight(character.get());
        character->registerAgentsManager(agents_manager_);
        character->registerEnemy(player_.get());
        character->registerMapBlockage(&map_->getMapBlockage());
        registerFunctions(character.get());
        registerWeapons(character.get());

        auto talkable_area = character->getTalkableArea();
        if (talkable_area != nullptr)
        {
            engine_->registerObj<HoveringObject>(talkable_area);
        }
    }
}

void Game::close()
{
    map_->getList<NPC>().clear();
    Framework::close();
}

float Game::getTimeManipulationFuel() const
{
    return time_manipulation_fuel_;
}

void Game::respawn(const std::string& map_name)
{
    player_ = std::make_unique<Player>(sf::Vector2f{0.0f, 0.0f});
    player_->setName(CONF<std::string>("general/player_name"));
    ui_->registerPlayer(player_.get());
    time_manipulation_fuel_ = player_->getMaxTimeManipulation();

    Framework::respawn(map_name);

    this->setStartingPosition();

    journal_->clear();
    this->cleanPlayerClone();
    this->loadSave();
}

void Game::setStartingPosition()
{
    for (auto& special : map_->getList<Special>())
    {
        if (special->getId() == "starting_position")
            player_->setPosition(special->getPosition());
    }
}

void Game::preloadSave()
{
    achievements_->setAchievementsUnlocked(CONF<j3x::List>("save/achievements_unlocked"));
    stats_->setEnemiesKilled(CONF<int>("save/kills"));
    stats_->setCrystalsPicked(CONF<int>("save/crystals"));
    stats_->setExplosions(CONF<int>("save/explosions"));
    stats_->setExp(CONF<int>("save/exp"));
    stats_->setLevel(CONF<int>("save/level"));
    achievements_->rotate();
}

void Game::loadSave()
{
    preloadSave();

    std::vector<std::shared_ptr<AbstractWeapon>> weapons;
    bool has_any_saved_weapons = false;
    for (const auto& weapon_data : CONF<j3x::List>("save/weapons"))
    {
        const auto& data = j3x::getObj<j3x::List>(weapon_data);
        const auto& id = j3x::getObj<std::string>(data, 0);
        const auto& state = j3x::getObj<float>(data, 1);
        const auto& upgrades = j3x::getObj<j3x::List>(data, 2);

        auto weapon = AbstractWeapon::create(player_.get(), j3x::getObj<std::string>(id));
        weapon->setState(state);
        for (const auto& upgrade : upgrades)
            weapon->upgrade(j3x::getObj<std::string>(upgrade));
        weapons.emplace_back(weapon);

        if (dynamic_cast<NoWeapon*>(weapon.get()) == nullptr)
            has_any_saved_weapons = true;
    }

    if (has_any_saved_weapons && this->isNormalGameplay())
    {
        this->unregisterWeapons(player_.get());
        player_->clearWeapons();
        player_->setWeapons(weapons);
        this->registerWeapons(player_.get());
    }

    for (const auto& item : CONF<j3x::List>("save/backpack"))
    {
        const auto& data = j3x::getObj<j3x::List>(item);
        const auto& id = j3x::getObj<std::string>(data, 0);
        const auto& state = j3x::getObj<int>(data, 1);
        player_->addSpecialToBackpack(id, state,
                                      [this](Functional* functional) { this->registerFunctions(functional); });
    }

    player_->setSkillPoints(CONF<int>("save/skill_points"));
    for (const auto& skill : CONF<j3x::List>("save/skills"))
    {
        const auto& data = j3x::getObj<j3x::List>(skill);
        const auto& id = j3x::getObj<std::string>(data, 0);
        const auto& state = j3x::getObj<int>(data, 1);
        player_->setSkill(id, state);
    }
}

void Game::saveState(bool presave)
{
    std::string out;
    const auto& maps = CONF<j3x::List>("maps_order");

    auto previous_maps = map_->getPreviousMapsAndCurrent();
    if (presave)
        previous_maps.emplace_back(map_->getNextMapName());
    r3e::j3x::serializeAssign("maps_unlocked", previous_maps, out);
    r3e::j3x::serializeAssign("achievements_unlocked", achievements_->getAchievementsUnlocked(), out);
    r3e::j3x::serializeAssign("exp", stats_->getExp(), out);
    r3e::j3x::serializeAssign("level", stats_->getLevel(), out);
    r3e::j3x::serializeAssign("kills", stats_->getEnemiesKilled(), out);
    r3e::j3x::serializeAssign("crystals", stats_->getCrystalsPicked(), out);
    r3e::j3x::serializeAssign("explosions", stats_->getExplosions(), out);
    r3e::j3x::serializeAssign("skill_points", player_->getSkillPoints(), out);
    r3e::j3x::serializeAssign("skills", player_->getSkills(), out);
    r3e::j3x::serializeAssign("weapons", player_->getWeaponsToSerialize(), out);
    r3e::j3x::serializeAssign("backpack", player_->getBackpackToSerialize(), out);

    RM.saveConfigFile("user_dir", "save", out);
    CFG.appendConfig("../data/config/user/save.j3x", "save", true);
}

void Game::finishMap()
{
    Framework::finishMap();

    if (is_playing_previous_map_)
        ui_->openMenu();
    else
    {
        const auto& new_map = map_->getNextMapName();
        if (!new_map.empty())
        {
            this->saveState(true);
            this->respawn(map_->getNextMapName());
        }
    }
}

void Game::startGame(const std::string& map_name)
{
    Framework::startGame(map_name);

    is_playing_previous_map_ = !map_name.empty();

    this->respawn(!is_playing_previous_map_ ? j3x::getObj<std::string>(CONF<j3x::List>("save/maps_unlocked").back()) :
                  map_name);
    ui_->startGame();
}

void Game::extraRenderStatesManipulations(sf::RenderStates& states)
{
    if (rag3_time_elapsed_ > 0.0f)
    {
        auto shader = &RM.getShader("rag3.frag");
        shader->setUniform("time", rag3_time_elapsed_);
        shader->setUniform("rag3_time", CONF<float>("rag3_time"));
        states.shader = shader;
    }
    else
    {
        Framework::extraRenderStatesManipulations(states);
    }
}

void Game::drawAdditionalPlayers(graphics::Graphics& graphics)
{
    if (player_clone_ != nullptr)
        graphics.drawSorted(*player_clone_);
}

void Game::drawAdditionalPlayersLighting()
{
    if (player_clone_ != nullptr && player_clone_->getLightPoint() != nullptr)
        lighting_->add(*player_clone_->getLightPoint());
}

void Game::initSound(bool force)
{
    Framework::initSound(force);

    if (force)
    {
        music_manager_ = std::make_unique<audio::MusicManager>();
        music_manager_->addToQueue(CONF<std::string>("paths/music_dir") + "/menu.ogg");
    }

    music_manager_->setVolume(CONF<float>("sound/music_volume"));
    if (force && CONF<bool>("sound/sound_on"))
        music_manager_->play();
}

bool Game::isMulti() const
{
    return false;
}
