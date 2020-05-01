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

    static void mapStart(Functional* obj, const std::string& data, Character* user);
    static void mapEnd(Functional* obj, const std::string& data, Character* user);
    static void openDoor(Functional* obj, const std::string& data, Character* user);
    static void changeOpenState(Functional* obj, const std::string& data, Character* user);
    static void readNote(Functional* obj, const std::string& data, Character* user);
    static void addWeapon(Functional* obj, const std::string& dat, Character* user);
    static void addAmmo(Functional* obj, const std::string& data, Character* user);
    static void addHealth(Functional* obj, const std::string& data, Character* user);
    static void addSpeed(Functional* obj, const std::string& data, Character* user);
    static void pickCrystal(Functional* obj, const std::string& data, Character* user);
    static void spawnThought(Functional* obj, const std::string& data, Character* user);
    static void teleport(Functional* obj, const std::string& data, Character* user);
    static void kill(Functional* obj, const std::string& data, Character* user);
    static void removeDecoration(Functional* obj, const std::string& data, Character* user);
    static void spawnLava(Functional* obj, const std::string& data, Character* user);
    static void nullFunc(Functional* obj, const std::string& data, Character* user);
    static void deactivate(Functional* obj, const std::string& data, Character* user);

    std::function<void(Functional*, const std::string&, Character*)> bindFunction(const std::string& key) const;
    const std::string& bindTextToUse(const std::string& key) const;
    bool isUsableByNPC(const std::string& key) const;

private:
    std::unordered_map<std::string, std::function<void(Functional*, const std::string&, Character*)>> functions_;
    std::unordered_map<std::string, std::string> text_to_use_;
    std::unordered_map<std::string, bool> is_usable_by_npc_;

};

#endif //RAG3_GAME_MISC_SPECIALFUNCTIONS_H
