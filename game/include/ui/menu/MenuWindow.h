//
// Created by jul3x on 14.07.21.
//

#ifndef RAG3_GAME_INCLUDE_UI_MENU_MENUWINDOW_H
#define RAG3_GAME_INCLUDE_UI_MENU_MENUWINDOW_H

#include <TGUI/TGUI.hpp>

#include <R3E/system/Config.h>

#include <common/ResourceManager.h>

using namespace r3e;

class MenuWindow : public sf::Drawable {
public:
    explicit MenuWindow(tgui::Gui* gui, tgui::Theme* theme, const std::string& header);

    virtual void doShow(bool show);

protected:
    bool is_visible_;

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        if (is_visible_)
            target.draw(background_, states);
    }

    sf::RectangleShape background_;
    tgui::Label::Ptr header_;

    tgui::Gui* gui_;
    tgui::Theme* theme_;

};

#endif //RAG3_GAME_INCLUDE_UI_MENU_MENUWINDOW_H
