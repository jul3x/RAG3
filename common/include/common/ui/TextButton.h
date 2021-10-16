//
// Created by jul3x on 26.05.2021.
//

#ifndef RAG3_COMMON_INCLUDE_COMMON_UI_TEXTBUTTON_H
#define RAG3_COMMON_INCLUDE_COMMON_UI_TEXTBUTTON_H

#include <TGUI/TGUI.hpp>


class Framework;

class TextButton {
public:
    explicit TextButton(Framework* framework, const std::string& text,
                        const sf::Vector2f& position, float size, float show_duration);

    void bindFunction(const std::function<void()>& func);

    void show(bool show = true);

private:
    Framework* framework_;
    tgui::Button::Ptr button_;

    float show_duration_;

};

#endif //RAG3_COMMON_INCLUDE_COMMON_UI_TEXTBUTTON_H
