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

    text_to_use_["MapStart"] = "[F] Start new map";
    text_to_use_["MapEnd"] = "[F] End this map";
    text_to_use_["OpenDoor"] = "[F] Use object";
    text_to_use_["ReadNote"] = "[F] Read note";
    text_to_use_["AddWeapon"] = "[F} Pick weapon";
    text_to_use_["AddAmmo"] = "[F] Pick ammunition";
    text_to_use_["AddHealth"] = "[F] Pick to heal yourself";
    text_to_use_["AddSpeed"] = "[F] Pick to inject";
    text_to_use_["PickCrystal"] = "[F] Pick crystal";
}

void SpecialFunctions::mapStart(Special* obj, const std::string& data)
{
    // empty
}

void SpecialFunctions::mapEnd(Special* obj, const std::string& data)
{
    Game::get().getPlayer().setHealth(0);
}

void SpecialFunctions::openDoor(Special* obj, const std::string& data)
{
    auto door_id = std::stoi(data);
    auto door = Game::get().getMap().getObstacleObject(door_id);
    auto grid_pos = std::make_pair(static_cast<size_t>(door->getPosition().x / DecorationTile::SIZE_X_),
                                   static_cast<size_t>(door->getPosition().y / DecorationTile::SIZE_Y_));

    if (door->getCollisionArea().getType() == Collision::Area::Type::None)
    {
        door->changeTexture(&RM.getTexture("obstacles/" + door->getId()));
        door->changeCollisionArea(Collision::Box(utils::j3x::getFloat(RM.getObjectParams("obstacles", door->getId()), "collision_size_x"),
                                                 utils::j3x::getFloat(RM.getObjectParams("obstacles", door->getId()), "collision_size_y")));

        obj->changeTexture(&RM.getTexture("specials/" + obj->getId()));

        Game::get().getMap().getMapBlockage().blockage_.at(grid_pos.first).at(grid_pos.second) =
                utils::j3x::getFloat(RM.getObjectParams("obstacles", door->getId()), "endurance");
    }
    else
    {
        door->changeTexture(&RM.getTexture("obstacles/" + door->getId() + "_open"));
        door->changeCollisionArea(Collision::None());

        obj->changeTexture(&RM.getTexture("specials/" + obj->getId() + "_open"));

        Game::get().getMap().getMapBlockage().blockage_.at(grid_pos.first).at(grid_pos.second) = 0.0f;
    }
}

void SpecialFunctions::readNote(Special* obj, const std::string& data)
{
    auto str = data;
    std::replace(str.begin(), str.end(), '$', '\n');
    Game::get().spawnThought(str);
}

void SpecialFunctions::addWeapon(Special* obj, const std::string& data)
{
    auto& player = Game::get().getPlayer();
    auto data_parsed = data;
    std::replace(data_parsed.begin(), data_parsed.end(), ' ', '_');

    std::shared_ptr<AbstractWeapon> weapon = std::make_shared<ShootingWeapon>(data_parsed);
    weapon->registerSpawningFunction(
            std::bind(&Game::spawnBullet, &Game::get(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    player.addWeaponToBackpack(weapon);

    Game::get().spawnThought("Fuck yeah!");

    obj->deactivate();
}

void SpecialFunctions::addAmmo(Special* obj, const std::string& data)
{
    auto& player = Game::get().getPlayer();
    auto data_parsed = data;
    std::replace(data_parsed.begin(), data_parsed.end(), ' ', '_');

    player.addAmmoToWeapon(data_parsed);

    obj->deactivate();
}

void SpecialFunctions::addHealth(Special* obj, const std::string& data)
{
    auto& player = Game::get().getPlayer();
    auto data_parsed = std::stoi(data);

    player.setHealth(player.getHealth() + data_parsed);

    Game::get().spawnThought("Uff!\nThat's what I needed...");

    obj->deactivate();
}

void SpecialFunctions::addSpeed(Special* obj, const std::string& data)
{
    auto& player = Game::get().getPlayer();
    auto data_parsed = std::stof(data);

    player.setSpeedFactor(player.getSpeedFactor() + data_parsed);

    Game::get().spawnThought("Woah!\nI feel... faster...");

    obj->deactivate();
}

void SpecialFunctions::pickCrystal(Special* obj, const std::string& data)
{
    Game::get().getStats().pickCrystal();

    Game::get().spawnThought("I need more of them!");

    obj->deactivate();
}

std::function<void(Special*, const std::string&)> SpecialFunctions::bindFunction(const std::string& key) const
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
