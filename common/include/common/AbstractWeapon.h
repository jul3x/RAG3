//
// Created by jul3x on 16.10.19.
//


#ifndef RAG3_COMMON_INCLUDE_COMMON_ABSTRACTWEAPON_H
#define RAG3_COMMON_INCLUDE_COMMON_ABSTRACTWEAPON_H

#include <string>
#include <memory>
#include <list>
#include <functional>

#include <R3E/objects/AbstractDrawableObject.h>
#include <R3E/objects/Identifiable.h>


using namespace r3e;

class Character;

class AbstractWeapon : public AbstractDrawableObject, public Identifiable {
public:
    AbstractWeapon(Character* user,
                   const sf::Vector2f& size,
                   const sf::Vector2f& weapon_offset,
                   const std::string& name);

    void
    registerSpawningFunction(std::function<void(Character*, const std::string&, const sf::Vector2f&, float)> func);

    virtual sf::Vector2f use() = 0;
    virtual float getState() const = 0;
    virtual void setState(float state) = 0;
    virtual void setPosition(const sf::Vector2f& pos, const sf::Vector2f& offset);

    const sf::Vector2f& getWeaponOffset() const;
    Character* getUser() const;
    virtual void update(float time_elapsed);

    void upgrade(const std::string& id);

    static std::shared_ptr<AbstractWeapon> create(Character* user, const std::string& name);

protected:
    Character* user_;

    std::list<std::string> upgrades_;
    sf::Vector2f weapon_offset_;
    float time_elapsed_{0.0f};

    std::function<void(Character*, const std::string&, const sf::Vector2f&, float)> spawning_function_;

};


#endif //RAG3_COMMON_INCLUDE_COMMON_ABSTRACTWEAPON_H