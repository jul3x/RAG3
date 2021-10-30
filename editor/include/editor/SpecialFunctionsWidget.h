//
// Created by jul3x on 28.10.21.
//

#ifndef RAG3_EDITOR_INCLUDE_EDITOR_SPECIALFUNCTIONSWIDGET_H
#define RAG3_EDITOR_INCLUDE_EDITOR_SPECIALFUNCTIONSWIDGET_H

#include <common/objects/Special.h>

#include <editor/ChildWindow.h>


using namespace r3e;

namespace editor {

    class SpecialFunctionsWidget {
    public:
        SpecialFunctionsWidget(tgui::Theme* theme);

        void setObjectContent(Functional* functional);

        [[nodiscard]] tgui::Grid::Ptr getGrid() const;
  
        [[nodiscard]] bool isDataFocused() const;
  
        void addToData(const std::string& str);
  
        [[nodiscard]] std::string getFunctionsStr() const;

        [[nodiscard]] std::string getDatasStr() const;

    private:
        struct Function {
            unsigned int id;
            tgui::Grid::Ptr grid;
            tgui::EditBox::Ptr function, data;
            tgui::Button::Ptr clone, remove;
        };

        void addNewFunction(const std::string& func = "", const std::string& data = "");
        void clearFunctions();
        void removeFunction(unsigned int id);

        tgui::Theme* theme_;

        Functional* functional_;

        tgui::Grid::Ptr grid_;

        tgui::Button::Ptr add_;

        std::list<Function> functions_;
        unsigned int id_to_get;

    };

} // namespace editor

#endif //RAG3_EDITOR_INCLUDE_EDITOR_SPECIALFUNCTIONSWIDGET_H