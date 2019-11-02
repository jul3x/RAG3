//
// Created by jprolejko on 30.09.19.
//


#ifndef RAG3_OBJECTS_PLAYER_H
#define RAG3_OBJECTS_PLAYER_H

#include <chrono>

#include <system/Config.h>
#include <objects/Weapon.h>
#include <objects/Bullet.h>
#include <objects/AbstractPhysicalObject.h>


class Player : public DynamicObject {
public:
    Player(const sf::Vector2f &position,
           const sf::Vector2f &velocity);

    void initialize();

    bool shot();
    void getShot(const Bullet &bullet);
    void switchWeapon(int relative_position_backpack);
    virtual bool update(float time_elapsed);

protected:
    std::vector<Weapon> weapons_in_backpack_;
    std::vector<Weapon>::iterator current_weapon_;

private:
    void draw(sf::RenderTarget &target, sf::RenderStates states) const;

    static constexpr float SIZE_X_ = 125.0f;
    static constexpr float SIZE_Y_ = 125.0f;

    int life_;
};

#endif // RAG3_OBJECTS_PLAYER_H
