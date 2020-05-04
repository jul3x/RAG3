//
// Created by jul3x on 10.03.20.
//


#ifndef RAG3_COMMON_CHARACTER_H
#define RAG3_COMMON_CHARACTER_H

#include <memory>

#include <R3E/objects/AbstractPhysicalObject.h>
#include <R3E/objects/Identifiable.h>
#include <R3E/objects/Unique.h>

#include <common/Functional.h>
#include <common/AbstractWeapon.h>
#include <common/Shootable.h>
#include <common/Special.h>
#include <common/Decoration.h>


using namespace r3e;

class Character : public DynamicObject, public Shootable, public Functional {
public:
    enum class LifeState {
        High,
        Low,
        Critical,
        Dead
    };

    enum class AmmoState {
        High,
        Low,
        Zero
    };

    enum class GlobalState {
        Normal,
        OnFire
    };

    Character(const sf::Vector2f& position,
              const std::string& id,
              int u_id = -1);

    Character(const sf::Vector2f& position, const std::string& id,
              const std::string& activation, const std::vector<std::string>& functions,
              const std::vector<std::string>& datas, int u_id = -1);

    bool shot();

    void getShot(const Bullet& bullet) override;

    void switchWeapon(int relative_position_backpack);

    int getCurrentWeapon() const;

    void makeOnlyOneWeapon(const std::string& id, float state);

    void addWeaponToBackpack(const std::shared_ptr<AbstractWeapon>& ptr);

    void addAmmoToWeapon(const std::string& id);

    const std::vector<std::shared_ptr<AbstractWeapon>>& getWeapons() const;

    void setMaxHealth(float health);

    float getMaxHealth() const;

    LifeState getLifeState() const;

    void setGlobalState(GlobalState state);

    GlobalState getGlobalState() const;

    bool update(float time_elapsed) override;

    void setPosition(const sf::Vector2f& pos) override;

    void setPosition(float x, float y) override;

    void setPositionX(float x) override;

    void setPositionY(float y) override;

    void setRotation(float theta) override;

    float getRotation() const override;

    void setWeaponPointing(const sf::Vector2f& point);

    bool isAlreadyRotated() const;

    void setSpeedFactor(float factor);

    float getSpeedFactor() const;

    void setCurrentSpecialObject(Special* obj);

    Special* getCurrentSpecialObject() const;

protected:
    std::vector<std::shared_ptr<AbstractWeapon>> weapons_in_backpack_;
    std::vector<std::shared_ptr<AbstractWeapon>>::iterator current_weapon_;

    LifeState life_state_;
    AmmoState ammo_state_;
    GlobalState global_state_;

    float max_life_;
    float on_fire_time_;

    std::unique_ptr<Decoration> decorator_;

private:
    inline void handleLifeState();

    inline void handleAmmoState();

    inline void handleGlobalState(float time_elapsed);

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    static constexpr float ROTATING_HYSTERESIS_ = 15.0f;

    sf::Vector2f gun_offset_;
    float rotate_to_;
    float speed_factor_;
    short int current_rotation_quarter_;
    Special* current_special_object_;

};

#endif // RAG3_COMMON_CHARACTER_H
