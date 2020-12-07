//
// Created by jul3x on 31.03.20.
//

#ifndef RAG3_GAME_INCLUDE_MISC_SPECIALFUNCTIONS_H
#define RAG3_GAME_INCLUDE_MISC_SPECIALFUNCTIONS_H


#include <unordered_map>
#include <functional>
#include <string>

#include <common/Functional.h>


class SpecialFunctions
{
public:
    using SpecialFunction = std::function<void(Functional*, const j3x::Obj&, Character*)>;
    SpecialFunctions();

    static void mapStart(Functional* obj, const j3x::Obj& data, Character* user);
    static void mapEnd(Functional* obj, const j3x::Obj& data, Character* user);

    static void openDoor(Functional* obj, const j3x::Obj& data, Character* user);
    static void changeOpenState(Functional* obj, const j3x::Obj& data, Character* user);

    static void readNote(Functional* obj, const j3x::Obj& data, Character* user);
    static void addWeapon(Functional* obj, const j3x::Obj& dat, Character* user);
    static void addAmmo(Functional* obj, const j3x::Obj& data, Character* user);
    static void addHealth(Functional* obj, const j3x::Obj& data, Character* user);
    static void addSpeed(Functional* obj, const j3x::Obj& data, Character* user);
    static void takeRag3(Functional* obj, const j3x::Obj& data, Character* user);
    static void pickCrystal(Functional* obj, const j3x::Obj& data, Character* user);

    static void activateSpecial(Functional* obj, const j3x::Obj& data, Character* user);
    static void activateWeapon(Functional* obj, const j3x::Obj& data, Character* user);
    static void teleport(Functional* obj, const j3x::Obj& data, Character* user);
    static void kill(Functional* obj, const j3x::Obj& data, Character* user);
    static void setOnFire(Functional* obj, const j3x::Obj& data, Character* user);
    static void explode(Functional* obj, const j3x::Obj& data, Character* user);

    static void removeDecoration(Functional* obj, const j3x::Obj& data, Character* user);
    static void removeSpecial(Functional* obj, const j3x::Obj& data, Character* user);
    static void spawnExplosionEvent(Functional* obj, const j3x::Obj& data, Character* user);
    static void spawnExplosionEventByPos(Functional* obj, const j3x::Obj& data, Character* user);
    static void spawnThought(Functional* obj, const j3x::Obj& data, Character* user);
    static void spawnPlayerThought(Functional* obj, const j3x::Obj& data, Character* user);
    static void spawnLava(Functional* obj, const j3x::Obj& data, Character* user);
    static void spawnDestruction(Functional* obj, const j3x::Obj& data, Character* user);
    static void spawnMiniLava(Functional* obj, const j3x::Obj& data, Character* user);
    static void spawnAmmo(Functional* obj, const j3x::Obj& data, Character* user);
    static void spawnFlame(Functional* obj, const j3x::Obj& data, Character* user);

    static void nullFunc(Functional* obj, const j3x::Obj& data, Character* user);
    static void deactivate(Functional* obj, const j3x::Obj& data, Character* user);
    static void destroy(Functional* obj, const j3x::Obj& data, Character* user);

    const SpecialFunction& bindFunction(const std::string& key) const;
    const std::string& bindTextToUse(const std::string& key) const;
    bool isUsableByNPC(const std::string& key) const;

private:
    std::unordered_map<std::string, std::tuple<SpecialFunction, std::string, bool>> functions_;

};

#endif //RAG3_GAME_INCLUDE_MISC_SPECIALFUNCTIONS_H
