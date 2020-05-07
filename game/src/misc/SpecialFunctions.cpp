//
// Created by jul3x on 31.03.20.
//

#include <common/ResourceManager.h>
#include <common/ShootingWeapon.h>

#include <misc/SpecialFunctions.h>

#include <Game.h>


SpecialFunctions::SpecialFunctions()
{
    functions_["MapStart"] = &mapStart;
    functions_["MapEnd"]= &mapEnd;
    functions_["OpenDoor"] = &openDoor;
    functions_["ReadNote"] = &readNote;
    functions_["AddWeapon"] = &addWeapon;
    functions_["AddAmmo"] = &addAmmo;
    functions_["AddHealth"] = &addHealth;
    functions_["AddSpeed"] = &addSpeed;
    functions_["PickCrystal"] = &pickCrystal;
    functions_["SpawnThought"] = &spawnThought;
    functions_["ChangeOpenState"] = &changeOpenState;
    functions_["Teleport"] = &teleport;
    functions_["Kill"] = &kill;
    functions_["SetOnFire"] = &setOnFire;
    functions_["Explode"] = &explode;
    functions_["RemoveDecoration"] = &removeDecoration;
    functions_["SpawnLava"] = &spawnLava;
    functions_["SpawnExplosionEvent"] = &spawnExplosionEvent;
    functions_["SpawnFlame"] = &spawnFlame;
    functions_["SpawnAmmo"] = &spawnAmmo;
    functions_["Null"] = &nullFunc;
    functions_["Deactivate"] = &deactivate;

    text_to_use_["MapStart"] = "[F] Start new map";
    text_to_use_["MapEnd"] = "[F] End this map";
    text_to_use_["OpenDoor"] = "[F] Use object";
    text_to_use_["ReadNote"] = "[F] Read note";
    text_to_use_["AddWeapon"] = "[F} Pick weapon";
    text_to_use_["AddAmmo"] = "[F] Pick ammunition";
    text_to_use_["AddHealth"] = "[F] Pick to heal yourself";
    text_to_use_["AddSpeed"] = "[F] Pick to inject";
    text_to_use_["PickCrystal"] = "[F] Pick crystal";
    text_to_use_["SpawnThought"] = "[F] Talk";
    text_to_use_["ChangeOpenState"] = "[F] Use object";
    text_to_use_["Teleport"] = "[F] To enter";
    text_to_use_["Kill"] = "[F] To use";
    text_to_use_["Explode"] = "";
    text_to_use_["SetOnFire"] = "";
    text_to_use_["RemoveDecoration"] = "";
    text_to_use_["SpawnLava"] = "";
    text_to_use_["SpawnExplosionEvent"] = "";
    text_to_use_["SpawnAmmo"] = "";
    text_to_use_["SpawnFlame"] = "";
    text_to_use_["Null"] = "";
    text_to_use_["Deactivate"] = "";

    is_usable_by_npc_["MapStart"] = false;
    is_usable_by_npc_["MapEnd"]= false;
    is_usable_by_npc_["OpenDoor"] = true;
    is_usable_by_npc_["ReadNote"] = true;
    is_usable_by_npc_["AddWeapon"] = false;
    is_usable_by_npc_["AddAmmo"] = false;
    is_usable_by_npc_["AddHealth"] = false;
    is_usable_by_npc_["AddSpeed"] = false;
    is_usable_by_npc_["PickCrystal"] = false;
    is_usable_by_npc_["SpawnThought"] = true;
    is_usable_by_npc_["ChangeOpenState"] = true;
    is_usable_by_npc_["Teleport"] = true;
    is_usable_by_npc_["Kill"] = true;
    is_usable_by_npc_["SetOnFire"] = true;
    is_usable_by_npc_["Explode"] = false;
    is_usable_by_npc_["RemoveDecoration"] = false;
    is_usable_by_npc_["SpawnLava"] = false;
    is_usable_by_npc_["SpawnExplosionEvent"] = false;
    is_usable_by_npc_["SpawnAmmo"] = false;
    is_usable_by_npc_["SpawnFlame"] = false;
    is_usable_by_npc_["Null"] = true;
    is_usable_by_npc_["Deactivate"] = true;
}

void SpecialFunctions::mapStart(Functional* obj, const std::string& data, Character* user)
{
    // empty
}

void SpecialFunctions::mapEnd(Functional* obj, const std::string& data, Character* user)
{
    std::cout << "[SpecialFunction] Map end." << std::endl;
    user->setHealth(0);
}

void SpecialFunctions::openDoor(Functional* obj, const std::string& data, Character* user)
{
    std::cout << "[SpecialFunction] Open door." << std::endl;
    auto door_id = std::stoi(data);
    auto door = Game::get().getMap().getObjectById<Obstacle>(door_id);
    auto grid_pos = std::make_pair(static_cast<size_t>(door->getPosition().x / DecorationTile::SIZE_X_),
                                   static_cast<size_t>(door->getPosition().y / DecorationTile::SIZE_Y_));

    if (door->getCollisionArea().getType() == collision::Area::Type::None)
    {
        door->changeTexture(&RM.getTexture("obstacles/" + door->getId()));
        door->changeCollisionArea(collision::Box(utils::j3x::get<float>(RM.getObjectParams("obstacles", door->getId()), "collision_size_x"),
                                                 utils::j3x::get<float>(RM.getObjectParams("obstacles", door->getId()), "collision_size_y")));

        Game::get().getMap().getMapBlockage().blockage_.at(grid_pos.first).at(grid_pos.second) =
                utils::j3x::get<float>(RM.getObjectParams("obstacles", door->getId()), "endurance");
    }
    else
    {
        door->changeTexture(&RM.getTexture("obstacles/" + door->getId() + "_open"));
        door->changeCollisionArea(collision::None());

        Game::get().getMap().getMapBlockage().blockage_.at(grid_pos.first).at(grid_pos.second) = 0.0f;
    }
}

void SpecialFunctions::changeOpenState(Functional* obj, const std::string& data, Character* user)
{
    std::cout << "[SpecialFunction] Changing open state." << std::endl;
    auto door_id = std::stoi(data);
    auto door = Game::get().getMap().getObjectById<Obstacle>(door_id);
    auto special_obj = Game::get().getMap().getObjectById<Special>(obj->getUniqueId());

    if (door->getCollisionArea().getType() == collision::Area::Type::None)
    {
        special_obj->changeTexture(&RM.getTexture("specials/" + obj->getId()));
    }
    else
    {
        special_obj->changeTexture(&RM.getTexture("specials/" + obj->getId() + "_open"));
    }
}

void SpecialFunctions::readNote(Functional* obj, const std::string& data, Character* user)
{
    std::cout << "[SpecialFunction] Reading note." << std::endl;
    auto str = data;
    std::replace(str.begin(), str.end(), '$', '\n');
    Game::get().spawnThought(user, str);
}

void SpecialFunctions::addWeapon(Functional* obj, const std::string& data, Character* user)
{
    std::cout << "[SpecialFunction] Adding weapon." << std::endl;

    auto data_parsed = data;
    std::replace(data_parsed.begin(), data_parsed.end(), ' ', '_');

    std::shared_ptr<AbstractWeapon> weapon = std::make_shared<ShootingWeapon>(data_parsed);

    if (!weapon->getName().empty())
        weapon->registerSpawningFunction(
                Game::get().getSpawningFunction(utils::j3x::get<std::string>(
                        RM.getObjectParams("weapons", weapon->getName()), "spawn_func")));

    user->addWeaponToBackpack(weapon);

    obj->deactivate();
}

void SpecialFunctions::addAmmo(Functional* obj, const std::string& data, Character* user)
{
    std::cout << "[SpecialFunction] Adding ammo." << std::endl;
    auto data_parsed = data;
    std::replace(data_parsed.begin(), data_parsed.end(), ' ', '_');

    user->addAmmoToWeapon(data_parsed);

    obj->deactivate();
}

void SpecialFunctions::addHealth(Functional* obj, const std::string& data, Character* user)
{
    std::cout << "[SpecialFunction] Adding health." << std::endl;
    auto data_parsed = std::stoi(data);

    user->setHealth(std::min(user->getHealth() + data_parsed, user->getMaxHealth()));

    Game::get().spawnThought(user, "Uff!\nThat's what I needed...");

    obj->deactivate();
}

void SpecialFunctions::addSpeed(Functional* obj, const std::string& data, Character* user)
{
    std::cout << "[SpecialFunction] Adding speed." << std::endl;
    auto data_parsed = std::stof(data);

    user->setSpeedFactor(user->getSpeedFactor() + data_parsed);

    Game::get().spawnThought(user, "Woah!\nI feel... faster...");

    obj->deactivate();
}

void SpecialFunctions::pickCrystal(Functional* obj, const std::string& data, Character* user)
{
    std::cout << "[SpecialFunction] Picking crystal." << std::endl;

    Game::get().getStats().pickCrystal();

    Game::get().spawnThought(user, "I need more of them!");

    obj->deactivate();
}

void SpecialFunctions::spawnThought(Functional* obj, const std::string& data, Character* user)
{
    std::cout << "[SpecialFunction] Spawning thought." << std::endl;
    auto str = data;
    std::replace(str.begin(), str.end(), '$', '\n');
    Game::get().spawnThought(user, str);
}

void SpecialFunctions::teleport(Functional* obj, const std::string& data, Character* user)
{
    std::cout << "[SpecialFunction] Teleport." << std::endl;
    auto player = dynamic_cast<Player*>(user);

    try
    {
        auto pos = utils::j3x::convert<sf::Vector2f>(data);

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

void SpecialFunctions::setOnFire(Functional* obj, const std::string& data, Character* user)
{
    std::cout << "[SpecialFunction] Setting on fire." << std::endl;
    user->setGlobalState(Character::GlobalState::OnFire);
}

void SpecialFunctions::explode(Functional* obj, const std::string& data, Character* user)
{
    std::cout << "[SpecialFunction] Explode." << std::endl;
    auto obs = dynamic_cast<AbstractPhysicalObject*>(obj);

    Game::get().spawnExplosionForce(obs->getPosition(), std::stof(data));

    Game::get().getStats().explode();
}

void SpecialFunctions::removeDecoration(Functional* obj, const std::string& data, Character* user)
{
    std::cout << "[SpecialFunction] Remove decoration." << std::endl;
    auto decoration_id = std::stoi(data);
    auto decoration = Game::get().getMap().getObjectById<Decoration>(decoration_id);
    decoration->deactivate();
}

void SpecialFunctions::spawnExplosionEvent(Functional* obj, const std::string& data, Character* user)
{
    std::cout << "[SpecialFunction] Spawn explosion event." << std::endl;
    auto obs = dynamic_cast<AbstractPhysicalObject*>(obj);
    Game::get().spawnExplosionEvent(obs->getPosition(), std::stof(data));
}

void SpecialFunctions::spawnLava(Functional* obj, const std::string& data, Character* user)
{
    std::cout << "[SpecialFunction] Spawn lava." << std::endl;
    auto pos = utils::j3x::convert<sf::Vector2f>(data);
    Game::get().spawnSpecial(pos, "lava");
    Game::get().spawnExplosionEvent(pos, 250.0f);
}

void SpecialFunctions::spawnAmmo(Functional* obj, const std::string& data, Character* user)
{
    std::cout << "[SpecialFunction] Spawn ammo." << std::endl;

    auto npc = dynamic_cast<Character*>(obj);

    auto ammo_offset = CFG.get<float>("characters/ammo_drop_offset");
    for (size_t i = 0; i < CFG.get<int>("characters/ammo_dropped"); ++i)
    {
        auto offset = sf::Vector2f(utils::num::getRandom(-ammo_offset, ammo_offset),
                                   utils::num::getRandom(-ammo_offset, ammo_offset));
        Game::get().spawnSpecial(npc->getPosition() + offset, data + "_ammo");
    }
}

void SpecialFunctions::spawnFlame(Functional* obj, const std::string& data, Character* user)
{
    std::cout << "[SpecialFunction] Spawn flame." << std::endl;
    auto pos = utils::j3x::convert<sf::Vector2f>(data);
    Game::get().spawnSpecial(pos, "flame");
    Game::get().spawnExplosionEvent(pos, 250.0f);
}


void SpecialFunctions::kill(Functional* obj, const std::string& data, Character* user)
{
    std::cout << "[SpecialFunction] Kill." << std::endl;
    user->setHealth(0);

    // Game::get().spawnAnimationEvent(data);
}

void SpecialFunctions::nullFunc(Functional *obj, const std::string &data, Character* user)
{
    std::cout << "[SpecialFunction] Null." << std::endl;
}

void SpecialFunctions::deactivate(Functional *obj, const std::string &data, Character* user)
{
    std::cout << "[SpecialFunction] Deactivating." << std::endl;
    obj->deactivate();
}

std::function<void(Functional*, const std::string&, Character*)>
SpecialFunctions::bindFunction(const std::string& key) const
{
    auto it = functions_.find(key);

    if (it == functions_.end())
    {
        throw std::invalid_argument("[SpecialFunctions] Function " + key + " is not handled!");
    }

    return it->second;
}

const std::string& SpecialFunctions::bindTextToUse(const std::string &key) const
{
    auto it = text_to_use_.find(key);

    if (it == text_to_use_.end())
    {
        throw std::invalid_argument("[SpecialFunctions] Function " + key + " is not handled!");
    }

    return it->second;
}

bool SpecialFunctions::isUsableByNPC(const std::string& key) const
{
    auto it = is_usable_by_npc_.find(key);

    if (it == is_usable_by_npc_.end())
    {
        throw std::invalid_argument("[SpecialFunctions] Function " + key + " is not handled!");
    }

    return it->second;
}
