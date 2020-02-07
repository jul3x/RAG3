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

    bool update(float time_elapsed) override;

    void setPosition(const sf::Vector2f& pos) override;

    void setPosition(float x, float y) override;

    void setPositionX(float x) override;

    void setPositionY(float y) override;

    void setRotation(float theta) override;

protected:
    std::vector<std::unique_ptr<AbstractWeapon>> weapons_in_backpack_;
    std::vector<std::unique_ptr<AbstractWeapon>>::iterator current_weapon_;
    const ai::Path* path_;

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    static constexpr float SIZE_X_ = 30.0f;
    static constexpr float SIZE_Y_ = 30.0f;

    int max_life_;

};

#endif // RAG3_GAME_CHARACTERS_CHARACTER_H
