//
// Created by jul3x on 31.03.20.
//

#include <common/ResourceManager.h>

#include <misc/SpecialFunctions.h>

#include <Game.h>


SpecialFunctions::SpecialFunctions()
{
    functions_["MapStart"] = &mapStart;
    functions_["MapEnd"]= &mapEnd;
    functions_["OpenDoor"] = &openDoor;
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

    door->changeTexture(&RM.getTexture("obstacles/" + door->getId() + "_open"));
    door->changeCollisionArea(Collision::None());

    obj->changeTexture(&RM.getTexture("specials/" + obj->getId() + "_open"));

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
