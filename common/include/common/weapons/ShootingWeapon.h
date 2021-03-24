//
// Created by jul3x on 16.10.19.
//


#ifndef RAG3_COMMON_INCLUDE_COMMON_SHOOTINGWEAPON_H
#define RAG3_COMMON_INCLUDE_COMMON_SHOOTINGWEAPON_H

#include <cmath>
#include <string>

#include <common/weapons/AbstractWeapon.h>


class ShootingWeapon : public AbstractWeapon {
public:
    explicit ShootingWeapon(Character* user, const std::string& id);

    sf::Vector2f use() override;

    int getAmmunition() const;
    float getState() const override;
    void addAmmo(int ammo) override;
    void setState(float state) override;

    void setFlipY(bool flip) override;

    void recalculate() override;

private:
    // weapon parameters
    std::string spawn_type_;
    int spawn_quantity_;
    short int spawn_offset_factor_;
    float spawn_angular_diff_;
    float spawn_timeout_;
    float spawn_offset_;
    float recoil_;
    int ammunition_, max_ammunition_;

};


#endif //RAG3_COMMON_INCLUDE_COMMON_SHOOTINGWEAPON_H