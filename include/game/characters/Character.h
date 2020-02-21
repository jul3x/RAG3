//
// Created by jul3x on 30.09.19.
//


#ifndef RAG3_GAME_CHARACTERS_CHARACTER_H
#define RAG3_GAME_CHARACTERS_CHARACTER_H

#include <memory>

#include <engine/system/Config.h>
#include <engine/objects/AbstractPhysicalObject.h>
#include <engine/ai/AStar.h>

#include <game/weapons/AbstractWeapon.h>
#include <game/weapons/Bullet.h>
#include <game/misc/Shootable.h>


class Character : public DynamicObject, public Shootable {
public:
    enum class LifeState {
        High,
        Low,
        Critical
    };

    enum class AmmoState {
        High,
        Low,
        Zero
    };

    Character(const sf::Vector2f& position,
              const sf::Vector2f& velocity,
              int max_life);

    bool shot();

    void getShot(const Bullet& bullet) override;

    void switchWeapon(int relative_position_backpack);

    int getCurrentWeapon() const;

    const std::vector<std::unique_ptr<AbstractWeapon>>& getWeapons() const;

    int getHealth() const;

    int getMaxHealth() const;

    LifeState getLifeState() const;

    bool update(float time_elapsed) override;

    void setPosition(const sf::Vector2f& pos) override;

    void setPosition(float x, float y) override;

    void setPositionX(float x) override;

    void setPositionY(float y) override;

    void setRotation(float theta) override;

    void setWeaponPointing(const sf::Vector2f& point);

    bool isAlreadyRotated() const;

protected:
    std::vector<std::unique_ptr<AbstractWeapon>> weapons_in_backpack_;
    std::vector<std::unique_ptr<AbstractWeapon>>::iterator current_weapon_;
    const ai::Path* path_;

    LifeState life_state_;
    AmmoState ammo_state_;

    int max_life_;

private:
    inline void handleLifeState();
    inline void handleAmmoState();

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    static constexpr float SIZE_X_ = 100.0f;
    static constexpr float SIZE_Y_ = 100.0f;

    float rotate_to_;

};

#endif // RAG3_GAME_CHARACTERS_CHARACTER_H
