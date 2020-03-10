//
// Created by jul3x on 16.10.19.
//


#ifndef RAG3_GAME_WEAPONS_SHOOTINGWEAPON_H
#define RAG3_GAME_WEAPONS_SHOOTINGWEAPON_H

#include <chrono>
#include <cmath>
#include <string>

#include <weapons/AbstractWeapon.h>


class ShootingWeapon : public AbstractWeapon {
public:
    ShootingWeapon(float bullet_timeout,
                   float recoil,
                   int ammunition,
                   const sf::Vector2f& size,
                   const sf::Vector2f& weapon_offset,
                   std::string bullet_type,
                   int bullet_quantity,
                   float bullet_angular_diff,
                   const std::string& texture_name);

    sf::Vector2f use() override;

    int getAmmunition() const;

    float getState() const override;

private:
    static constexpr float BULLET_STARTING_OFFSET_ = 20.0f;
    // weapon parameters
    std::string bullet_type_;
    int bullet_quantity_;
    float bullet_angular_diff_;
    float bullet_timeout_;
    float recoil_;
    int ammunition_, max_ammunition_;

    std::chrono::system_clock::time_point last_bullet_time_;

};


#endif // RAG3_GAME_WEAPONS_SHOOTINGWEAPON_H