//
// Created by jul3x on 31.03.20.
//

#include <misc/SpecialFunctions.h>

#include <Game.h>


SpecialFunctions::SpecialFunctions()
{
    functions_["MapStart"] = &mapStart;
    functions_["MapEnd"]= &mapEnd;
}

void SpecialFunctions::mapStart(const std::string& data)
{
    // empty
}

void SpecialFunctions::mapEnd(const std::string& data)
{
    Game::get().getPlayer().setHealth(0);
}

std::function<void(const std::string&)> SpecialFunctions::bindFunction(const std::string& key) const
{
    auto it = functions_.find(key);

    if (it == functions_.end())
    {
        throw std::invalid_argument("[SpecialFunctions] Function " + key + " is not handled!");
    }

    return it->second;
}
