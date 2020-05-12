//
// Created by jul3x on 12.05.20.
//


#ifndef RAG3_COMMON_INCLUDE_COMMON_MELEEWEAPON_H
#define RAG3_COMMON_INCLUDE_COMMON_MELEEWEAPON_H

#include <common/AbstractWeapon.h>


class MeleeWeapon : public AbstractWeapon {
public:
    explicit MeleeWeapon(const std::string& id);

    sf::Vector2f use() override;

    float getState() const override;

    void setState(float state) override;

private:
    float enabled_time_;
    float use_timeout_;
    float reversed_recoil_;

};

#endif //RAG3_COMMON_INCLUDE_COMMON_MELEEWEAPON_H