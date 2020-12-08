//
// Created by jul3x on 27.02.19.
//

#ifndef RAG3_GAME_INCLUDE_UI_USERINTERFACE_H
#define RAG3_GAME_INCLUDE_UI_USERINTERFACE_H

#include <TGUI/TGUI.hpp>

#include <R3E/system/Config.h>
#include <R3E/system/AbstractUserInterface.h>

#include <ui/WeaponsBar.h>
#include <ui/HealthBar.h>
#include <ui/TimeBar.h>
#include <ui/Crosshair.h>
#include <ui/BloodSplash.h>
#include <ui/LeftHud.h>
#include <ui/RightHud.h>
#include <ui/StatsHud.h>
#include <ui/SmallBackpackHud.h>
#include <ui/LevelHud.h>
#include <ui/FullHud.h>
#include <ui/Achievement.h>
#include <ui/AcceptWindow.h>
#include <ui/Window.h>

#include <misc/Thought.h>
#include <misc/BonusText.h>
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

    void spawnAchievement(const std::string& title, const std::string& text, const std::string& tex);
    void spawnThought(Character* user, const std::string& text);
    void spawnBonusText(const sf::Vector2f& pos, const std::string& text);
    void spawnAcceptWindow(const std::string& text, const std::function<void()>& func);
    void spawnNoteWindow(const std::string& text);
    void closeWindow(Window* window);

    void draw(graphics::Graphics& graphics) override;

private:
    inline void handleScrolling(float delta);
    inline void handleKeys();
    inline void handleMouse(sf::RenderWindow& graphics_window);
    inline void updatePlayerStates(float time_elapsed);

    static constexpr float ACHIEVEMENTS_MARGIN_ = 20.0f;

    static constexpr float HEALTH_BAR_X_ = 163.0f;
    static constexpr float HEALTH_BAR_Y_ = 161.0f;

    static constexpr float TIME_BAR_X_ = 165.0f;
    static constexpr float TIME_BAR_Y_ = 157.0f;

    static constexpr float FPS_X_ = 150.0f;
    static constexpr float FPS_Y_ = 300.0f;

    static constexpr float OBJECT_USE_TEXT_OFFSET_Y_ = 32.0f;

    std::unique_ptr<tgui::Gui> gui_;
    tgui::Theme theme_;

    WeaponsBar weapons_bar_;
    Crosshair crosshair_;
    BloodSplash blood_splash_;
    LeftHud left_hud_;
    RightHud right_hud_;
    HealthBar health_bar_;
    TimeBar time_bar_;
    StatsHud stats_hud_;
    SmallBackpackHud small_backpack_hud_;
    LevelHud level_hud_;

    std::unique_ptr<FullHud> full_hud_;

    std::list<Achievement> achievements_;
    std::list<Thought> thoughts_;
    std::list<BonusText> bonus_texts_;
    std::list<std::shared_ptr<Window>> windows_;

    sf::Text fps_text_;
    sf::Text object_use_text_;
    sf::Text npc_talk_text_;

    Player* player_;
    Camera* camera_;
};


#endif //RAG3_GAME_INCLUDE_UI_USERINTERFACE_H
