//
// Created by jul3x on 31.03.20.
//

#ifndef RAG3_GAME_INCLUDE_MISC_SPECIALFUNCTIONS_H
#define RAG3_GAME_INCLUDE_MISC_SPECIALFUNCTIONS_H


#include <unordered_map>
#include <functional>
#include <string>

#include <common/misc/Functional.h>


class Framework;

class SpecialFunctions {
public:
    using SpecialFunction = std::function<void(Functional*, const j3x::Obj&, Character*)>;
    explicit SpecialFunctions(Framework* framework);

    void mapStart(Functional* obj, const j3x::Obj& data, Character* user);
    void mapEnd(Functional* obj, const j3x::Obj& data, Character* user);

    void payRespect(Functional* obj, const j3x::Obj& data, Character* user);
    void turnLight(Functional* obj, const j3x::Obj& data, Character* user);
    void pourWater(Functional* obj, const j3x::Obj& data, Character* user);
    void openDoor(Functional* obj, const j3x::Obj& data, Character* user);
    void changeOpenState(Functional* obj, const j3x::Obj& data, Character* user);

    void readNote(Functional* obj, const j3x::Obj& data, Character* user);
    void addWeapon(Functional* obj, const j3x::Obj& dat, Character* user);
    void addAmmo(Functional* obj, const j3x::Obj& data, Character* user);
    void addHealth(Functional* obj, const j3x::Obj& data, Character* user);
    void addSpeed(Functional* obj, const j3x::Obj& data, Character* user);
    void takeRag3(Functional* obj, const j3x::Obj& data, Character* user);
    void pickCrystal(Functional* obj, const j3x::Obj& data, Character* user);

    void activateSpecial(Functional* obj, const j3x::Obj& data, Character* user);
    void activateWeapon(Functional* obj, const j3x::Obj& data, Character* user);
    void teleport(Functional* obj, const j3x::Obj& data, Character* user);
    void kill(Functional* obj, const j3x::Obj& data, Character* user);
    void killObj(Functional* obj, const j3x::Obj& data, Character* user);
    void zoomTo(Functional* obj, const j3x::Obj& data, Character* user);
    void setOnFire(Functional* obj, const j3x::Obj& data, Character* user);
    void explode(Functional* obj, const j3x::Obj& data, Character* user);

    void removeDecoration(Functional* obj, const j3x::Obj& data, Character* user);
    void removeSpecial(Functional* obj, const j3x::Obj& data, Character* user);
    void spawnExplosionEvent(Functional* obj, const j3x::Obj& data, Character* user);
    void spawnExplosionEventByPos(Functional* obj, const j3x::Obj& data, Character* user);
    void spawnThought(Functional* obj, const j3x::Obj& data, Character* user);
    void spawnPlayerThought(Functional* obj, const j3x::Obj& data, Character* user);
    void spawnLava(Functional* obj, const j3x::Obj& data, Character* user);
    void spawnDestruction(Functional* obj, const j3x::Obj& data, Character* user);
    void spawnMiniLava(Functional* obj, const j3x::Obj& data, Character* user);
    void spawnAmmo(Functional* obj, const j3x::Obj& data, Character* user);
    void spawnFlame(Functional* obj, const j3x::Obj& data, Character* user);
    void spawnCrystal(Functional* obj, const j3x::Obj& data, Character* user);

    void nullFunc(Functional* obj, const j3x::Obj& data, Character* user);
    void deactivate(Functional* obj, const j3x::Obj& data, Character* user);
    void destroy(Functional* obj, const j3x::Obj& data, Character* user);

    const SpecialFunction& bindFunction(const std::string& key) const;
    const std::string& bindTextToUse(const std::string& key) const;
    bool isUsableByNPC(const std::string& key) const;

private:
    std::unordered_map<std::string, std::tuple<SpecialFunction, std::string, bool>> functions_;

    Framework* framework_;

};

#endif //RAG3_GAME_INCLUDE_MISC_SPECIALFUNCTIONS_H
