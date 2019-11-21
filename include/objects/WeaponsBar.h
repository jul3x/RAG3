//
// Created by jprolejko on 03.11.19.
//

#ifndef RAG3_OBJECTS_WEAPONSBAR_H
#define RAG3_OBJECTS_WEAPONSBAR_H


#include <list>
#include <vector>
#include <memory>
#include <string>

#include <SFML/Graphics/Text.hpp>

#include <objects/AbstractWeapon.h>
#include <objects/AbstractDrawableObject.h>


class WeaponsBar : public AbstractDrawableObject {
public:
    WeaponsBar(const sf::Vector2f &position);

    void updateWeaponsList(const std::vector<std::unique_ptr<AbstractWeapon>> &weapons);
    void updateCurrentWeapon(int curr_weapon);

private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    static constexpr float WEAPON_SIZE_X_ = 70.0f;
    static constexpr float WEAPON_SIZE_Y_ = 70.0f;
    static constexpr int SLOTS_ = 6;

    static constexpr float SIZE_X_ = SLOTS_ * WEAPON_SIZE_X_;
    static constexpr float SIZE_Y_ = WEAPON_SIZE_Y_;

    std::list<AbstractDrawableObject> weapons_;
    std::vector<sf::Text> ammo_;
    AbstractDrawableObject current_highlight_;

};

#endif // RAG3_OBJECTS_WEAPONSBAR_H