//
// Created by jul3x on 24.03.2021.
//

#ifndef RAG3_COMMON_INCLUDE_COMMON_UI_USERINTERFACE_H
#define RAG3_COMMON_INCLUDE_COMMON_UI_USERINTERFACE_H

#include <TGUI/TGUI.hpp>

#include <R3E/system/Config.h>
#include <R3E/system/AbstractUserInterface.h>

#include <common/ui/hud/FullHud.h>
#include <common/ui/hud/WeaponsBar.h>
#include <common/ui/hud/HudBar.h>
#include <common/ui/hud/RightHud.h>
#include <common/ui/hud/LeftHud.h>
#include <common/ui/hud/StatsHud.h>
#include <common/ui/hud/SmallBackpackHud.h>
#include <common/ui/Crosshair.h>
#include <common/ui/AcceptWindow.h>
#include <common/ui/Window.h>
#include <common/ui/Achievement.h>
#include <common/ui/Message.h>
#include <common/ui/BloodSplash.h>
#include <common/ui/FadeText.h>
#include <common/ui/menu/Menu.h>
#include <common/misc/Thought.h>
#include <common/misc/BonusText.h>
#include <common/misc/Camera.h>
#include <common/misc/TutorialArrow.h>
#include <common/characters/Player.h>


class Framework;

using namespace r3e;

class UserInterface : public AbstractUserInterface {

public:
    enum class Keys {
        Up,
        Down,
        Left,
        Right,
        Run
    };

    static constexpr std::array<UserInterface::Keys, 5> IMPLEMENTED_KEYS = {
            Keys::Up, Keys::Down, Keys::Left, Keys::Right, Keys::Run
    };

    explicit UserInterface(Framework* framework);

    UserInterface(const UserInterface&) = delete;

    UserInterface& operator=(const UserInterface&) = delete;

    void initialize(graphics::Graphics& graphics) override;
    void update(graphics::Graphics& graphics, float time_elapsed) override;

    virtual void registerPlayer(Player* player);
    virtual void registerCamera(Camera* camera);
    virtual void initializeTutorialArrows();

    virtual void spawnAchievement(const std::string& title, const std::string& text, const std::string& tex);
    virtual void spawnThought(Character* user, const std::string& text);
    virtual void spawnBonusText(const sf::Vector2f& pos, const std::string& text, float time = 0.0f);
    virtual void spawnAcceptWindow(const std::string& text, const std::function<void()>& func);
    virtual void spawnNoteWindow(const std::string& text, bool note_info = true);
    virtual void spawnMessage(const std::string& text);
    virtual void clearThoughts();
    virtual void clearThought(Character* user);
    virtual void closeWindow(Window* window);
    virtual void openMenu();
    virtual void startGame();
    virtual void clearWindows();
    virtual void showFullHud(bool show);

    static void applyMovement(Player* player, const std::unordered_set<UserInterface::Keys>& keys_pressed);

    tgui::Gui* getGui();
    tgui::Theme* getTheme();
    Framework* getFramework();

    void draw(graphics::Graphics& graphics) override;

    const std::unordered_set<UserInterface::Keys>& getKeysPressed();
    bool isLeftMousePressed() const;

    void removeArrowIfExists(AbstractPhysicalObject* obj);

protected:
    virtual void handleEvents(graphics::Graphics& graphics);
    virtual void handleScrolling(float delta);
    virtual void handleKeys();
    virtual void handleMouse(sf::RenderWindow& graphics_window);
    virtual void updatePlayerStates(float time_elapsed);

    virtual bool canZoomIn(bool is_mouse_on_gui) const;
    virtual bool canZoomOut() const;
    virtual void zoomIn(const sf::Vector2f& mouse_world_pos);
    virtual void zoomOut();

    virtual void rollMessagesUp();
    virtual void setTalking(bool is_talking);
    virtual void enterTalking();

    static constexpr float ACHIEVEMENTS_MARGIN_ = 20.0f;

    static constexpr float HEALTH_BAR_X_ = 370.0f;
    static constexpr float HEALTH_BAR_Y_ = 40.0f;

    static constexpr float TIME_BAR_X_ = 141.0f;
    static constexpr float TIME_BAR_Y_ = 102.0f;

    static constexpr float SPEED_BAR_X_ = 141.0f;
    static constexpr float SPEED_BAR_Y_ = 54.0f;

    static constexpr float FPS_X_ = 150.0f;
    static constexpr float FPS_Y_ = 300.0f;

    static constexpr float OBJECT_USE_TEXT_OFFSET_Y_ = 30.0f;
    static constexpr float TALK_TEXT_OFFSET_Y_ = 50.0f;

    std::unique_ptr<tgui::Gui> gui_;
    tgui::Theme theme_;

    // NORMAL HUD - maybe should be refactored
    WeaponsBar weapons_bar_;
    Crosshair crosshair_;
    RightHud right_hud_;
    LeftHud left_hud_;
    HudBar health_bar_;
    HudBar time_bar_;
    HudBar speed_bar_;
    SmallBackpackHud small_backpack_hud_;
    BloodSplash blood_splash_;
    StatsHud stats_hud_;

    // MENUS
    std::unique_ptr<FullHud> full_hud_;
    std::unique_ptr<Menu> menu_;

    // MISC
    std::list<Achievement> achievements_;
    std::list<Thought> thoughts_;
    std::list<BonusText> bonus_texts_;
    std::list<Message> messages_;
    std::unordered_map<AbstractPhysicalObject*, TutorialArrow> tutorial_arrows_;
    std::list<std::shared_ptr<Window>> windows_;
    sf::Text fps_text_;
    FadeText object_use_text_;
    FadeText npc_talk_text_;
    FadeText upgrade_text_;
    Player* player_;
    Camera* camera_;
    Framework* framework_;

    std::unordered_set<UserInterface::Keys> keys_pressed_;
    bool is_left_mouse_pressed_, is_talking_;

};


#endif //RAG3_COMMON_INCLUDE_COMMON_UI_USERINTERFACE_H
