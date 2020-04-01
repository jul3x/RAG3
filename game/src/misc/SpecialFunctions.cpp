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
    functions_["ReadNote"] = &readNote;
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
    Game::get().spawnThought(data);
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
