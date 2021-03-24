//
// Created by jul3x on 27.02.19.
//

#include <R3E/utils/Geometry.h>

#include <common/ResourceManager.h>
#include <common/weapons/MeleeWeapon.h>
#include <common/Framework.h>

#include <common/events/Event.h>



Framework::Framework() : time_elapsed_(0.0f), state_(Framework::GameState::Menu)
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

    spawning_func_["bullet"] = [this] (Character* user, const std::string& name, const sf::Vector2f& pos, float dir) { this->spawnBullet(user, name, pos, dir); };
    spawning_func_["fire"] = [this] (Character* user, const std::string& name, const sf::Vector2f& pos, float dir) { this->spawnFire(user, name, pos, dir); };
    spawning_func_["null"] = [this] (Character* user, const std::string& name, const sf::Vector2f& pos, float dir) { this->spawnNull(user, name, pos, dir); };

    camera_ = std::make_unique<Camera>();
    special_functions_ = std::make_unique<SpecialFunctions>(this);
    map_ = std::make_unique<Map>();

    engine_->registerCamera(camera_.get());

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

    for (auto& weapon : map_->getList<PlacedWeapon>())
    {
        auto func = this->getSpawningFunction(RMGET<std::string>("weapons", weapon->getId(), "spawn_func"));
        weapon->registerSpawningFunction(std::get<0>(func), std::get<1>(func));
    }

    // TODO OVERRIDE METHODS = initSpecials initWeapons and etc...
}

void Framework::update(float time_elapsed)
{
}

void Framework::updateMapObjects(float time_elapsed)
{
    // TODO Extract as much as you can
}

void Framework::updateBullets(float time_elapsed)
{
    // TODO Extract as much as you can
}

void Framework::draw(graphics::Graphics& graphics)
{
    // TODO Extract as much as you can
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

void Framework::spawnEvent(const std::string& name, const sf::Vector2f& pos, float dir, float r)
{
    auto event = std::make_shared<Event>(pos, name, dir, r);
    engine_->spawnAnimationEvent(event);

    registerLight(event.get());
}

DestructionSystem* Framework::spawnSparksEvent(const sf::Vector2f& pos, const float dir, const float r)
{
    spawnEvent("sparks", pos, dir, r);
    return spawnNewDestructionSystem(pos, dir - 90.0f, destruction_params_["debris"], 1.0f);
}

void Framework::spawnExplosionEvent(const sf::Vector2f& pos)
{
    auto number = std::to_string(utils::num::getRandom(1, 1));
    spawnEvent("explosion_" + number, pos, 0.0f, RMGET<sf::Vector2f>("animations", "explosion_" + number, "size").x);

    if (CONF<bool>("sound/sound_on"))
        engine_->spawnSoundEvent(RM.getSound("wall_explosion"), pos);
}

void Framework::spawnKillEvent(const sf::Vector2f& pos)
{
    auto number = std::to_string(utils::num::getRandom(1, 1));
    spawnEvent("explosion_" + number, pos, 0.0f, RMGET<sf::Vector2f>("animations", "explosion_" + number, "size").x);

//    if (CONF<bool>("sound/sound_on"))
//        engine_->spawnSoundEvent(RM.getSound("wall_explosion"), pos);
}

void Framework::spawnTeleportationEvent(const sf::Vector2f& pos)
{
    spawnEvent("teleportation", pos + sf::Vector2f{0.0f, 10.0f});

    if (CONF<bool>("sound/sound_on"))
        engine_->spawnSoundEvent(RM.getSound("teleportation"), pos);
}

void Framework::spawnSwirlEvent(const std::string& name, const sf::Vector2f& pos, bool flipped)
{
    auto event = std::make_shared<Event>(pos, name + "_swirl");
    engine_->spawnAnimationEvent(event);

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
    spawnEvent("shot", pos + RMGET<float>("bullets", name, "burst_offset") * vector, dir * 180.0f / M_PI, RMGET<float>("bullets", name, "burst_size"));

    if (CONF<bool>("sound/sound_on"))
        engine_->spawnSoundEvent(RM.getSound(name + "_bullet_shot"), pos);
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

void Framework::updateExplosions()
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

void Framework::alertCollision(HoveringObject* h_obj, StaticObject* s_obj)
{
    // TODO Extract as much as you can
}

void Framework::alertCollision(HoveringObject* h_obj, DynamicObject* d_obj)
{
    // TODO Extract as much as you can
}

void Framework::alertCollision(DynamicObject* d_obj, StaticObject* s_obj)
{
    // Nothing to do for now (maybe sounds?)
}

void Framework::alertCollision(DynamicObject* d_obj_1, DynamicObject* d_obj_2)
{
    // Nothing to do for now (maybe sounds?)
}

Bullet* Framework::spawnNewBullet(Character* user, const std::string& id, const sf::Vector2f& pos, float dir)
{
    bullets_.emplace_back(std::make_unique<Bullet>(user, pos, id, dir));

    auto ptr = bullets_.back().get();

    registerFunctions(ptr);
    engine_->registerHoveringObject(ptr);

    return ptr;
}

Fire* Framework::spawnFire(Character* user, const std::string& name, const sf::Vector2f& pos, float dir)
{
    auto vector = sf::Vector2f{static_cast<float>(std::cos(dir)), static_cast<float>(std::sin(dir))};
    return this->spawnNewFire(user, pos + 20.0f * vector, dir);
}

void Framework::updateFire(float time_elapsed)
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

Fire* Framework::spawnNewFire(Character* user, const sf::Vector2f& pos, float dir)
{
    fire_.emplace_back(std::make_unique<Fire>(user, pos, dir));

    auto ptr = fire_.back().get();

    registerLight(ptr);
    engine_->registerHoveringObject(ptr);

    return ptr;
}

ObstacleTile* Framework::spawnNewObstacleTile(const std::string& id, const sf::Vector2f& pos)
{
    auto new_ptr = map_->spawn<ObstacleTile>(pos, 0.0f, id);
    engine_->registerStaticObject(new_ptr);
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
    engine_->registerStaticObject(new_ptr);

    registerFunctions(new_ptr);

    return new_ptr;
}

void Framework::findAndDeleteBullet(Bullet* ptr)
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

    LOG.error("[Framework] Warning - bullet to delete not found!");
}

void Framework::findAndDeleteFire(Fire* ptr)
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

    LOG.error("[Framework] Warning - fire to delete not found!");
}

void Framework::findAndDeleteDecoration(Decoration* ptr)
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

Special* Framework::getCurrentSpecialObject() const
{
    return nullptr;
}

void Framework::useSpecialObject()
{

}

std::tuple<Framework::SpawningFunction, Framework::AnimationSpawningFunction> Framework::getSpawningFunction(const std::string& name)
{
    static const auto swirl = [this](const std::string& name, const sf::Vector2f pos, float dir, bool flipped) { this->spawnSwirlEvent(name, pos, flipped); };
    static const auto null = [this](const std::string& name, const sf::Vector2f pos, float dir, bool flipped) { this->spawnNull(nullptr, "", pos, 0.0f); };
    static const auto sparks = [this](const std::string& name, const sf::Vector2f pos, float dir, bool flipped) { this->spawnShotEvent(name, pos, dir); };
    auto it = spawning_func_.find(name);

    if (it == spawning_func_.end())
    {
        throw std::invalid_argument("[Framework] SpawningFunction " + name + " is not handled!");
    }

    if (name == "bullet")
        return std::make_tuple(it->second, sparks);
    else if (name == "fire")
        return std::make_tuple(it->second, null);

    return std::make_tuple(it->second, swirl);
}

void Framework::spawnNull(Character* user, const std::string& name, const sf::Vector2f& pos, float dir)
{

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

Decoration* Framework::spawnDecoration(const sf::Vector2f& pos, const std::string& name)
{
    return this->spawnNewDecoration(name, -1, pos);
}

Special* Framework::spawnNewSpecial(const std::string& id, int u_id,
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

void Framework::findAndDeleteSpecial(Special* ptr)
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

    LOG.error("[Framework] Warning - special to delete not found!");
}

void Framework::findAndDeleteDestructionSystem(DestructionSystem* ptr)
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
            engine_->registerHoveringObject(melee_weapon->getMeleeWeaponArea());
        }
    }
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

void Framework::updateDestructionSystems(float time_elapsed)
{
    utils::eraseIf<std::unique_ptr<DestructionSystem>>(destruction_systems_, [&time_elapsed, this](std::unique_ptr<DestructionSystem>& system) {
        if (!system->update(time_elapsed))
        {
            return true;
        }
        return false;
    });
}

void Framework::initDestructionParams()
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

DestructionSystem* Framework::spawnNewDestructionSystem(const sf::Vector2f& pos, float dir, const DestructionParams& params, float quantity_factor)
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
    // TODO
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

void Framework::setRag3Time(float duration)
{

}

void Framework::forceZoomTo(AbstractPhysicalObject* character)
{

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