//
// Created by jul3x on 31.03.20.
//

#ifndef RAG3_GAME_MISC_SPECIALFUNCTIONS_H
#define RAG3_GAME_MISC_SPECIALFUNCTIONS_H


#include <unordered_map>
#include <functional>
#include <string>

#include <common/Special.h>


class SpecialFunctions
{
public:
    SpecialFunctions();

    static void mapStart(Special* obj, const std::string& data);
    static void mapEnd(Special* obj, const std::string& data);
    static void openDoor(Special* obj, const std::string& data);
    static void readNote(Special* obj, const std::string& data);

    std::function<void(Special*, const std::string&)> bindFunction(const std::string& key) const;
    const std::string& bindTextToUse(const std::string& key) const;

private:
    std::unordered_map<std::string, std::function<void(Special*, const std::string&)>> functions_;
    std::unordered_map<std::string, std::string> text_to_use_;

};

#endif //RAG3_GAME_MISC_SPECIALFUNCTIONS_H
