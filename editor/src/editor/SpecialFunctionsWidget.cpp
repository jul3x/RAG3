//
// Created by jul3x on 30.10.21.
//

#include <R3E/system/Config.h>

#include <editor/SpecialFunctionsWidget.h>


using namespace editor;

SpecialFunctionsWidget::SpecialFunctionsWidget(tgui::Theme* theme) :
            functional_(nullptr),
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

    id_to_get = 0;
}

std::string SpecialFunctionsWidget::getFunctionsStr() const
{
    j3x::List ret;

    for (const auto& func : functions_)
    {
        ret.emplace_back(std::string(func.function->getText()));
    }

    std::string out;
    j3x::serialize(ret, out);
    return out;
}

std::string SpecialFunctionsWidget::getDatasStr() const
{
    std::string out = "[";
    for (const auto& func : functions_)
    {
        out += func.data->getText() + ";";
    }

    if (!functions_.empty())
        out.back() = ']';
    else
        out += "]";
    
    return out;
}

bool SpecialFunctionsWidget::isDataFocused() const
{
    for (const auto& func : functions_)
    {
        if (func.data->isFocused())
            return true;
    }
    return false;
}

void SpecialFunctionsWidget::addToData(const std::string& data) 
{
    for (const auto& func : functions_)
    {
        if (func.data->isFocused())
        {
            auto pos = func.data->getCaretPosition();
            auto curr_text = func.data->getText();

            auto new_text = curr_text.substring(0, pos) + data + curr_text.substring(pos, curr_text.getSize() - pos);
            func.data->setText(new_text);
            func.data->setCaretPosition(pos + data.length());
        }
    }
}

void SpecialFunctionsWidget::addNewFunction(const std::string& func, const std::string& data)
{
    ++id_to_get;
    functions_.emplace_back();
    
    auto index = functions_.size();
    auto& back = functions_.back();
    back.id = id_to_get;
    back.grid = tgui::Grid::create();
    back.grid->setAutoSize(true);

    back.function = tgui::EditBox::create();
    back.function->setText(func);
    back.function->setRenderer(theme_->getRenderer("EditBox"));
    back.function->setSize(CONF<float>("functions_width"), CONF<float>("text_box_height"));
    back.function->setTextSize(CONF<float>("label_text_size"));
    back.grid->addWidget(back.function, 0, 0);

    back.data = tgui::EditBox::create();
    back.data->setText(data);
    back.data->setRenderer(theme_->getRenderer("EditBox"));
    back.data->setSize(CONF<float>("functions_width"), CONF<float>("text_box_height"));
    back.data->setTextSize(CONF<float>("label_text_size"));
    back.grid->addWidget(back.data, 0, 1);

    back.remove = tgui::Button::create();
    back.remove->setRenderer(theme_->getRenderer("Button"));
    back.remove->setText("-");
    back.remove->setSize(CONF<float>("functions_width") / 3.0f, CONF<float>("text_box_height"));
    back.remove->setTextSize(CONF<float>("label_text_size"));
    back.remove->connect("mousepressed", [this, &back]() {
        this->removeFunction(back.id);
    });
    back.grid->addWidget(back.remove, 0, 2);

    back.clone = tgui::Button::create();
    back.clone->setRenderer(theme_->getRenderer("Button"));
    back.clone->setText("Clone");
    back.remove->setSize(CONF<float>("functions_width") / 3.0f, CONF<float>("text_box_height"));

    back.clone->setTextSize(CONF<float>("label_text_size"));
    back.clone->connect("mousepressed", [this, &back]() {
        this->addNewFunction(back.function->getText(), back.data->getText());
    });
    back.grid->addWidget(back.clone, 0, 3);

    grid_->addWidget(back.grid, index + 1, 0);

    float padding = CONF<float>("items_padding");
    back.grid->setWidgetPadding(0, 0, {padding, 0});
    back.grid->setWidgetPadding(1, 0, {padding, 0});
    back.grid->setWidgetPadding(2, 0, {padding, 0});
    back.grid->setWidgetPadding(3, 0, {padding, 0});
}

tgui::Grid::Ptr SpecialFunctionsWidget::getGrid() const
{
    return grid_;
}

void SpecialFunctionsWidget::setObjectContent(Functional* functional)
{
    this->clearFunctions();

    functional_ = functional;
    const auto& funcs = functional_->getFunctions();
    const auto& datas = functional_->getDatas();

    for (int i = 0; i < funcs.size(); ++i)
    {
        std::string func, data;
        j3x::serialize(funcs.at(i), func);

        // omit " characters
        func = func.substr(1, func.length() - 2);

        j3x::serialize(datas.at(i), data);
        this->addNewFunction(func, data);
    }
}

void SpecialFunctionsWidget::clearFunctions()
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

void SpecialFunctionsWidget::removeFunction(unsigned int id)
{
    utils::eraseIf<Function>(functions_, [this, id](Function& function) {
        if (function.id == id)
        {
            function.grid->remove(function.function);
            function.grid->remove(function.data);
            function.grid->remove(function.clone);
            function.grid->remove(function.remove);
            grid_->remove(function.grid);
            return true;
        }
        return false;
    });
}
