//
// Created by jul3x on 03.11.19.
//

#ifndef RAG3_GAME_INCLUDE_UI_WEAPONSBAR_H
#define RAG3_GAME_INCLUDE_UI_WEAPONSBAR_H


#include <list>
#include <vector>
#include <memory>
#include <string>

#include <SFML/Graphics/Text.hpp>

#include <R3E/objects/AbstractDrawableObject.h>
#include <R3E/objects/StateWithInertia.h>

#include <common/AbstractWeapon.h>


using namespace r3e;

class WeaponsBar : public AbstractDrawableObject {
public:
    explicit WeaponsBar(const sf::Vector2f& position);

    void update(const std::vector<std::shared_ptr<AbstractWeapon>>& weapons, int curr_weapon, float time_elapsed);

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    static constexpr float CHOSEN_WEAPON_SIZE_FACTOR_ = 1.25f;
    static constexpr int SLOTS_ = 4;

    std::vector<sf::Vector2f> weapons_positions_;

    std::list<AbstractDrawableObject> weapons_;
    std::vector<sf::Text> ammo_;
    std::vector<StateWithInertia<float>> ammo_quantity_;

};

#endif //RAG3_GAME_INCLUDE_UI_WEAPONSBAR_H
