//
// Created by jprolejko on 30.09.19.
//


#ifndef RAG3_OBJECTS_CHARACTER_H
#define RAG3_OBJECTS_CHARACTER_H

#include <memory>

#include <system/Config.h>
#include <objects/AbstractWeapon.h>
#include <objects/Bullet.h>
#include <objects/AbstractPhysicalObject.h>


class Character : public DynamicObject {
public:
    Character(const sf::Vector2f &position,
              const sf::Vector2f &velocity,
              const int max_life);

    bool shot();
    void getShot(const Bullet &bullet);
    void switchWeapon(int relative_position_backpack);

    int getCurrentWeapon() const;
    const std::vector<std::unique_ptr<AbstractWeapon>>& getWeapons() const;
    int getHealth() const;
    int getMaxHealth() const;

    virtual bool update(float time_elapsed);

protected:
    std::vector<std::unique_ptr<AbstractWeapon>> weapons_in_backpack_;
    std::vector<std::unique_ptr<AbstractWeapon>>::iterator current_weapon_;

private:
    void draw(sf::RenderTarget &target, sf::RenderStates states) const;

    static constexpr float SIZE_X_ = 125.0f;
    static constexpr float SIZE_Y_ = 125.0f;

    int max_life_;
    int life_;
};

#endif // RAG3_OBJECTS_CHARACTER_H