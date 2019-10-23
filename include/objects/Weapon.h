//
// Created by jprolejko on 16.10.19.
//


#ifndef RAG3_OBJECTS_WEAPON_H
#define RAG3_OBJECTS_WEAPON_H

#include <chrono>
#include <cmath>
#include <string>

#include <objects/AbstractDrawableObject.h>


class Weapon : public AbstractDrawableObject  {
public:
    Weapon(float bullet_timeout,
           float recoil,
           int amunition,
           const sf::Vector2f &size,
           const sf::Vector2f &weapon_offset,
           const std::string &texture_name);

    sf::Vector2f shot();

private:
    // weapon parameters
    sf::Vector2f weapon_offset_;
    float bullet_timeout_;
    float recoil_;
    int amunition_;

    std::chrono::system_clock::time_point last_bullet_time_;

};


#endif // RAG3_OBJECTS_WEAPON_H