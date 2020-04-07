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

    text_to_use_["MapStart"] = "[F] Start new map";
    text_to_use_["MapEnd"] = "[F] End this map";
    text_to_use_["OpenDoor"] = "[F] Use object";
    text_to_use_["ReadNote"] = "[F] Read note";
    text_to_use_["AddWeapon"] = "[F} Pick weapon";
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
        door->changeCollisionArea(Collision::Box(utils::getFloat(RM.getObjectParams("obstacles", door->getId()), "collision_size_x"),
                                                 utils::getFloat(RM.getObjectParams("obstacles", door->getId()), "collision_size_y")));

        obj->changeTexture(&RM.getTexture("specials/" + obj->getId()));

        Game::get().getMap().getMapBlockage().blockage_.at(grid_pos.first).at(grid_pos.second) =
                utils::getFloat(RM.getObjectParams("obstacles", door->getId()), "endurance");
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

    auto& specials = Game::get().getMap().getSpecials();

    Game::get().deleteHoveringObject(obj);
    for (auto it = specials.begin(); it != specials.end(); ++it)
    {
        if (it->get() == obj)
        {
            specials.erase(it);
            return;
        }
    }
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
