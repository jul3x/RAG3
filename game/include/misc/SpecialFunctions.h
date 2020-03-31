//
// Created by jul3x on 31.03.20.
//

#ifndef RAG3_GAME_MISC_SPECIALFUNCTIONS_H
#define RAG3_GAME_MISC_SPECIALFUNCTIONS_H


#include <unordered_map>
#include <functional>
#include <string>

class SpecialFunctions
{
public:
    SpecialFunctions();

    static void mapStart(const std::string& data);
    static void mapEnd(const std::string& data);

    std::function<void(const std::string&)> bindFunction(const std::string& key) const;

private:
    std::unordered_map<std::string, std::function<void(const std::string&)>> functions_;

};

#endif //RAG3_GAME_MISC_SPECIALFUNCTIONS_H
