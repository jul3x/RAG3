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
        SpecialFunctionsWidget(tgui::Theme* theme) :
            special_(nullptr),
            theme_(theme)
        {
            grid_ = tgui::Grid::create();
            grid_->setSize("90%", "80%");
            grid_->setAutoSize(true);

            auto label = tgui::Label::create();
            label->setRenderer(theme->getRenderer("Label"));
            label->setText("Functions / Parameters:");
            label->setTextSize(CONF<float>("label_text_size"));

            grid_->addWidget(label, 0, 0);

            add_ = tgui::Button::create();
            add_->setRenderer(theme->getRenderer("Button"));
            add_->setText("Add new");
            add_->setTextSize(CONF<float>("label_text_size"));
            add_->setSize(CONF<sf::Vector2f>("button_size"));
            add_->connect("mousepressed", [this]() {
                this->addNewFunction();
            });
            grid_->addWidget(add_, 1, 0);
        }

        void setObjectContent(Special* special)
        {
            this->clearFunctions();

            special_ = special;
            const auto& funcs = special_->getFunctions();
            const auto& datas = special_->getDatas();

            for (int i = 0; i < funcs.size(); ++i)
            {
                std::string func, data;
                j3x::serialize(funcs.at(i), func);
                j3x::serialize(datas.at(i), data);
                this->addNewFunction(func, data);
            }
        }

        [[nodiscard]] tgui::Grid::Ptr getGrid() const
        {
            return grid_;
        }

//        [[nodiscard]] bool isDataFocused() const;
//
//        void addToData(const std::string& str);

        [[nodiscard]] j3x::List getFunctions() const
        {
            std::vector<std::string> out;
            for (const auto& func : functions_)
            {
                out.emplace_back(func.function->getText());
            }

            return j3x::convertToList(out);
        }

        [[nodiscard]] j3x::List getDatas() const
        {
            std::vector<std::string> out;
            for (const auto& func : functions_)
            {
                out.emplace_back(func.data->getText());
            }

            return j3x::convertToList(out);
        }

    private:
        struct Function {
            tgui::Grid::Ptr grid;
            tgui::EditBox::Ptr function, data;
            tgui::Button::Ptr clone, remove;
        };

        void addNewFunction(const std::string& func = "", const std::string& data = "")
        {
            functions_.emplace_back();
            auto index = functions_.size();
            auto& back = functions_.back();
            auto it = functions_.end() - 1;
            back.grid = tgui::Grid::create();
            back.grid->setAutoSize(true);

            back.function = tgui::EditBox::create();
            back.function->setText(func);
            back.function->setRenderer(theme_->getRenderer("EditBox"));
            back.function->setSize("30%", CONF<float>("text_box_height"));
            back.function->setTextSize(CONF<float>("label_text_size"));
            back.grid->addWidget(back.function, 0, 0);

            back.data = tgui::EditBox::create();
            back.data->setText(data);
            back.data->setRenderer(theme_->getRenderer("EditBox"));
            back.data->setSize("50%", CONF<float>("text_box_height"));
            back.data->setTextSize(CONF<float>("label_text_size"));
            back.grid->addWidget(back.data, 0, 1);

            back.remove = tgui::Button::create();
            back.remove->setRenderer(theme_->getRenderer("Button"));
            back.remove->setText("-");
            back.remove->setSize("10%", CONF<float>("text_box_height"));
            back.remove->setTextSize(CONF<float>("label_text_size"));
            back.remove->connect("mousepressed", [this, it]() {
                this->removeFunction(it);
            });
            back.grid->addWidget(back.remove, 0, 2);

            back.clone = tgui::Button::create();
            back.clone->setRenderer(theme_->getRenderer("Button"));
            back.clone->setText("Clone");
            back.remove->setSize("10%", CONF<float>("text_box_height"));

            back.clone->setTextSize(CONF<float>("label_text_size"));
            back.clone->connect("mousepressed", [this, &back]() {
                this->addNewFunction(back.function->getText(), back.data->getText());
            });
            back.grid->addWidget(back.clone, 0, 3);

            grid_->addWidget(back.grid, index + 1, 0);
        }

        void clearFunctions()
        {
            for (auto & function : functions_)
            {
                function.grid->remove(function.function);
                function.grid->remove(function.data);
                function.grid->remove(function.clone);
                function.grid->remove(function.remove);
                grid_->remove(function.grid);
            }

            functions_.clear();
        }

        void removeFunction(std::vector<Function>::iterator it)
        {
            it->grid->remove(it->function);
            it->grid->remove(it->data);
            it->grid->remove(it->clone);
            it->grid->remove(it->remove);
            grid_->remove(it->grid);

            functions_.erase(it);
        }

        tgui::Theme* theme_;

        Special* special_;

        tgui::Grid::Ptr grid_;

        tgui::Button::Ptr add_;

        std::vector<Function> functions_;

    };

} // namespace editor

#endif //RAG3_EDITOR_INCLUDE_EDITOR_SPECIALFUNCTIONSWIDGET_H