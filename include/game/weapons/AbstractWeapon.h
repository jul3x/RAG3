//
// Created by jul3x on 16.10.19.
//


#ifndef RAG3_GAME_WEAPONS_ABSTRACTWEAPON_H
#define RAG3_GAME_WEAPONS_ABSTRACTWEAPON_H

#include <string>

#include <engine/objects/AbstractDrawableObject.h>


using namespace r3e;

class AbstractWeapon : public AbstractDrawableObject {
public:
    AbstractWeapon(const sf::Vector2f& size,
                   const sf::Vector2f& weapon_offset,
                   const std::string& name);

    virtual sf::Vector2f use() = 0;

    virtual float getState() const = 0;

    const sf::Vector2f& getWeaponOffset() const;

    const std::string& getName() const;

protected:
    sf::Vector2f weapon_offset_;

private:
    std::string name_;

};


#endif // RAG3_GAME_WEAPONS_ABSTRACTWEAPON_H