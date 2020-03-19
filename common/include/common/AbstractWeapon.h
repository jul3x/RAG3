//
// Created by jul3x on 16.10.19.
//


#ifndef RAG3_GAME_WEAPONS_ABSTRACTWEAPON_H
#define RAG3_GAME_WEAPONS_ABSTRACTWEAPON_H

#include <string>
#include <functional>

#include <R3E/objects/AbstractDrawableObject.h>


using namespace r3e;

class AbstractWeapon : public AbstractDrawableObject {
public:
    AbstractWeapon(const sf::Vector2f& size,
                   const sf::Vector2f& weapon_offset,
                   const std::string& name);

    void registerSpawningFunction(std::function<void(const std::string&, const sf::Vector2f&, float)> func);

    virtual sf::Vector2f use(float time_factor) = 0;

    virtual float getState() const = 0;

    const sf::Vector2f& getWeaponOffset() const;

    const std::string& getName() const;

protected:
    sf::Vector2f weapon_offset_;


    std::function<void(const std::string&, const sf::Vector2f&, float)> spawning_function_;

private:
    std::string name_;

};


#endif // RAG3_GAME_WEAPONS_ABSTRACTWEAPON_H