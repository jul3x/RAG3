//
// Created by jul3x on 31.03.20.
//

#include <R3E/system/Logger.h>

#include <common/ResourceManager.h>
#include <common/weapons/ShootingWeapon.h>
#include <common/Framework.h>
#include <common/misc/SpecialFunctions.h>
#include <common/misc/Journal.h>


SpecialFunctions::SpecialFunctions(Framework* framework) : framework_(framework)
{
    // TODO this looks horrible...
    functions_["MapStart"] = std::make_tuple([this](Functional* obj, const j3x::Obj& data, Character* user) { this->mapStart(obj, data, user); }, "[F] Start new map", false);
    functions_["MapEnd"]= std::make_tuple([this](Functional* obj, const j3x::Obj& data, Character* user) { this->mapEnd(obj, data, user); }, "[F] End this map", false);
    functions_["TurnLight"] = std::make_tuple([this](Functional* obj, const j3x::Obj& data, Character* user) { this->turnLight(obj, data, user); }, "[F] Press", true);
    functions_["PourWater"] = std::make_tuple([this](Functional* obj, const j3x::Obj& data, Character* user) { this->pourWater(obj, data, user); }, "[F] Pour water on yourself", true);
    functions_["OpenDoor"] = std::make_tuple([this](Functional* obj, const j3x::Obj& data, Character* user) { this->openDoor(obj, data, user); }, "", true);
    functions_["ReadNote"] = std::make_tuple([this](Functional* obj, const j3x::Obj& data, Character* user) { this->readNote(obj, data, user); }, "[F] Read note", true);
    functions_["AddWeapon"] = std::make_tuple([this](Functional* obj, const j3x::Obj& data, Character* user) { this->addWeapon(obj, data, user); }, "[F} Pick weapon", false);
    functions_["AddAmmo"] = std::make_tuple([this](Functional* obj, const j3x::Obj& data, Character* user) { this->addAmmo(obj, data, user); }, "[F] Pick ammunition", false);
    functions_["AddHealth"] = std::make_tuple([this](Functional* obj, const j3x::Obj& data, Character* user) { this->addHealth(obj, data, user); }, "[F] Pick to heal yourself", false);
    functions_["AddSpeed"] = std::make_tuple([this](Functional* obj, const j3x::Obj& data, Character* user) { this->addSpeed(obj, data, user); }, "[F] Pick to inject", false);
    functions_["TakeRag3"] = std::make_tuple([this](Functional* obj, const j3x::Obj& data, Character* user) { this->takeRag3(obj, data, user); }, "[F] Pick to take", false);
    functions_["PickCrystal"] = std::make_tuple([this](Functional* obj, const j3x::Obj& data, Character* user) { this->pickCrystal(obj, data, user); }, "[F] Pick crystal", false);
    functions_["PayRespect"] = std::make_tuple([this](Functional* obj, const j3x::Obj& data, Character* user) { this->payRespect(obj, data, user); }, "[F] Pay respect", false);
    functions_["SpawnThought"] = std::make_tuple([this](Functional* obj, const j3x::Obj& data, Character* user) { this->spawnThought(obj, data, user); }, "", true);
    functions_["SpawnPlayerThought"] = std::make_tuple([this](Functional* obj, const j3x::Obj& data, Character* user) { this->spawnPlayerThought(obj, data, user); }, "", true);
    functions_["ChangeOpenState"] = std::make_tuple([this](Functional* obj, const j3x::Obj& data, Character* user) { this->changeOpenState(obj, data, user); }, "[F] Pull the trigger", true);
    functions_["Teleport"] = std::make_tuple([this](Functional* obj, const j3x::Obj& data, Character* user) { this->teleport(obj, data, user); }, "", true);
    functions_["Kill"] = std::make_tuple([this](Functional* obj, const j3x::Obj& data, Character* user) { this->kill(obj, data, user); }, "", true);
    functions_["KillObj"] = std::make_tuple([this](Functional* obj, const j3x::Obj& data, Character* user) { this->killObj(obj, data, user); }, "", true);
    functions_["ZoomTo"] = std::make_tuple([this](Functional* obj, const j3x::Obj& data, Character* user) { this->zoomTo(obj, data, user); }, "", false);
    functions_["ActivateWeapon"] = std::make_tuple([this](Functional* obj, const j3x::Obj& data, Character* user) { this->activateWeapon(obj, data, user); }, "", false);
    functions_["SetOnFire"] = std::make_tuple([this](Functional* obj, const j3x::Obj& data, Character* user) { this->setOnFire(obj, data, user); }, "", false); // TODO maybe true?
    functions_["Explode"] = std::make_tuple([this](Functional* obj, const j3x::Obj& data, Character* user) { this->explode(obj, data, user); }, "", false);
    functions_["RemoveDecoration"] = std::make_tuple([this](Functional* obj, const j3x::Obj& data, Character* user) { this->removeDecoration(obj, data, user); }, "", false);
    functions_["RemoveSpecial"] = std::make_tuple([this](Functional* obj, const j3x::Obj& data, Character* user) { this->removeSpecial(obj, data, user); }, "", false);
    functions_["SpawnLava"] = std::make_tuple([this](Functional* obj, const j3x::Obj& data, Character* user) { this->spawnLava(obj, data, user); }, "", false);
    functions_["SpawnExplosionEvent"] = std::make_tuple([this](Functional* obj, const j3x::Obj& data, Character* user) { this->spawnExplosionEvent(obj, data, user); }, "", false);
    functions_["SpawnExplosionEventByPos"] = std::make_tuple([this](Functional* obj, const j3x::Obj& data, Character* user) { this->spawnExplosionEventByPos(obj, data, user); }, "", true);
    functions_["SpawnMiniLava"] = std::make_tuple([this](Functional* obj, const j3x::Obj& data, Character* user) { this->spawnMiniLava(obj, data, user); }, "", false);
    functions_["SpawnFlame"] = std::make_tuple([this](Functional* obj, const j3x::Obj& data, Character* user) { this->spawnFlame(obj, data, user); }, "", false);
    functions_["SpawnAmmo"] = std::make_tuple([this](Functional* obj, const j3x::Obj& data, Character* user) { this->spawnAmmo(obj, data, user); }, "", false);
    functions_["SpawnDestruction"] = std::make_tuple([this](Functional* obj, const j3x::Obj& data, Character* user) { this->spawnDestruction(obj, data, user); }, "", true);
    functions_["SpawnCrystal"] = std::make_tuple([this](Functional* obj, const j3x::Obj& data, Character* user) { this->spawnCrystal(obj, data, user); }, "", true);
    functions_["null"] = std::make_tuple([this](Functional* obj, const j3x::Obj& data, Character* user) { this->nullFunc(obj, data, user); }, "", true);
    functions_["Deactivate"] = std::make_tuple([this](Functional* obj, const j3x::Obj& data, Character* user) { this->deactivate(obj, data, user); }, "", true);
    functions_["Destroy"] = std::make_tuple([this](Functional* obj, const j3x::Obj& data, Character* user) { this->destroy(obj, data, user); }, "", true);
    functions_["ActivateSpecial"] = std::make_tuple([this](Functional* obj, const j3x::Obj& data, Character* user) { this->activateSpecial(obj, data, user); }, "", true);
}


const SpecialFunctions::SpecialFunction& SpecialFunctions::bindFunction(const std::string& key) const
{
    auto it = functions_.find(key);

    if (it == functions_.end())
    {
        throw std::invalid_argument("[SpecialFunctions] Function " + key + " is not handled!");
    }

    return std::get<0>(it->second);
}

const std::string& SpecialFunctions::bindTextToUse(const std::string &key) const
{
    auto it = functions_.find(key);

    if (it == functions_.end())
    {
        throw std::invalid_argument("[SpecialFunctions] Function " + key + " is not handled!");
    }

    return std::get<1>(it->second);
}

bool SpecialFunctions::isUsableByNPC(const std::string& key) const
{
    auto it = functions_.find(key);

    if (it == functions_.end())
    {
        throw std::invalid_argument("[SpecialFunctions] Function " + key + " is not handled!");
    }

    return std::get<2>(it->second);
}

void SpecialFunctions::mapStart(Functional* obj, const j3x::Obj& data, Character* user)
{
    // empty
}

void SpecialFunctions::mapEnd(Functional* obj, const j3x::Obj& data, Character* user)
{
    LOG.info("[SpecialFunction] Map end.");
    user->setHealth(0);
}

void SpecialFunctions::openDoor(Functional* obj, const j3x::Obj& data, Character* user)
{
    LOG.info("[SpecialFunction] Open door.");
    auto door = framework_->getMap()->getObjectById<Obstacle>(j3x::getObj<int>(data));
    float endurance = 0.0f;
    if (door->getCollisionArea().getType() == collision::Area::Type::None)
    {
        door->changeTexture(&RM.getTexture("obstacles/" + door->getId()));
        door->changeCollisionArea(collision::Box(RMGET<sf::Vector2f>("obstacles", door->getId(), "collision_size").x,
                                                 RMGET<sf::Vector2f>("obstacles", door->getId(), "collision_size").y,
                                                 RMGET<sf::Vector2f>("obstacles", door->getId(), "collision_offset")));
        door->setZIndex(RMGET<int>("obstacles", door->getId(), "z_index"));
        endurance = RMGET<float>("obstacles", door->getId(), "endurance");
    }
    else
    {
        door->changeTexture(&RM.getTexture("obstacles/" + door->getId() + "_open"));
        door->changeCollisionArea(collision::None());
        door->setZIndex(RMGET<int>("obstacles", door->getId() + "_open", "z_index"));
    }

    Map::markBlocked(framework_->getMap()->getMapBlockage().blockage_,
                     door->getPosition() + RMGET<sf::Vector2f>("obstacles", door->getId(), "collision_offset"),
                     RMGET<sf::Vector2f>("obstacles",  door->getId(), "collision_size"),
                     endurance);

    if (user != nullptr && framework_->getJournal() != nullptr)
        framework_->getJournal()->event<DoorOpen>(door);
}

void SpecialFunctions::changeOpenState(Functional* obj, const j3x::Obj& data, Character* user)
{
    LOG.info("[SpecialFunction] Changing open state.");

    auto special_obj = framework_->getMap()->getObjectById<Special>(obj->getUniqueId());

    if (special_obj->getAdditionalBooleanData())
    {
        special_obj->changeTexture(&RM.getTexture("specials/" + obj->getId()));
        special_obj->setAdditionalBooleanData(false);
    }
    else
    {
        special_obj->changeTexture(&RM.getTexture("specials/" + obj->getId() + "_open"));
        special_obj->setAdditionalBooleanData(true);
    }

    if (user != nullptr)
    {
        framework_->spawnEvent("dust", special_obj->getPosition() + sf::Vector2f{0.0f, 10.0f});
        if (framework_->getJournal() != nullptr)
            framework_->getJournal()->event<ChangeOpenState>(special_obj);
    }
}

void SpecialFunctions::pourWater(Functional* obj, const j3x::Obj& data, Character* user)
{
    LOG.info("[SpecialFunction] Pouring water.");

    framework_->spawnThought(user, "Feels good man...");
    user->setGlobalState(Character::GlobalState::Normal);
}

void SpecialFunctions::turnLight(Functional* obj, const j3x::Obj& data, Character* user)
{
    LOG.info("[SpecialFunction] Turn light.");

    auto light_obj = framework_->getMap()->getObjectById<Decoration>(j3x::getObj<int>(data));

    if (light_obj->getLightPoint() != nullptr)
    {
        light_obj->lightOff();
    }
    else
    {
        light_obj->makeLightPoint(light_obj->getPosition(),
                                  RMGET<float>("decorations", light_obj->getId(), "light_point_radius", true) * CONF<float>("graphics/global_zoom"),
                                  &RM.getTexture("lightpoint"), RMGET<std::string>("decorations", light_obj->getId(), "light_point"),
                                  RMGET<float>("decorations", light_obj->getId(), "light_point_data", true));
        framework_->registerLight(light_obj);
    }
}

void SpecialFunctions::readNote(Functional* obj, const j3x::Obj& data, Character* user)
{
    LOG.info("[SpecialFunction] Reading note.");

    if (framework_->getUI() != nullptr)
        framework_->getUI()->spawnNoteWindow(j3x::getObj<std::string>(data));
}

void SpecialFunctions::addWeapon(Functional* obj, const j3x::Obj& data, Character* user)
{
    LOG.info("[SpecialFunction] Adding weapon.");

    auto& data_parsed = j3x::getObj<std::string>(data);
    std::shared_ptr<AbstractWeapon> weapon = AbstractWeapon::create(user, data_parsed);

    if (user->addWeaponToBackpack(weapon))
    {
        framework_->registerWeapon(weapon.get());
    }
}

void SpecialFunctions::addAmmo(Functional* obj, const j3x::Obj& data, Character* user)
{
    LOG.info("[SpecialFunction] Adding ammo.");

    user->addAmmoToWeapon(j3x::getObj<std::string>(data));
}

void SpecialFunctions::addHealth(Functional* obj, const j3x::Obj& data, Character* user)
{
    LOG.info("[SpecialFunction] Adding health.");
    auto data_parsed = j3x::getObj<int>(data);

    user->setHealth(std::min(user->getHealth() + data_parsed, user->getMaxHealth()));

    framework_->spawnThought(user, "Uff!\nThat's what I needed...");
}

void SpecialFunctions::addSpeed(Functional* obj, const j3x::Obj& data, Character* user)
{
    LOG.info("[SpecialFunction] Adding speed.");
    auto data_parsed = j3x::getObj<sf::Vector2f>(data);

    user->setSpeedFactor(data_parsed.x, data_parsed.y);

    framework_->spawnThought(user, "Woah!\nI feel... faster...");
}

void SpecialFunctions::pickCrystal(Functional* obj, const j3x::Obj& data, Character* user)
{
    LOG.info("[SpecialFunction] Picking crystal.");
    auto& pos = dynamic_cast<AbstractPhysicalObject*>(obj)->getPosition();

    if (framework_->getStats() != nullptr)
        framework_->getStats()->pickCrystal(pos);

    framework_->spawnThought(user, "I need more of them!");
}

void SpecialFunctions::spawnThought(Functional* obj, const j3x::Obj& data, Character* user)
{
    LOG.info("[SpecialFunction] Spawning thought.");

    framework_->spawnThought(user, j3x::getObj<std::string>(data));
}

void SpecialFunctions::teleport(Functional* obj, const j3x::Obj& data, Character* user)
{
    LOG.info("[SpecialFunction] Teleport.");
    auto player = dynamic_cast<Player*>(user);

    try
    {
        auto pos = j3x::getObj<sf::Vector2f>(data);

        user->setPosition(pos);
        user->setForcedVelocity({0.0f, 0.0f});

        if (player != nullptr)
        {
            framework_->spawnFadeInOut();
            framework_->getCamera()->setCenter({player->getPosition().x, player->getPosition().y, 0.0f});
        }
    }
    catch (const std::exception& e)
    {
        throw std::invalid_argument("[SpecialFunctions] Bad format of position type in function data!");
    }
}

void SpecialFunctions::setOnFire(Functional* obj, const j3x::Obj& data, Character* user)
{
    LOG.info("[SpecialFunction] Setting on fire.");
    user->setGlobalState(Character::GlobalState::OnFire);
}

void SpecialFunctions::explode(Functional* obj, const j3x::Obj& data, Character* user)
{
    LOG.info("[SpecialFunction] Exploding.");
    auto obs = dynamic_cast<AbstractPhysicalObject*>(obj);

    framework_->spawnExplosionForce(obs->getPosition(), j3x::getObj<float>(data));

    if (framework_->getStats() != nullptr)
        framework_->getStats()->explode(obs->getPosition());
}

void SpecialFunctions::removeDecoration(Functional* obj, const j3x::Obj& data, Character* user)
{
    LOG.info("[SpecialFunction] Removing decoration.");
    auto decoration_id = j3x::getObj<int>(data);
    auto decoration = framework_->getMap()->getObjectById<Decoration>(decoration_id);
    decoration->deactivate();
}

void SpecialFunctions::spawnExplosionEvent(Functional* obj, const j3x::Obj& data, Character* user)
{
    LOG.info("[SpecialFunction] Spawning explosion event.");
    auto obs = dynamic_cast<AbstractPhysicalObject*>(obj);
    framework_->spawnExplosionEvent(obs->getPosition());
}

void SpecialFunctions::spawnExplosionEventByPos(Functional* obj, const j3x::Obj& data, Character* user)
{
    LOG.info("[SpecialFunction] Spawning explosion event by position.");
    auto pos = j3x::getObj<sf::Vector2f>(data);
    framework_->spawnExplosionEvent(pos);
}

void SpecialFunctions::spawnLava(Functional* obj, const j3x::Obj& data, Character* user)
{
    LOG.info("[SpecialFunction] Spawning lava.");
    auto pos = j3x::getObj<sf::Vector2f>(data);
    framework_->spawnSpecial(pos, "lava");
}

void SpecialFunctions::spawnMiniLava(Functional* obj, const j3x::Obj& data, Character* user)
{
    LOG.info("[SpecialFunction] Spawning mini lava.");
    auto pos = j3x::getObj<sf::Vector2f>(data);
    framework_->spawnSpecial(pos, "mini_lava");
}

void SpecialFunctions::spawnAmmo(Functional* obj, const j3x::Obj& data, Character* user)
{
    LOG.info("[SpecialFunction] Spawning ammo.");

    auto npc = dynamic_cast<Character*>(obj);

    auto ammo_offset = CONF<float>("characters/ammo_drop_offset");
    for (size_t i = 0; i < CONF<int>("characters/ammo_dropped"); ++i)
    {
        auto offset = sf::Vector2f(utils::num::getRandom(-ammo_offset, ammo_offset),
                                   utils::num::getRandom(-ammo_offset, ammo_offset));
        framework_->spawnSpecial(npc->getPosition() + offset, j3x::getObj<std::string>(data) + "_ammo");
    }
}

void SpecialFunctions::spawnFlame(Functional* obj, const j3x::Obj& data, Character* user)
{
    LOG.info("[SpecialFunction] Spawning flame.");
    auto pos = j3x::getObj<sf::Vector2f>(data);
    framework_->spawnSpecial(pos, "flame");
    framework_->spawnExplosionEvent(pos);
}

void SpecialFunctions::kill(Functional* obj, const j3x::Obj& data, Character* user)
{
    LOG.info("[SpecialFunction] Killing.");

    const auto& what = j3x::getObj<std::string>(data);

    // TODO - is it worth it?
    if (what == "lava" || what == "water")
    {
        auto object = dynamic_cast<AbstractPhysicalObject*>(obj);

        auto difference = object->getPosition() + object->getCollisionArea().getOffset() - user->getPosition() - user->getCollisionArea().getOffset();
        if (!utils::num::isNearlyEqual(std::get<0>(utils::geo::cartesianToPolar(difference)), 0.0f, 16.0f))
        {
            auto diff = utils::geo::getNormalized(difference);
            user->addSteeringForce(RMGET<float>("characters", user->getId(), "max_speed") * diff, 0.1f);
        }
        else
        {
            user->setHealth(0);
        }
    }
    else
    {
        user->setHealth(0);
    }
//     Game::get().spawnAnimationEvent(data);
}

void SpecialFunctions::killObj(Functional* obj, const j3x::Obj& data, Character* user)
{
    LOG.info("[SpecialFunction] Killing object.");

    auto what = framework_->getMap()->getObjectById<Obstacle>(j3x::getObj<int>(data));
    if (what != nullptr) what->setHealth(0);
}

void SpecialFunctions::nullFunc(Functional *obj, const j3x::Obj &data, Character* user)
{
    LOG.info("[SpecialFunction] Null.");
}

void SpecialFunctions::deactivate(Functional *obj, const j3x::Obj &data, Character* user)
{
    LOG.info("[SpecialFunction] Deactivating.");
    obj->deactivate();

    if (framework_->getJournal() != nullptr)
        framework_->getJournal()->event<ObjectDeactivated>(dynamic_cast<Special*>(obj));
}

void SpecialFunctions::destroy(Functional *obj, const j3x::Obj &data, Character* user)
{
    LOG.info("[SpecialFunction] Destroying " + std::to_string(obj->getUniqueId()) + ".");
    framework_->spawnEvent("dust", dynamic_cast<AbstractPhysicalObject*>(obj)->getPosition());
    user->setCurrentSpecialObject(nullptr);
    obj->destroy();
}

void SpecialFunctions::activateWeapon(Functional* obj, const j3x::Obj& data, Character* user)
{
    LOG.info("[SpecialFunction] Activating weapon.");
    auto weapon_id = j3x::getObj<int>(data);
    auto weapon = framework_->getMap()->getObjectById<PlacedWeapon>(weapon_id);
    weapon->setActive(!weapon->getActive());

    if (framework_->getJournal() != nullptr)
        framework_->getJournal()->event<WeaponActivation>(weapon);
}

void SpecialFunctions::spawnPlayerThought(Functional* obj, const j3x::Obj& data, Character* user)
{
    LOG.info("[SpecialFunction] Spawning player thought.");

    if (framework_->getPlayer() != nullptr)
        framework_->spawnThought(framework_->getPlayer(), j3x::getObj<std::string>(data));
}

void SpecialFunctions::removeSpecial(Functional* obj, const j3x::Obj& data, Character* user)
{
    LOG.info("[SpecialFunction] Removing special.");
    auto special_id = j3x::getObj<int>(data);
    auto special = framework_->getMap()->getObjectById<Special>(special_id);
    special->destroy();
}

void SpecialFunctions::activateSpecial(Functional* obj, const j3x::Obj& data, Character* user)
{
    LOG.info("[SpecialFunction] Activating special.");
    auto special_id = j3x::getObj<int>(data);
    auto special = framework_->getMap()->getObjectById<Special>(special_id);
    special->activate();
}

void SpecialFunctions::spawnDestruction(Functional* obj, const j3x::Obj& data, Character* user)
{
    LOG.info("[SpecialFunction] Spawning destruction.");
    auto object = dynamic_cast<AbstractPhysicalObject*>(obj);
    framework_->spawnDecoration(object->getPosition(), "destroyed_wall_" + std::to_string(utils::num::getRandom(1, 3)));
}

void SpecialFunctions::takeRag3(Functional* obj, const j3x::Obj& data, Character* user)
{
    LOG.info("[SpecialFunction] Taking Rag3.");

    framework_->setRag3Time(CONF<float>("rag3_time"));
    user->setSpeedFactor(CONF<float>("rag3_speed_factor"), CONF<float>("rag3_time"));
    framework_->spawnThought(user, "RAG3!");
}

void SpecialFunctions::payRespect(Functional* obj, const j3x::Obj& data, Character* user)
{
    framework_->spawnThought(user, "Respect BRO!");
}

void SpecialFunctions::zoomTo(Functional* obj, const j3x::Obj& data, Character* user)
{
    LOG.info("[SpecialFunction] Zooming to.");

    auto npc = framework_->getMap()->getObjectById<NPC>(j3x::getObj<int>(data));
    framework_->forceZoomTo(npc);
}

void SpecialFunctions::spawnCrystal(Functional* obj, const j3x::Obj& data, Character* user)
{
    LOG.info("[SpecialFunction] Spawning crystal.");
    auto pos = j3x::getObj<sf::Vector2f>(data);
    framework_->spawnSpecial(pos, "crystal");
}