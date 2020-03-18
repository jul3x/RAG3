//
// Created by jul3x on 27.02.19.
//

#ifndef RAG3_GAME_UI_USERINTERFACE_H
#define RAG3_GAME_UI_USERINTERFACE_H

#include <R3E/system/Config.h>
#include <R3E/system/AbstractUserInterface.h>

#include <ui/WeaponsBar.h>
#include <ui/HealthBar.h>
#include <ui/Crosshair.h>
#include <ui/BloodSplash.h>
#include <characters/Player.h>
#include <misc/Camera.h>


using namespace r3e;

class UserInterface : public AbstractUserInterface {

public:
    explicit UserInterface();

    UserInterface(const UserInterface&) = delete;

    UserInterface& operator=(const UserInterface&) = delete;

    void initialize(graphics::Graphics& graphics) override;

    void handleEvents(graphics::Graphics& graphics, float time_elapsed) override;

    void registerPlayer(Player* player);

    void registerCamera(Camera* camera);

    void draw(graphics::Graphics& graphics) override;

private:
    inline void handleScrolling(float delta);

    inline void handleKeys();

    inline void handleMouse(sf::RenderWindow& graphics_window);

    inline void updatePlayerStates();

    static constexpr float WEAPONS_BAR_OFF_Y_ = 70.0f;

    static constexpr float LOGO_OFF_X_ = 240.0f;
    static constexpr float LOGO_OFF_Y_ = 120.0f;
    static constexpr float LOGO_SIZE_X_ = 512.0f;
    static constexpr float LOGO_SIZE_Y_ = 288.0f;

    static constexpr float HEALTH_BAR_X_ = 150.0f;
    static constexpr float HEALTH_BAR_Y_ = 50.0f;

    static constexpr float FPS_X_ = 150.0f;
    static constexpr float FPS_Y_ = 300.0f;

    WeaponsBar weapons_bar_;
    HealthBar health_bar_;
    Crosshair crosshair_;
    BloodSplash blood_splash_;

    AbstractDrawableObject logo_;

    sf::Text fps_text_;

    Player* player_;
    Camera* camera_;
};


#endif //RAG3_GAME_UI_USERINTERFACE_H
