//
// Created by jul3x on 31.03.20.
//

#ifndef RAG3_EDITOR_INCLUDE_EDITOR_OBSTACLEOBJECTWINDOW_H
#define RAG3_EDITOR_INCLUDE_EDITOR_OBSTACLEOBJECTWINDOW_H

#include <common/Obstacle.h>

#include <editor/ChildWindow.h>


using namespace r3e;

namespace editor {

    class ObstacleObjectWindow : public ChildWindow {
    public:
        ObstacleObjectWindow(tgui::Gui* gui, tgui::Theme* theme);

        void setObjectContent(const std::string& category, Obstacle* obstacle);

        [[nodiscard]] bool isDataFocused() const;

        void addToData(const std::string& str);

    private:
        Obstacle* obstacle_;

        tgui::Grid::Ptr grid_;

        tgui::Button::Ptr button_;

        tgui::EditBox::Ptr id_box_, act_box_;
        tgui::TextBox::Ptr fun_box_, data_box_;

    };

} // namespace editor

#endif //RAG3_EDITOR_INCLUDE_EDITOR_OBSTACLEOBJECTWINDOW_H