//
// Created by jprolejko on 30.09.19.
//


#ifndef RAG3_OBJECTS_PLAYER_H
#define RAG3_OBJECTS_PLAYER_H

#include <chrono>

#include <system/Config.h>
#include <objects/Weapon.h>
#include <objects/AbstractPhysicalObject.h>


class Player : public DynamicObject {
public:
    Player(const sf::Vector2f &position,
           const sf::Vector2f &velocity);

    void shot();
    virtual void update(float time_elapsed);

private:
    void draw(sf::RenderTarget &target, sf::RenderStates states) const;

    static constexpr float SIZE_X_ = 70.0f;
    static constexpr float SIZE_Y_ = 70.0f;
    static constexpr float GUN_OFFSET_X_ = 40.0f;

    Weapon &weapon_;
};

#endif // RAG3_OBJECTS_PLAYER_H
