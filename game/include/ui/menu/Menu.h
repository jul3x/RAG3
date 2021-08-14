//
// Created by jul3x on 09.02.21.
//

#ifndef RAG3_GAME_INCLUDE_UI_MENU_MENU_H
#define RAG3_GAME_INCLUDE_UI_MENU_MENU_H

#include <TGUI/TGUI.hpp>

#include <R3E/objects/AbstractPhysicalObject.h>
#include <R3E/objects/StateWithInertia.h>
#include <R3E/utils/Numeric.h>

#include <common/events/Event.h>

#include <ui/menu/MenuWindow.h>


using namespace r3e;

class Framework;

class UserInterface;

class Menu : public AbstractDrawableObject {
public:
    enum class Window {
        None = 0,
        LoadGame = 1,
        Settings = 2,
        About = 3,
    };

    explicit Menu(Framework* framework, UserInterface* ui, tgui::Gui* gui, tgui::Theme* theme);

    void update(float time_elapsed);
    void doShow(bool show);
    void showWindow(Window window);

private:
    static void null()
    {
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    std::vector<std::pair<std::string, std::function<void()>>> elements_;
    std::vector<tgui::Button::Ptr> buttons_;

    std::unordered_map<Menu::Window, std::unique_ptr<MenuWindow>> windows_;

    AbstractDrawableObject bar_;
    DynamicObject logo_;
    std::list<Event> animation_events_;

    Framework* framework_;
    UserInterface* ui_;
    tgui::Gui* gui_;
    tgui::Theme* theme_;

    StateWithInertia<float> opacity_;
    StateWithInertia<float> logo_rotation_;
    float time_elapsed_, explosion_elapsed_;

};

#endif //RAG3_GAME_INCLUDE_UI_MENU_MENU_H
