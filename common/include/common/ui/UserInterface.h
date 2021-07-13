//
// Created by jul3x on 24.03.2021.
//

#ifndef RAG3_COMMON_INCLUDE_COMMON_UI_USERINTERFACE_H
#define RAG3_COMMON_INCLUDE_COMMON_UI_USERINTERFACE_H

#include <TGUI/TGUI.hpp>

#include <R3E/system/Config.h>
#include <R3E/system/AbstractUserInterface.h>

#include <common/ui/WeaponsBar.h>
#include <common/ui/HudBar.h>
#include <common/ui/Crosshair.h>
#include <common/ui/RightHud.h>
#include <common/ui/AcceptWindow.h>
#include <common/ui/Window.h>
#include <common/misc/Thought.h>
#include <common/misc/BonusText.h>
#include <common/characters/Player.h>
#include <common/misc/Camera.h>
#include <common/misc/TutorialArrow.h>
#include <common/ui/Achievement.h>
#include <common/ui/SmallBackpackHud.h>


class Framework;

using namespace r3e;

class UserInterface : public AbstractUserInterface {

public:
    explicit UserInterface(Framework* framework);

    UserInterface(const UserInterface&) = delete;

    UserInterface& operator=(const UserInterface&) = delete;

    void initialize(graphics::Graphics& graphics) override;
    void update(graphics::Graphics& graphics, float time_elapsed) override;

    virtual void registerPlayer(Player* player);
    virtual void registerCamera(Camera* camera);

    virtual void spawnAchievement(const std::string& title, const std::string& text, const std::string& tex);
    virtual void spawnThought(Character* user, const std::string& text);
    virtual void spawnBonusText(const sf::Vector2f& pos, const std::string& text);
    virtual void spawnAcceptWindow(const std::string& text, const std::function<void()>& func);
    virtual void spawnNoteWindow(const std::string& text);
    virtual void clearThoughts();
    virtual void closeWindow(Window* window);
    virtual void openMenu();
    virtual void startGame();
    virtual void clearWindows();

    tgui::Gui* getGui();
    tgui::Theme* getTheme();
    Framework* getFramework();

    void draw(graphics::Graphics& graphics) override;

    const std::set<sf::Keyboard::Key>& getKeysPressed();
    bool isLeftMousePressed() const;

    void removeArrowIfExists(AbstractPhysicalObject* obj);

protected:
    virtual void handleTutorialArrows(float time_elapsed);
    virtual void handleEvents(graphics::Graphics& graphics);
    virtual void handleAdditionalKeyPressed(sf::Keyboard::Key code);
    virtual void handleKeyReleased(sf::Keyboard::Key code);
    virtual void handleScrolling(float delta);
    virtual void handleKeys();
    virtual void handleMouse(sf::RenderWindow& graphics_window);
    virtual void updatePlayerStates(float time_elapsed);

    static constexpr float ACHIEVEMENTS_MARGIN_ = 20.0f;

    static constexpr float HEALTH_BAR_X_ = 370.0f;
    static constexpr float HEALTH_BAR_Y_ = 40.0f;

    static constexpr float TIME_BAR_X_ = 141.0f;
    static constexpr float TIME_BAR_Y_ = 102.0f;

    static constexpr float SPEED_BAR_X_ = 141.0f;
    static constexpr float SPEED_BAR_Y_ = 54.0f;

    static constexpr float FPS_X_ = 150.0f;
    static constexpr float FPS_Y_ = 300.0f;

    static constexpr float OBJECT_USE_TEXT_OFFSET_Y_ = 20.0f;
    static constexpr float TALK_TEXT_OFFSET_Y_ = 40.0f;

    std::unique_ptr<tgui::Gui> gui_;
    tgui::Theme theme_;

    // NORMAL HUD - maybe should be refactored
    WeaponsBar weapons_bar_;
    Crosshair crosshair_;
    RightHud right_hud_;
    HudBar health_bar_;
    SmallBackpackHud small_backpack_hud_;

    // MISC
    std::list<Achievement> achievements_;
    std::list<Thought> thoughts_;
    std::list<BonusText> bonus_texts_;
    std::unordered_map<AbstractPhysicalObject*, TutorialArrow> tutorial_arrows_;
    bool tutorial_arrows_initialized_;
    std::list<std::shared_ptr<Window>> windows_;
    sf::Text fps_text_;
    sf::Text object_use_text_;
    sf::Text npc_talk_text_;
    Player* player_;
    Camera* camera_;
    Framework* framework_;

    std::set<sf::Keyboard::Key> keys_pressed_;
    bool is_left_mouse_pressed_;

};


#endif //RAG3_COMMON_INCLUDE_COMMON_UI_USERINTERFACE_H
