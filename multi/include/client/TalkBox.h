//
// Created by jul3x on 30.01.22.
//

#ifndef RAG3_MULTI_INCLUDE_CLIENT_TALKBOX_H
#define RAG3_MULTI_INCLUDE_CLIENT_TALKBOX_H

#include <TGUI/TGUI.hpp>

#include <R3E/system/Config.h>


using namespace r3e;

class TalkBox {
public:
    TalkBox(tgui::Gui* gui, tgui::Theme* theme, const sf::Vector2f& pos);

    void update(bool show);
    void show(bool show);

    bool isEmpty() const;
    std::string getText() const;

private:
    tgui::EditBox::Ptr talk_box_;
    tgui::Label::Ptr info_talk_box_;
    tgui::Label::Ptr left_;

};


#endif //RAG3_MULTI_INCLUDE_CLIENT_TALKBOX_H
