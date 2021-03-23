//
// Created by jul3x on 10.03.20.
//


#ifndef RAG3_COMMON_INCLUDE_COMMON_CHARACTER_H
#define RAG3_COMMON_INCLUDE_COMMON_CHARACTER_H

#include <memory>
#include <list>

#include <R3E/objects/AbstractPhysicalObject.h>
#include <R3E/objects/Identifiable.h>
#include <R3E/objects/Unique.h>
#include <R3E/objects/Lightable.h>
#include <R3E/graphics/LightPoint.h>
#include <R3E/graphics/StaticShadow.h>

#include <common/Functional.h>
#include <common/AbstractWeapon.h>
#include <common/Shootable.h>
#include <common/Special.h>
#include <common/Decoration.h>
#include <common/TalkableArea.h>
#include <common/MeleeWeapon.h>


using namespace r3e;

class Character : public DynamicObject, public Shootable, public Lightable, public Functional {
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
              const std::string& activation, const j3x::List& functions,
              const j3x::List& datas, int u_id = -1);

    // Weapon manipulation
    bool shot();
    void switchWeapon(int relative_position_backpack);
    void setCurrentWeapon(int number);
    int getCurrentWeapon() const;
    void makeOnlyOneWeapon(const std::string& id, float state);
    bool addWeaponToBackpack(const std::shared_ptr<AbstractWeapon>& ptr);
    void addAmmoToWeapon(const std::string& id);

    // Getters
    const std::vector<std::shared_ptr<AbstractWeapon>>& getWeapons() const;
    float getMaxHealth() const;
    LifeState getLifeState() const;
    GlobalState getGlobalState() const;
    float getRotation() const override;
    float getSpeedFactor() const;
    Special* getCurrentSpecialObject() const;
    Character* getCurrentTalkableCharacter() const;
    bool isAlreadyRotated() const;
    bool isTalkable() const;
    TalkableArea* getTalkableArea() const;
    const std::string& getTalkScenarioStr() const;
    const j3x::List& getTalkScenario() const;
    graphics::StaticShadow* getShadow() const;
    float getRotateTo() const;
    const sf::Vector2f& getGunOffset() const;

    // Setters
    void setMaxHealth(float health);
    void setGlobalState(GlobalState state);
    void setPosition(const sf::Vector2f& pos) override;
    void setPosition(float x, float y) override;
    void setPositionX(float x) override;
    void setPositionY(float y) override;
    void setRotation(float theta) override;
    void setRotateTo(float theta);
    void setWeaponPointing(const sf::Vector2f& point);
    void setSpeedFactor(float factor, float time);
    void setCurrentSpecialObject(Special* obj);
    void setCurrentFrame(short int frame) override;
    void setCurrentTalkableCharacter(Character* obj);
    void setTalkScenarioStr(const std::string& str);
    void setTalkScenario(const j3x::List& str);
    void changeTexture(sf::Texture* texture, bool reset = false) override;

    bool updateAnimation(float time_elapsed, float animation_speed_factor = 1.0f) override;
    bool update(float time_elapsed) override;
    void getShot(const Bullet& bullet, float factor = 1.0f) override;
    virtual void getCut(const MeleeWeapon& weapon, float factor = 1.0f);
    bool talk(const std::function<void(Character*, const std::string&)>& talking_func, Character* character);

protected:
    void handleLifeState();
    void handleAmmoState();
    virtual void handleGlobalState(float time_elapsed);

    std::vector<std::shared_ptr<AbstractWeapon>> weapons_in_backpack_;
    size_t current_weapon_;

    LifeState life_state_;
    AmmoState ammo_state_;
    GlobalState global_state_;

    float max_life_;
    float on_fire_time_;
    short int current_rotation_quarter_;

    std::unique_ptr<graphics::TransformedTextureShadow> static_shadow_;
    std::unique_ptr<Decoration> decorator_;
    std::unique_ptr<TalkableArea> talkable_area_;
    std::function<void(Character*, const std::string&)> talking_func_;
    bool is_talkable_;
    bool is_moving_;
    bool should_respond_;

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    sf::Vector2f gun_offset_;
    float rotate_to_;

    Special* current_special_object_;
    Character* current_talkable_character_;
    j3x::List talk_scenario_;
    size_t talk_moment_;

    float talking_time_elapsed_;

    std::pair<float, float> speed_factor_with_time_;

};

#endif //RAG3_COMMON_INCLUDE_COMMON_CHARACTER_H
