//
// Created by jul3x on 31.03.20.
//

#ifndef RAG3_GAME_MISC_SPECIALFUNCTIONS_H
#define RAG3_GAME_MISC_SPECIALFUNCTIONS_H


#include <unordered_map>
#include <functional>
#include <string>

#include <common/Functional.h>


class SpecialFunctions
{
public:
    SpecialFunctions();

    static void mapStart(Functional* obj, const std::string& data);
    static void mapEnd(Functional* obj, const std::string& data);
    static void openDoor(Functional* obj, const std::string& data);
    static void changeOpenState(Functional* obj, const std::string& data);
    static void readNote(Functional* obj, const std::string& data);
    static void addWeapon(Functional* obj, const std::string& data);
    static void addAmmo(Functional* obj, const std::string& data);
    static void addHealth(Functional* obj, const std::string& data);
    static void addSpeed(Functional* obj, const std::string& data);
    static void pickCrystal(Functional* obj, const std::string& data);
    static void spawnThought(Functional* obj, const std::string& data);
    static void nullFunc(Functional* obj, const std::string& data);

    std::function<void(Functional*, const std::string&)> bindFunction(const std::string& key) const;
    const std::string& bindTextToUse(const std::string& key) const;

private:
    std::unordered_map<std::string, std::function<void(Functional*, const std::string&)>> functions_;
    std::unordered_map<std::string, std::string> text_to_use_;

};

#endif //RAG3_GAME_MISC_SPECIALFUNCTIONS_H
