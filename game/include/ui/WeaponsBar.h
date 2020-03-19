//
// Created by jul3x on 03.11.19.
//

#ifndef RAG3_GAME_UI_WEAPONSBAR_H
#define RAG3_GAME_UI_WEAPONSBAR_H


#include <list>
#include <vector>
#include <memory>
#include <string>

#include <SFML/Graphics/Text.hpp>

#include <R3E/objects/AbstractDrawableObject.h>

#include <common/AbstractWeapon.h>


using namespace r3e;

class WeaponsBar : public AbstractDrawableObject {
public:
    explicit WeaponsBar(const sf::Vector2f& position);

    void updateWeaponsList(const std::vector<std::shared_ptr<AbstractWeapon>>& weapons);

    void updateCurrentWeapon(int curr_weapon);

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    static constexpr float WEAPON_SIZE_X_ = 70.0f;
    static constexpr float WEAPON_SIZE_Y_ = 70.0f;
    static constexpr int SLOTS_ = 6;

    static constexpr float SIZE_X_ = SLOTS_ * WEAPON_SIZE_X_;
    static constexpr float SIZE_Y_ = WEAPON_SIZE_Y_;

    std::list<AbstractDrawableObject> weapons_;
    std::vector<sf::Text> ammo_;
    AbstractDrawableObject current_highlight_;

};

#endif // RAG3_GAME_UI_WEAPONSBAR_H
