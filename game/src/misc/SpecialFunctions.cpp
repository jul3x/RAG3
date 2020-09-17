//
// Created by jul3x on 31.03.20.
//

#include <common/ResourceManager.h>
#include <common/ShootingWeapon.h>

#include <misc/SpecialFunctions.h>

#include <Game.h>


SpecialFunctions::SpecialFunctions()
{
    functions_["MapStart"] = std::make_tuple(&mapStart, "[F] Start new map", false);
    functions_["MapEnd"]= std::make_tuple(&mapEnd, "[F] End this map", false);
    functions_["OpenDoor"] = std::make_tuple(&openDoor, "", true);
    functions_["ReadNote"] = std::make_tuple(&readNote, "[F] Read note", true);
    functions_["AddWeapon"] = std::make_tuple(&addWeapon, "[F} Pick weapon", false);
    functions_["AddAmmo"] = std::make_tuple(&addAmmo, "[F] Pick ammunition", false);
    functions_["AddHealth"] = std::make_tuple(&addHealth, "[F] Pick to heal yourself", false);
    functions_["AddSpeed"] = std::make_tuple(&addSpeed, "[F] Pick to inject", false);
    functions_["PickCrystal"] = std::make_tuple(&pickCrystal, "[F] Pick crystal", false);
    functions_["SpawnThought"] = std::make_tuple(&spawnThought, "", true);
    functions_["SpawnPlayerThought"] = std::make_tuple(&spawnPlayerThought, "", true);
    functions_["ChangeOpenState"] = std::make_tuple(&changeOpenState, "[F] Pull the trigger", true);
    functions_["Teleport"] = std::make_tuple(&teleport, "", true);
    functions_["Kill"] = std::make_tuple(&kill, "", true);
    functions_["ActivateWeapon"] = std::make_tuple(&activateWeapon, "", false);
    functions_["SetOnFire"] = std::make_tuple(&setOnFire, "", true);
    functions_["Explode"] = std::make_tuple(&explode, "", false);
    functions_["RemoveDecoration"] = std::make_tuple(&removeDecoration, "", false);
    functions_["RemoveSpecial"] = std::make_tuple(&removeSpecial, "", false);
    functions_["SpawnLava"] = std::make_tuple(&spawnLava, "", false);
    functions_["SpawnExplosionEvent"] = std::make_tuple(&spawnExplosionEvent, "", false);
    functions_["SpawnExplosionEventByPos"] = std::make_tuple(&spawnExplosionEventByPos, "", true);
    functions_["SpawnMiniLava"] = std::make_tuple(&spawnMiniLava, "", false);
    functions_["SpawnFlame"] = std::make_tuple(&spawnFlame, "", false);
    functions_["SpawnAmmo"] = std::make_tuple(&spawnAmmo, "", false);
    functions_["SpawnDestruction"] = std::make_tuple(&spawnDestruction, "", true);
    functions_["Null"] = std::make_tuple(&nullFunc, "", true);
    functions_["Deactivate"] = std::make_tuple(&deactivate, "", true);
    functions_["Destroy"] = std::make_tuple(&destroy, "", true);
    functions_["ActivateSpecial"] = std::make_tuple(&activateSpecial, "", true);
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
    std::cout << "[SpecialFunction] Map end." << std::endl;
    user->setHealth(0);
}

void SpecialFunctions::openDoor(Functional* obj, const j3x::Obj& data, Character* user)
{
    std::cout << "[SpecialFunction] Open door." << std::endl;
    auto door_id = j3x::getObj<int>(data);
    auto door = Game::get().getMap().getObjectById<Obstacle>(door_id);

    auto grid_pos = std::make_pair(std::round((door->getPosition().x + RMGET<sf::Vector2f>("obstacles", door->getId(), "collision_offset").x) / DecorationTile::SIZE_X_),
                                   std::round((door->getPosition().y + RMGET<sf::Vector2f>("obstacles", door->getId(), "collision_offset").y) / DecorationTile::SIZE_Y_));

    if (door->getCollisionArea().getType() == collision::Area::Type::None)
    {
        door->changeTexture(&RM.getTexture("obstacles/" + door->getId()));
        door->changeCollisionArea(collision::Box(RMGET<sf::Vector2f>("obstacles", door->getId(), "collision_size").x,
                                                 RMGET<sf::Vector2f>("obstacles", door->getId(), "collision_size").y,
                                                 RMGET<sf::Vector2f>("obstacles", door->getId(), "collision_offset")));
        door->setZIndex(RMGET<int>("obstacles", door->getId(), "z_index"));

        Game::get().getMap().getMapBlockage().blockage_.at(grid_pos.first).at(grid_pos.second) =
                RMGET<float>("obstacles", door->getId(), "endurance");
    }
    else
    {
        door->changeTexture(&RM.getTexture("obstacles/" + door->getId() + "_open"));
        door->changeCollisionArea(collision::None());
        door->setZIndex(RMGET<int>("obstacles", door->getId() + "_open", "z_index"));

        Game::get().getMap().getMapBlockage().blockage_.at(grid_pos.first).at(grid_pos.second) = 0.0f;
    }

    if (user != nullptr)
        Game::get().getJournal().event<DoorOpen>(door);
}

void SpecialFunctions::changeOpenState(Functional* obj, const j3x::Obj& data, Character* user)
{
    std::cout << "[SpecialFunction] Changing open state." << std::endl;
    auto special_obj = Game::get().getMap().getObjectById<Special>(obj->getUniqueId());

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
        Game::get().spawnEvent("dust", special_obj->getPosition() + sf::Vector2f{0.0f, 10.0f});
        Game::get().getJournal().event<ChangeOpenState>(special_obj);
    }
}

void SpecialFunctions::readNote(Functional* obj, const j3x::Obj& data, Character* user)
{
    std::cout << "[SpecialFunction] Reading note." << std::endl;
    Game::get().spawnThought(user, j3x::getObj<std::string>(data));
}

void SpecialFunctions::addWeapon(Functional* obj, const j3x::Obj& data, Character* user)
{
    std::cout << "[SpecialFunction] Adding weapon." << std::endl;

    auto& data_parsed = j3x::getObj<std::string>(data);
    std::shared_ptr<AbstractWeapon> weapon;
    if (data_parsed.length() > 5 && data_parsed.substr(0, 5) == "melee")
    {
        weapon = std::make_shared<MeleeWeapon>(user, data_parsed);
    }
    else
    {
        weapon = std::make_shared<ShootingWeapon>(user, data_parsed);
    }

    if (user->addWeaponToBackpack(weapon))
    {
        Game::get().registerWeapon(weapon.get());
    }
}

void SpecialFunctions::addAmmo(Functional* obj, const j3x::Obj& data, Character* user)
{
    std::cout << "[SpecialFunction] Adding ammo." << std::endl;
    user->addAmmoToWeapon(j3x::getObj<std::string>(data));
}

void SpecialFunctions::addHealth(Functional* obj, const j3x::Obj& data, Character* user)
{
    std::cout << "[SpecialFunction] Adding health." << std::endl;
    auto data_parsed = j3x::getObj<int>(data);

    user->setHealth(std::min(user->getHealth() + data_parsed, user->getMaxHealth()));

    Game::get().spawnThought(user, "Uff!\nThat's what I needed...");
}

void SpecialFunctions::addSpeed(Functional* obj, const j3x::Obj& data, Character* user)
{
    std::cout << "[SpecialFunction] Adding speed." << std::endl;
    auto data_parsed = j3x::getObj<float>(data);

    user->setSpeedFactor(user->getSpeedFactor() + data_parsed);

    Game::get().spawnThought(user, "Woah!\nI feel... faster...");
}

void SpecialFunctions::pickCrystal(Functional* obj, const j3x::Obj& data, Character* user)
{
    std::cout << "[SpecialFunction] Picking crystal." << std::endl;

    Game::get().getStats().pickCrystal();

    Game::get().spawnThought(user, "I need more of them!");
}

void SpecialFunctions::spawnThought(Functional* obj, const j3x::Obj& data, Character* user)
{
    std::cout << "[SpecialFunction] Spawning thought." << std::endl;
    Game::get().spawnThought(user, j3x::getObj<std::string>(data));
}

void SpecialFunctions::teleport(Functional* obj, const j3x::Obj& data, Character* user)
{
    std::cout << "[SpecialFunction] Teleport." << std::endl;
    auto player = dynamic_cast<Player*>(user);

    try
    {
        auto pos = j3x::getObj<sf::Vector2f>(data);

        user->setPosition(pos);
        user->setForcedVelocity({0.0f, 0.0f});

        if (player != nullptr)
        {
            Game::get().spawnFadeInOut();
            Game::get().getCamera().setCenter({player->getPosition().x, player->getPosition().y, 0.0f});
        }
    }
    catch (const std::exception& e)
    {
        throw std::invalid_argument("[SpecialFunctions] Bad format of position type in function data!");
    }
}

void SpecialFunctions::setOnFire(Functional* obj, const j3x::Obj& data, Character* user)
{
    std::cout << "[SpecialFunction] Setting on fire." << std::endl;
    user->setGlobalState(Character::GlobalState::OnFire);
}

void SpecialFunctions::explode(Functional* obj, const j3x::Obj& data, Character* user)
{
    std::cout << "[SpecialFunction] Explode." << std::endl;
    auto obs = dynamic_cast<AbstractPhysicalObject*>(obj);

    Game::get().spawnExplosionForce(obs->getPosition(), j3x::getObj<float>(data));

    Game::get().getStats().explode();
}

void SpecialFunctions::removeDecoration(Functional* obj, const j3x::Obj& data, Character* user)
{
    std::cout << "[SpecialFunction] Remove decoration." << std::endl;
    auto decoration_id = j3x::getObj<int>(data);
    auto decoration = Game::get().getMap().getObjectById<Decoration>(decoration_id);
    decoration->deactivate();
}

void SpecialFunctions::spawnExplosionEvent(Functional* obj, const j3x::Obj& data, Character* user)
{
    std::cout << "[SpecialFunction] Spawn explosion event." << std::endl;
    auto obs = dynamic_cast<AbstractPhysicalObject*>(obj);
    Game::get().spawnExplosionEvent(obs->getPosition(), j3x::getObj<float>(data));
}

void SpecialFunctions::spawnExplosionEventByPos(Functional* obj, const j3x::Obj& data, Character* user)
{
    std::cout << "[SpecialFunction] Spawn explosion event by pos." << std::endl;
    auto pos = j3x::getObj<sf::Vector2f>(data);
    Game::get().spawnExplosionEvent(pos, 250.0f);
}

void SpecialFunctions::spawnLava(Functional* obj, const j3x::Obj& data, Character* user)
{
    std::cout << "[SpecialFunction] Spawn lava." << std::endl;
    auto pos = j3x::getObj<sf::Vector2f>(data);
    Game::get().spawnSpecial(pos, "lava");
}

void SpecialFunctions::spawnMiniLava(Functional* obj, const j3x::Obj& data, Character* user)
{
    std::cout << "[SpecialFunction] Spawn mini lava." << std::endl;
    auto pos = j3x::getObj<sf::Vector2f>(data);
    Game::get().spawnSpecial(pos, "mini_lava");
}

void SpecialFunctions::spawnAmmo(Functional* obj, const j3x::Obj& data, Character* user)
{
    std::cout << "[SpecialFunction] Spawn ammo." << std::endl;

    auto npc = dynamic_cast<Character*>(obj);

    auto ammo_offset = CONF<float>("characters/ammo_drop_offset");
    for (size_t i = 0; i < CONF<int>("characters/ammo_dropped"); ++i)
    {
        auto offset = sf::Vector2f(utils::num::getRandom(-ammo_offset, ammo_offset),
                                   utils::num::getRandom(-ammo_offset, ammo_offset));
        Game::get().spawnSpecial(npc->getPosition() + offset, j3x::getObj<std::string>(data) + "_ammo");
    }
}

void SpecialFunctions::spawnFlame(Functional* obj, const j3x::Obj& data, Character* user)
{
    std::cout << "[SpecialFunction] Spawn flame." << std::endl;
    auto pos = j3x::getObj<sf::Vector2f>(data);
    Game::get().spawnSpecial(pos, "flame");
    Game::get().spawnExplosionEvent(pos, 250.0f);
}

void SpecialFunctions::kill(Functional* obj, const j3x::Obj& data, Character* user)
{
    std::cout << "[SpecialFunction] Kill." << std::endl;
    user->setHealth(0);

    // Game::get().spawnAnimationEvent(data);
}

void SpecialFunctions::nullFunc(Functional *obj, const j3x::Obj &data, Character* user)
{
    std::cout << "[SpecialFunction] Null." << std::endl;
}

void SpecialFunctions::deactivate(Functional *obj, const j3x::Obj &data, Character* user)
{
    std::cout << "[SpecialFunction] Deactivating." << std::endl;
    obj->deactivate();
    Game::get().getJournal().event<ObjectDeactivated>(dynamic_cast<Special*>(obj));
}

void SpecialFunctions::destroy(Functional *obj, const j3x::Obj &data, Character* user)
{
    std::cout << "[SpecialFunction] Destroying " + std::to_string(obj->getUniqueId()) + "." << std::endl;
    Game::get().spawnEvent("dust", dynamic_cast<AbstractPhysicalObject*>(obj)->getPosition());
    obj->destroy();
}

void SpecialFunctions::activateWeapon(Functional* obj, const j3x::Obj& data, Character* user)
{
    std::cout << "[SpecialFunction] Activate weapon." << std::endl;
    auto weapon_id = j3x::getObj<int>(data);
    auto weapon = Game::get().getMap().getObjectById<PlacedWeapon>(weapon_id);
    weapon->setActive(!weapon->getActive());

    Game::get().getJournal().event<WeaponActivation>(weapon);
}

void SpecialFunctions::spawnPlayerThought(Functional* obj, const j3x::Obj& data, Character* user)
{
    std::cout << "[SpecialFunction] Spawning player thought." << std::endl;
    Game::get().spawnThought(&Game::get().getPlayer(), j3x::getObj<std::string>(data));
}

void SpecialFunctions::removeSpecial(Functional* obj, const j3x::Obj& data, Character* user)
{
    std::cout << "[SpecialFunction] Remove special." << std::endl;
    auto special_id = j3x::getObj<int>(data);
    auto special = Game::get().getMap().getObjectById<Special>(special_id);
    special->destroy();
}

void SpecialFunctions::activateSpecial(Functional* obj, const j3x::Obj& data, Character* user)
{
    std::cout << "[SpecialFunction] Activate special." << std::endl;
    auto special_id = j3x::getObj<int>(data);
    auto special = Game::get().getMap().getObjectById<Special>(special_id);
    special->activate();
}

void SpecialFunctions::spawnDestruction(Functional* obj, const j3x::Obj& data, Character* user)
{
    std::cout << "[SpecialFunction] Spawn destruction." << std::endl;
    auto object = dynamic_cast<AbstractPhysicalObject*>(obj);
    Game::get().spawnDecoration(object->getPosition(), "destroyed_wall_" + std::to_string(utils::num::getRandom(1, 3)));
}
