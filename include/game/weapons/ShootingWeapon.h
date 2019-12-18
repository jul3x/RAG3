//
// Created by jprolejko on 16.10.19.
//


#ifndef RAG3_GAME_WEAPONS_SHOOTINGWEAPON_H
#define RAG3_GAME_WEAPONS_SHOOTINGWEAPON_H

#include <chrono>
#include <cmath>
#include <string>

#include <game/weapons/AbstractWeapon.h>


class ShootingWeapon : public AbstractWeapon  {
public:
    ShootingWeapon(float bullet_timeout,
                   float recoil,
                   int amunition,
                   const sf::Vector2f &size,
                   const sf::Vector2f &weapon_offset,
                   const std::string &bullet_type,
                   int bullet_quantity,
                   float bullet_angular_diff,
                   const std::string &texture_name);

    virtual sf::Vector2f use();
    int getAmunition() const;

private:
    // weapon parameters
    sf::Vector2f weapon_offset_;
    std::string bullet_type_;
    int bullet_quantity_;
    float bullet_angular_diff_;
    float bullet_timeout_;
    float recoil_;
    int amunition_;

    std::chrono::system_clock::time_point last_bullet_time_;

};


#endif // RAG3_GAME_WEAPONS_SHOOTINGWEAPON_H