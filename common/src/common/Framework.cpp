//
// Created by jul3x on 27.02.19.
//

#include <R3E/utils/Geometry.h>

#include <common/ResourceManager.h>
#include <common/Framework.h>

#include <common/events/Event.h>
#include <common/misc/Journal.h>
#include <common/misc/JournalEntries.h>


Framework::Framework() : time_elapsed_(0.0f), state_(Framework::GameState::Menu), should_finish_map_(false)
{
    engine_ = std::make_unique<Engine>();
    engine_->registerGame(this);
}

void Framework::initialize()
{
    engine_->initializeGraphics(
            sf::Vector2i{CONF<int>("graphics/window_width_px"), CONF<int>("graphics/window_height_px")},
            "Codename: Rag3",
            CONF<bool>("graphics/full_screen") ? sf::Style::Fullscreen : sf::Style::Default,
            sf::Color(CONF<int>("graphics/background_color")));
    engine_->setFPSLimit(60);

    spawning_func_["bullet"] = [this](Character* user, const std::string& name, const sf::Vector2f& pos, float dir) {
        this->spawnBullet(user, name, pos, dir);
    };
    spawning_func_["fire"] = [this](Character* user, const std::string& name, const sf::Vector2f& pos, float dir) {
        this->spawnFire(user, name, pos, dir);
    };
    spawning_func_["null"] = [this](Character* user, const std::string& name, const sf::Vector2f& pos, float dir) {
        this->spawnNull(user, name, pos, dir);
    };

    camera_ = std::make_unique<Camera>();
    special_functions_ = std::make_unique<SpecialFunctions>(this);
    map_ = std::make_unique<Map>();

    engine_->registerCamera(camera_.get());

    this->initParams();
    weather_ = std::make_unique<WeatherSystem>(this, weather_params_.get());
}

void Framework::update(float time_elapsed)
{

}

void Framework::updateMapObjects(float time_elapsed)
{
    auto& blockage = map_->getMapBlockage();

    auto& obstacles = map_->getList<Obstacle>();;
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
            obstacleDestroyedEvent(it->get());
            if (this->getJournal() != nullptr)
                this->getJournal()->event<DestroyObstacle>(it->get());

            this->spawnExplosionEvent((*it)->getPosition());

            auto next_it = std::next(it);
            engine_->unregisterObj<StaticObject>(it->get());

            if ((*it)->getActivation() == Functional::Activation::OnKill)
            {
                (*it)->use(this->getPlayer());
            }

            Map::markBlocked(blockage.blockage_, (*it)->getPosition() +
                                                 RMGET<sf::Vector2f>("obstacles", (*it)->getId(), "collision_offset"),
                             RMGET<sf::Vector2f>("obstacles", (*it)->getId(), "collision_size"), 0.0f);

            obstacles.erase(it);
            it = next_it;
            do_increment = false;
        }

        if (do_increment)
            ++it;
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
            if (this->getJournal() != nullptr)
                this->getJournal()->event<DestroySpecial>(it->get());
            auto next_it = std::next(it);
            engine_->unregisterObj<HoveringObject>(it->get());

            specials.erase(it);
            it = next_it;
            do_increment = false;
        }

        if (do_increment)
            ++it;
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
            if (this->getJournal() != nullptr)
                this->getJournal()->event<DestroyDecoration>(it->get());
            auto next_it = std::next(it);

            decorations.erase(it);
            it = next_it;
            do_increment = false;
        }

        if (do_increment)
            ++it;
    }

    for (auto& weapon : weapons)
    {
        weapon->update(time_elapsed);
    }
}

void Framework::updateBullets(float time_elapsed)
{
    for (auto it = bullets_.begin(); it != bullets_.end(); ++it)
    {
        if (!(*it)->update(time_elapsed))
        {
            if ((*it)->getActivation() == Functional::Activation::OnKill)
            {
                (*it)->use(this->getPlayer());
            }

            if (getJournal() != nullptr)
                getJournal()->event<DestroyBullet>(it->get());
            engine_->unregisterObj<HoveringObject>(it->get());
            auto next_it = std::next(it);
            bullets_.erase(it);
            it = next_it;
        }
    }
}

void Framework::updateFire(float time_elapsed)
{
    for (auto it = fire_.begin(); it != fire_.end(); ++it)
    {
        if (!(*it)->update(time_elapsed))
        {
            engine_->unregisterObj<HoveringObject>(it->get());
            auto next_it = std::next(it);
            fire_.erase(it);
            it = next_it;
        }
    }
}

void Framework::updateExplosions()
{
    for (const auto& explosion : explosions_)
    {
        engine_->unregisterObj<HoveringObject>(explosion.get());
    }
    explosions_.clear();

    for (const auto& desired_explosion : desired_explosions_)
    {
        camera_->setShaking(1.5f);
        explosions_.emplace_back(std::make_unique<Explosion>(desired_explosion.first, desired_explosion.second));
        engine_->registerObj<HoveringObject>(explosions_.back().get());
    }

    desired_explosions_.clear();
}

void Framework::updateDestructionSystems(float time_elapsed)
{
    utils::eraseIf<std::unique_ptr<DestructionSystem>>(destruction_systems_, [&time_elapsed, this](
            std::unique_ptr<DestructionSystem>& system) {
        if (!system->update(time_elapsed))
        {
            return true;
        }
        return false;
    });
}

void Framework::draw(graphics::Graphics& graphics)
{
}

void Framework::start()
{
    engine_->start();
}

Map* Framework::getMap()
{
    return map_.get();
}

Camera* Framework::getCamera()
{
    return camera_.get();
}

Journal* Framework::getJournal()
{
    return nullptr;
}

AbstractStats* Framework::getStats()
{
    return stats_.get();
}

const std::list<std::unique_ptr<Bullet>>& Framework::getBullets() const
{
    return bullets_;
}

void Framework::alertCollision(HoveringObject* h_obj, StaticObject* s_obj)
{
    auto bullet = dynamic_cast<Bullet*>(h_obj);
    auto obstacle = dynamic_cast<Obstacle*>(s_obj);
    auto obstacle_tile = dynamic_cast<ObstacleTile*>(s_obj);

    if (bullet != nullptr)
    {
        if (obstacle != nullptr)
        {
            if (getJournal() != nullptr)
                getJournal()->event<ShotObstacle>(obstacle);
            obstacle->getShot(*bullet);

            spawnSparksEvent(bullet->getPosition(), bullet->getRotation() - 90.0f, 0.0f);
            spawnSparksEvent2(bullet->getPosition(), bullet->getRotation() - 90.0f, 0.0f);
            bullet->setDead();
        }
        else if (obstacle_tile != nullptr)
        {
            spawnSparksEvent(bullet->getPosition(), bullet->getRotation() - 90.0f, 0.0f);
            spawnSparksEvent2(bullet->getPosition(), bullet->getRotation() - 90.0f, 0.0f);
            bullet->setDead();
        }
        return;
    }

    auto explosion = dynamic_cast<Explosion*>(h_obj);
    if (explosion != nullptr)
    {
        if (obstacle != nullptr)
        {
            if (getJournal() != nullptr)
                getJournal()->event<ShotObstacle>(obstacle);
            explosion->applyForce(obstacle);
        }
        return;
    }

    auto fire = dynamic_cast<Fire*>(h_obj);
    if (fire != nullptr)
    {
        fire->setDead();
    }
}

void Framework::alertCollision(HoveringObject* h_obj, DynamicObject* d_obj)
{
    auto bullet = dynamic_cast<Bullet*>(h_obj);
    auto character = dynamic_cast<Character*>(d_obj);

    auto factor = this->getRag3Time() > 0.0f ? CONF<float>("characters/rag3_factor") : 1.0f;

    if (bullet != nullptr && character != nullptr)
    {
        if (bullet->getUser() != character)
        {
            if (bullet->getUser() != getPlayer())
            {
                if (character == getPlayer())
                {
                    static auto strength_skill_factor = CONF<float>("characters/strength_skill_factor");
                    factor = 1.0f / factor;
                    factor = factor * (strength_skill_factor - getPlayer()->getSkill(Player::Skills::Strength)) /
                             strength_skill_factor;
                }
                else
                    factor = 1.0f;
            }

            character->getShot(*bullet, factor);

            float offset = bullet->getRotation() > 0.0f && bullet->getRotation() < 180.0f ? -5.0f : 5.0f;
            spawnBloodEvent(character->getPosition() + sf::Vector2f(0.0f, offset), bullet->getRotation() + 180.0f,
                            bullet->getDeadlyFactor() * factor);

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
                spawnSound(RM.getSound("collect"), special->getPosition());
                player->addSpecialToBackpack(special->getId(), 1,
                                             [this](Functional* functional) { this->registerFunctions(functional); });
                special_functions_->destroy(special, {}, player);
            }
        }
        return;
    }

    auto talkable_area = dynamic_cast<TalkableArea*>(h_obj);

    if (talkable_area != nullptr && character != nullptr)
    {
        character->setCurrentTalkableCharacter(talkable_area->getFather());
        return;
    }

    auto explosion = dynamic_cast<Explosion*>(h_obj);

    if (character != nullptr && explosion != nullptr)
    {
        explosion->applyForce(character, 1.0f);
        return;
    }

    auto fire = dynamic_cast<Fire*>(h_obj);

    if (character != nullptr && fire != nullptr)
    {
        if (fire->getUser() != character)
            character->setGlobalState(Character::GlobalState::OnFire);
        return;
    }

    auto melee_weapon_area = dynamic_cast<MeleeWeaponArea*>(h_obj);

    if (character != nullptr && melee_weapon_area != nullptr)
    {
        if (character != melee_weapon_area->getFather()->getUser())
        {
            if (melee_weapon_area->getFather()->getUser() != getPlayer())
            {
                if (character == getPlayer())
                    factor = 1.0f / factor;
                else
                    factor = 1.0f;
            }

            float angle = utils::geo::wrapAngle0_360(std::get<1>(utils::geo::cartesianToPolar(
                    melee_weapon_area->getFather()->getUser()->getPosition() - character->getPosition())) * 180.0 /
                                                     M_PI);
            spawnBloodEvent(character->getPosition() + sf::Vector2f(0.0f, angle > 0 && angle <= 180 ? 5.0 : -5.0),
                            angle, melee_weapon_area->getFather()->getDeadlyFactor() * factor);
            spawnSound(RM.getSound("melee_hit"), character->getPosition());
            melee_weapon_area->setActive(false);
            character->getCut(*melee_weapon_area->getFather(), factor);
        }
        return;
    }
}

void Framework::alertCollision(DynamicObject* d_obj, StaticObject* s_obj)
{
    // Nothing to do for now (maybe sounds?)
}

void Framework::alertCollision(DynamicObject* d_obj_1, DynamicObject* d_obj_2)
{
    // Nothing to do for now (maybe sounds?)
}

Fire* Framework::spawnFire(Character* user, const std::string& name, const sf::Vector2f& pos, float dir)
{
    auto vector = sf::Vector2f{static_cast<float>(std::cos(dir)), static_cast<float>(std::sin(dir))};
    return this->spawnNewFire(user, pos + 20.0f * vector, dir);
}


void Framework::spawnEvent(const std::string& name, const sf::Vector2f& pos, float dir, float r)
{
    auto event = std::make_shared<Event>(pos, name, dir, r);
    engine_->spawnAnimationEvent(event);

    registerLight(event.get());
}

DestructionSystem* Framework::spawnSparksEvent(const sf::Vector2f& pos, const float dir, const float r)
{
    return spawnNewDestructionSystem(pos, dir - 90.0f, destruction_params_["debris"], 1.0f);
}

void Framework::spawnExplosionEvent(const sf::Vector2f& pos)
{
    auto number = std::to_string(utils::num::getRandom(1, 1));
    spawnEvent("explosion_" + number, pos, 0.0f, RMGET<sf::Vector2f>("animations", "explosion_" + number, "size").x);
    spawnSound(RM.getSound("wall_explosion"), pos);
}

void Framework::spawnKillEvent(const sf::Vector2f& pos)
{
    auto number = std::to_string(utils::num::getRandom(1, 1));
    spawnEvent("explosion_" + number, pos, 0.0f, RMGET<sf::Vector2f>("animations", "explosion_" + number, "size").x);
}

void Framework::spawnTeleportationEvent(const sf::Vector2f& pos)
{
    spawnEvent("teleportation", pos + sf::Vector2f{0.0f, 10.0f});
    spawnSound(RM.getSound("teleportation"), pos);
}

void Framework::spawnSwirlEvent(const std::string& name, const sf::Vector2f& pos, bool flipped)
{
    auto event = std::make_shared<Event>(pos, name + "_swirl");
    engine_->spawnAnimationEvent(event);
    spawnSound(RM.getSound(name), pos);

    if (flipped)
        event->setFlipX(true);

    registerLight(event.get());
}

void Framework::spawnFadeInOut()
{
    engine_->spawnEffect(std::make_shared<graphics::FadeInOut>(
            sf::Vector2f{static_cast<float>(CONF<int>("graphics/window_width_px")),
                         static_cast<float>(CONF<int>("graphics/window_height_px"))}, sf::Color::Black,
            CONF<float>("graphics/fade_in_out_duration")
    ));
}

Special* Framework::spawnSpecial(const sf::Vector2f& pos, const std::string& name)
{
    return this->spawnNewSpecial(name, -1, pos, Functional::Activation::None, {}, {});
}

void Framework::spawnShotEvent(const std::string& name, const sf::Vector2f& pos, float dir)
{
    auto vector = sf::Vector2f{static_cast<float>(std::cos(dir)), static_cast<float>(std::sin(dir))};
    spawnEvent("shot", pos + RMGET<float>("bullets", name, "burst_offset") * vector, dir * 180.0f / M_PI,
               RMGET<float>("bullets", name, "burst_size"));
    spawnSound(RM.getSound(name + "_bullet_shot"), pos);
}

DestructionSystem* Framework::spawnBloodEvent(const sf::Vector2f& pos, float dir, float deadly_factor)
{
    spawnEvent("blood", pos, dir, 0.0f);
    return spawnNewDestructionSystem(pos, dir, destruction_params_["blood"], deadly_factor);
}

Bullet* Framework::spawnBullet(Character* user, const std::string& name, const sf::Vector2f& pos, float dir)
{
    auto ptr = this->spawnNewBullet(user, name, pos, dir);

    auto vector = sf::Vector2f{static_cast<float>(std::cos(dir)), static_cast<float>(std::sin(dir))};

    spawnNewDestructionSystem(pos - 20.0f * vector, dir * 180.0f / M_PI + 90.0f, destruction_params_["husk"], 1.0f);
    return ptr;
}

void Framework::spawnExplosionForce(const sf::Vector2f& pos, float r)
{
    desired_explosions_.emplace_back(pos, r);
}

Bullet* Framework::spawnNewBullet(Character* user, const std::string& id, const sf::Vector2f& pos, float dir)
{
    bullets_.emplace_back(std::make_unique<Bullet>(user, pos, id, dir));

    auto ptr = bullets_.back().get();

    registerFunctions(ptr);
    engine_->registerObj<HoveringObject>(ptr);

    return ptr;
}

Fire* Framework::spawnNewFire(Character* user, const sf::Vector2f& pos, float dir)
{
    fire_.emplace_back(std::make_unique<Fire>(user, pos, dir));

    auto ptr = fire_.back().get();

    registerLight(ptr);
    engine_->registerObj<HoveringObject>(ptr);

    return ptr;
}

ObstacleTile* Framework::spawnNewObstacleTile(const std::string& id, const sf::Vector2f& pos)
{
    auto new_ptr = map_->spawn<ObstacleTile>(pos, 0.0f, id);
    engine_->registerObj<StaticObject>(new_ptr);
    return new_ptr;
}

Obstacle* Framework::spawnNewObstacle(const std::string& id, int u_id, const sf::Vector2f& pos,
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
    engine_->registerObj<StaticObject>(new_ptr);

    registerFunctions(new_ptr);

    return new_ptr;
}

void Framework::spawnNull(Character* user, const std::string& name, const sf::Vector2f& pos, float dir)
{

}

Decoration* Framework::spawnDecoration(const sf::Vector2f& pos, const std::string& name)
{
    return this->spawnNewDecoration(name, -1, pos);
}

Special* Framework::getCurrentSpecialObject() const
{
    return nullptr;
}

void Framework::useSpecialObject()
{

}

std::tuple<Framework::SpawningFunction, Framework::AnimationSpawningFunction>
Framework::getSpawningFunction(const std::string& name)
{
    static const auto swirl = [this](const std::string& name, const sf::Vector2f pos, float dir, bool flipped) {
        this->spawnSwirlEvent(name, pos, flipped);
    };
    static const auto null = [this](const std::string& name, const sf::Vector2f pos, float dir, bool flipped) {
        this->spawnNull(nullptr, "", pos, 0.0f);
    };
    static const auto fire = [this](const std::string& name, const sf::Vector2f pos, float dir, bool flipped) {
        this->spawnSound(RM.getSound("flame_shot"), pos);
    };
    static const auto sparks = [this](const std::string& name, const sf::Vector2f pos, float dir, bool flipped) {
        this->spawnShotEvent(name, pos, dir);
    };
    auto it = spawning_func_.find(name);

    if (it == spawning_func_.end())
    {
        throw std::invalid_argument("[Framework] SpawningFunction " + name + " is not handled!");
    }

    if (name == "bullet")
        return std::make_tuple(it->second, sparks);
    else if (name == "fire")
        return std::make_tuple(it->second, fire);

    return std::make_tuple(it->second, swirl);
}

const std::list<std::unique_ptr<Fire>>& Framework::getFires() const
{
    return fire_;
}

Decoration* Framework::spawnNewDecoration(const std::string& id, int u_id, const sf::Vector2f& pos)
{
    auto ptr = map_->spawn<Decoration>(pos, 0.0f, id);
    if (u_id != -1)
    {
        ptr->setUniqueId(u_id);
    }
    registerLight(ptr);

    return ptr;
}

Special* Framework::spawnNewSpecial(const std::string& id, int u_id,
                                    const sf::Vector2f& pos, Functional::Activation activation,
                                    const j3x::List& funcs, const j3x::List& datas)
{
    auto ptr = map_->spawn<Special>(pos, 0.0f, id);
    engine_->registerObj<HoveringObject>(ptr);

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

template<class T>
void Framework::findAndDelete(T* ptr)
{
}

template<>
void Framework::findAndDelete<Character>(Character* ptr)
{
    auto& npcs = map_->getList<NPC>();
    for (auto it = npcs.rbegin(); it != npcs.rend(); ++it)
    {
        if (it->get() == ptr)
        {
            this->unregisterCharacter(ptr);
            npcs.erase((++it).base());
            return;
        }
    }

    LOG.error("[Framework] Warning - character to delete not found!");
}

template<>
void Framework::findAndDelete<Bullet>(Bullet* ptr)
{
    for (auto it = bullets_.rbegin(); it != bullets_.rend(); ++it)
    {
        if (it->get() == ptr)
        {
            engine_->unregisterObj<HoveringObject>(ptr);
            bullets_.erase((++it).base());
            return;
        }
    }

    LOG.error("[Framework] Warning - bullet to delete not found!");
}

template<>
void Framework::findAndDelete<Fire>(Fire* ptr)
{
    for (auto it = fire_.rbegin(); it != fire_.rend(); ++it)
    {
        if (it->get() == ptr)
        {
            engine_->unregisterObj<HoveringObject>(ptr);
            fire_.erase((++it).base());
            return;
        }
    }

    LOG.error("[Framework] Warning - fire to delete not found!");
}

template<>
void Framework::findAndDelete<Decoration>(Decoration* ptr)
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

    LOG.error("[Framework] Warning - decoration to delete not found!");
}

template<>
void Framework::findAndDelete<Special>(Special* ptr)
{
    auto& specials = map_->getList<Special>();
    for (auto it = specials.rbegin(); it != specials.rend(); ++it)
    {
        if (it->get() == ptr)
        {
            engine_->unregisterObj<HoveringObject>(ptr);
            specials.erase((++it).base());
            return;
        }
    }

    LOG.error("[Framework] Warning - special to delete not found!");
}

template<>
void Framework::findAndDelete<DestructionSystem>(DestructionSystem* ptr)
{
    for (auto it = destruction_systems_.rbegin(); it != destruction_systems_.rend(); ++it)
    {
        if (it->get() == ptr)
        {
            destruction_systems_.erase((++it).base());
            return;
        }
    }

    LOG.error("[Framework] Warning - destruction system to delete not found!");
}

void Framework::unregisterTalkableArea(Character* character)
{
    auto talkable_area = character->getTalkableArea();
    if (talkable_area != nullptr)
    {
        engine_->unregisterObj<HoveringObject>(talkable_area);
    }
}

void Framework::registerWeapons(Character* character)
{
    for (auto& weapon : character->getWeapons())
    {
        registerWeapon(weapon.get());
    }
}

void Framework::registerWeapon(AbstractWeapon* weapon)
{
    if (!weapon->getId().empty() && weapon->getId() != "null")
    {
        auto func = this->getSpawningFunction(RMGET<std::string>("weapons", weapon->getId(), "spawn_func"));
        weapon->registerSpawningFunction(std::get<0>(func), std::get<1>(func));

        auto melee_weapon = dynamic_cast<MeleeWeapon*>(weapon);
        if (melee_weapon != nullptr)
        {
            engine_->registerObj<HoveringObject>(melee_weapon->getMeleeWeaponArea());
        }
    }
}

void Framework::unregisterWeapons(Character* character)
{
    for (auto& weapon : character->getWeapons())
    {
        auto melee_weapon = dynamic_cast<MeleeWeapon*>(weapon.get());
        if (melee_weapon != nullptr)
        {
            engine_->unregisterObj<HoveringObject>(melee_weapon->getMeleeWeaponArea());
        }
    }
}

void Framework::unregisterCharacter(Character* character, bool clear_clone)
{
    ui_->removeArrowIfExists(character);
    
    if (clear_clone)
    {
        auto player_clone = getPlayerClone();
        if (player_clone != nullptr)
        {
            player_clone->removeEnemy(character);
        }
    }

    engine_->unregisterObj<DynamicObject>(character);
    this->unregisterTalkableArea(character);
    this->unregisterWeapons(character);
}

void Framework::registerFunctions(Functional* functional) const
{
    for (const auto& function : functional->getFunctions())
    {
        auto& function_str = j3x::getObj<std::string>(function);
        functional->bindFunction(special_functions_->bindFunction(function_str),
                                 special_functions_->bindTextToUse(function_str),
                                 special_functions_->isUsableByNPC(function_str));
    }
}

void Framework::registerLight(Lightable* lightable) const
{
    if (lightable->getLightPoint() != nullptr)
    {
        lightable->getLightPoint()->registerGraphics(engine_->getGraphics());
    }
}

void Framework::initParams()
{
    destruction_params_["blood"] = {};
    destruction_params_["debris"] = {};
    destruction_params_["debris2"] = {};
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

    weather_params_ = std::make_unique<WeatherParams>();
    weather_params_->vel = CONF<float>("graphics/weather_vel");
    weather_params_->dir = CONF<float>("graphics/weather_dir");
    weather_params_->spread_degree = CONF<float>("graphics/weather_spread_degree");
    weather_params_->spread_vel = CONF<float>("graphics/weather_spread_vel");

    weather_params_->base_color = sf::Color(CONF<int>("graphics/weather_base_color"));
    weather_params_->rag3_color = sf::Color(CONF<int>("graphics/weather_rag3_color"));
    weather_params_->alpha_max = CONF<int>("graphics/weather_alpha_max");
    weather_params_->alpha_min = CONF<int>("graphics/weather_alpha_min");
    weather_params_->new_particles_time = CONF<float>("graphics/weather_new_particles_time");
    weather_params_->min_size = CONF<float>("graphics/weather_min_size");
    weather_params_->max_size = CONF<float>("graphics/weather_max_size");

    weather_params_->count = CONF<int>("graphics/weather_particles_count");

    weather_params_->shader = CONF<std::string>("graphics/weather_shader");
}

DestructionSystem*
Framework::spawnNewDestructionSystem(const sf::Vector2f& pos, float dir, const DestructionParams& params,
                                     float quantity_factor)
{
    destruction_systems_.emplace_back(std::make_unique<DestructionSystem>(pos, dir, params, quantity_factor));
    return destruction_systems_.back().get();
}

void Framework::close()
{
    this->engine_->getGraphics().getWindow().close();
}

void Framework::spawnThought(Character* user, const std::string& text)
{
    ui_->spawnThought(user, text);
}

void Framework::spawnBonusText(const sf::Vector2f& pos, const std::string& text)
{
    ui_->spawnBonusText(pos, text);
}

UserInterface* Framework::getUI()
{
    return ui_.get();
}

SpecialFunctions* Framework::getSpecialFunctions()
{
    return special_functions_.get();
}

Player* Framework::getPlayer()
{
    return nullptr;
}

PlayerClone* Framework::getPlayerClone()
{
    return nullptr;
}

void Framework::setRag3Time(float duration)
{

}

void Framework::forceZoomTo(AbstractPhysicalObject* character)
{

}

void Framework::killPlayer(Player* player)
{
    spawnDecoration(player->getPosition(), "blood");
    spawnKillEvent(player->getPosition());
    player->setDead();
    engine_->unregisterObj<DynamicObject>(player);
}

NPC* Framework::spawnNewNPC(const std::string& id, int u_id, Functional::Activation activation,
                            const j3x::List& funcs, const j3x::List& datas)
{
    return nullptr;
}

NPC* Framework::spawnNewPlayerClone(const std::string& weapon_id)
{
    return nullptr;
}

Character* Framework::getCurrentTalkableCharacter() const
{
    return nullptr;
}

Framework::GameState Framework::getGameState() const
{
    return state_;
}

void Framework::setGameState(Framework::GameState state)
{
    state_ = state;
}

void Framework::initObstacles()
{
    for (auto& obstacle : map_->getList<ObstacleTile>())
        engine_->registerObj<StaticObject>(obstacle.get());

    for (auto& obstacle : map_->getList<Obstacle>())
    {
        engine_->registerObj<StaticObject>(obstacle.get());
        registerLight(obstacle.get());

        registerFunctions(obstacle.get());
    }
}

void Framework::initDecorations()
{
    for (auto& decoration : map_->getList<Decoration>())
    {
        registerLight(decoration.get());
    }
}

void Framework::initWeapons()
{
    for (auto& weapon : map_->getList<PlacedWeapon>())
    {
        auto func = this->getSpawningFunction(RMGET<std::string>("weapons", weapon->getId(), "spawn_func"));
        weapon->registerSpawningFunction(std::get<0>(func), std::get<1>(func));
    }
}

void Framework::initNPCs()
{

}

void Framework::initPlayer(Player* player)
{
    engine_->registerObj<DynamicObject>(player);
    registerLight(player);
    registerWeapons(player);
}

void Framework::initSpecials()
{
    for (auto& special : map_->getList<Special>())
    {
        registerLight(special.get());
        engine_->registerObj<HoveringObject>(special.get());
        registerFunctions(special.get());
    }
}

float Framework::getRag3Time() const
{
    return 0.0f;
}

void Framework::useItem(const std::string& id)
{
    auto player = getPlayer();
    if (player != nullptr)
    {
        player->useItem(id);
        spawnSound(RM.getSound("ui_click"), player->getPosition());
    }
}

void Framework::obstacleDestroyedEvent(Obstacle* obstacle)
{

}

void Framework::spawnSound(const sf::SoundBuffer& sound, const sf::Vector2f& pos, bool force_pitch)
{
    if (CONF<bool>("sound/sound_on") && utils::geo::getDistance(pos, getPlayer()->getPosition()) < 500.0f)
        engine_->spawnSoundEvent(sound, pos, 100.0f, force_pitch);
}

void Framework::spawnSound(const sf::SoundBuffer& sound)
{
    if (CONF<bool>("sound/sound_on"))
        engine_->spawnSoundEvent(sound, 100.0f);
}

DestructionSystem* Framework::spawnSparksEvent2(const sf::Vector2f& pos, float dir, float r)
{
    return spawnNewDestructionSystem(pos, dir - 90.0f, destruction_params_["debris2"], 1.0f);
}

float Framework::getTimeManipulationFuel() const
{
    return 0.0f;
}

void Framework::respawn(const std::string& map_name)
{
    ui_->clearThoughts();
    setRag3Time(0.0f);

    map_->loadMap(map_name.empty() ? map_->getMapName() : map_name);
    engine_->initializeCollisions(map_->getSize(), CONF<float>("collision_grid_size"));
    agents_manager_ = std::make_unique<ai::AgentsManager>(&map_->getMapBlockage(), ai::AStar::EightNeighbours,
                                                          CONF<float>("characters/max_time_without_path_recalc"),
                                                          CONF<float>("characters/min_pos_change_without_path_recalc"),
                                                          CONF<int>("characters/max_path_search_depth"));
    engine_->getGraphics().setBgColor(sf::Color(j3x::get<int>(map_->getParams(), "background_color")));
    lighting_ = std::make_unique<graphics::Lighting>(
            sf::Vector2f{static_cast<float>(CONF<int>("graphics/window_width_px")),
                         static_cast<float>(CONF<int>("graphics/window_height_px"))},
            sf::Color(j3x::get<int>(map_->getParams(), "lighting_color")));
    fire_.clear();
    bullets_.clear();
    explosions_.clear();
    desired_explosions_.clear();
    destruction_systems_.clear();

    initObstacles();
    initDecorations();

    auto player = getPlayer();
    if (player != nullptr)
        initPlayer(player);
    initNPCs();
    initWeapons();
    initSpecials();

    ui_->initializeTutorialArrows();
    setGameState(Framework::GameState::Normal);
    should_finish_map_ = false;
}

void Framework::finishMap()
{
    should_finish_map_ = false;
}

void Framework::setFinishMap()
{
    should_finish_map_ = true;
}

void Framework::startGame(const std::string& map_name)
{

}
