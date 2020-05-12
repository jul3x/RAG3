//
// Created by jul3x on 12.05.20.
//


#ifndef RAG3_COMMON_INCLUDE_COMMON_MELEEWEAPON_H
#define RAG3_COMMON_INCLUDE_COMMON_MELEEWEAPON_H

#include <memory>

#include <common/AbstractWeapon.h>
#include <common/MeleeWeaponArea.h>


class MeleeWeapon : public AbstractWeapon {
public:
    explicit MeleeWeapon(Character* user, const std::string& id);

    sf::Vector2f use() override;

    float getState() const override;

    void setState(float state) override;

    MeleeWeaponArea* getMeleeWeaponArea() const;
    float getDeadlyFactor() const;

    void setPosition(const sf::Vector2f& position) override;
    void setPosition(float x, float y) override;
    void setPositionX(float x) override;
    void setPositionY(float y) override;

    void setRotation(float angle) override;

    void update(float time_elapsed) override;

private:
    std::unique_ptr<MeleeWeaponArea> area_;

    float use_elapsed_;
    float use_timeout_;
    float reversed_recoil_;
    float deadly_factor_;

};

#endif //RAG3_COMMON_INCLUDE_COMMON_MELEEWEAPON_H