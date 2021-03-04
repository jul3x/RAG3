//
// Created by jul3x on 21.11.19.
//


#ifndef RAG3_COMMON_INCLUDE_COMMON_NOWEAPON_H
#define RAG3_COMMON_INCLUDE_COMMON_NOWEAPON_H

#include <common/AbstractWeapon.h>


class NoWeapon : public AbstractWeapon {
public:
    NoWeapon(Character* user);

    sf::Vector2f use() override;

    float getState() const override;

    void setState(float state) override;

    void recalculate() override;
};

#endif //RAG3_COMMON_INCLUDE_COMMON_NOWEAPON_H