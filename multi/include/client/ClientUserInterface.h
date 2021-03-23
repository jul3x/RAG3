//
// Created by jul3x on 16.03.21.
//

#ifndef RAG3_MULTI_INCLUDE_UI_CLIENTUSERINTERFACE_H
#define RAG3_MULTI_INCLUDE_UI_CLIENTUSERINTERFACE_H

#include <TGUI/TGUI.hpp>

#include <R3E/system/Config.h>
#include <R3E/system/AbstractUserInterface.h>

#include <ui/menu/Menu.h>
#include <ui/hud/WeaponsBar.h>
#include <ui/hud/HealthBar.h>
#include <ui/hud/TimeBar.h>
#include <ui/Crosshair.h>
#include <ui/BloodSplash.h>
#include <ui/hud/LeftHud.h>
#include <ui/hud/RightHud.h>
#include <ui/hud/StatsHud.h>
#include <ui/hud/SmallBackpackHud.h>
#include <ui/hud/LevelHud.h>
#include <ui/hud/FullHud.h>
#include <ui/Achievement.h>
#include <ui/AcceptWindow.h>
#include <ui/Window.h>

#include <misc/Thought.h>
#include <misc/BonusText.h>
#include <characters/Player.h>
#include <misc/Camera.h>


using namespace r3e;

class ClientUserInterface : public AbstractUserInterface {

public:
    explicit ClientUserInterface();

    ClientUserInterface(const ClientUserInterface&) = delete;

    ClientUserInterface& operator=(const ClientUserInterface&) = delete;

    void initialize(graphics::Graphics& graphics) override;
    void handleEvents(graphics::Graphics& graphics, float time_elapsed) override;

    void registerPlayer(Player* player);
    void registerCamera(Camera* camera);

    void draw(graphics::Graphics& graphics) override;

    const std::set<sf::Keyboard::Key>& getKeysPressed();
    bool isLeftMousePressed() const;

private:
    inline void handleScrolling(float delta);
    inline void handleKeys();
    inline void handleMouse(sf::RenderWindow& graphics_window);
    inline void updatePlayerStates(float time_elapsed);

    static constexpr float OBJECT_USE_TEXT_OFFSET_Y_ = 32.0f;

    std::unique_ptr<tgui::Gui> gui_;
    tgui::Theme theme_;

    Crosshair crosshair_;

    // MISC
    sf::Text object_use_text_;
    Player* player_;
    Camera* camera_;

    std::set<sf::Keyboard::Key> keys_pressed_;
    std::set<sf::Mouse::Button> mouse_buttons_pressed_;

};


#endif //RAG3_MULTI_INCLUDE_UI_CLIENTUSERINTERFACE_H
