//
// Created by jul3x on 08.05.20.
//


#ifndef RAG3_COMMON_INCLUDE_COMMON_PLACEDWEAPON_H
#define RAG3_COMMON_INCLUDE_COMMON_PLACEDWEAPON_H

#include <cmath>
#include <string>

#include <R3E/objects/Unique.h>

#include <common/ShootingWeapon.h>


class PlacedWeapon : public ShootingWeapon, public Unique {
public:
    PlacedWeapon(const sf::Vector2f& position, float direction, const std::string& id, int u_id = -1);

    PlacedWeapon(const sf::Vector2f& position, float direction, const std::string& id, const std::string& usage,
                 float data, int u_id = -1);

    float getData() const;
    const std::string& getUsageStr() const;

    void update(float time_elapsed) override;

private:
    void constantUse(float time_elapsed);

    std::function<void(float)> usage_func_;
    float data_;
    std::string usage_str_;

};


#endif //RAG3_COMMON_INCLUDE_COMMON_PLACEDWEAPON_H