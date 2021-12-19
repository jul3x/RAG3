//
// Created by jul3x on 14.03.20.
//

#include <iostream>
#include <utility>

#include <R3E/system/Config.h>

#include <common/ResourceManager.h>

#include <editor/ListWindow.h>
#include <editor/UserInterface.h>
#include <Editor.h>


using namespace editor;

ListWindow::ListWindow(UserInterface* ui, tgui::Gui* gui, tgui::Theme* theme, std::string title,
                       const sf::Vector2f& pos, std::string id) :
        ui_(ui),
        ChildWindow(gui, theme, std::move(title), pos, CONF<sf::Vector2f>("list_windows_size"), std::move(id))
{

}

void ListWindow::initialize(const std::vector<std::string>& tabs, const std::vector<std::string>& paths_to_objects)
{
    tabs_ = tgui::Tabs::create();
    tabs_->setRenderer(theme_->getRenderer("Tabs"));
    tabs_->setTabHeight(CONF<float>("tab_height"));
    tabs_->setPosition(0, 0);
    tabs_->setTextSize(CONF<float>("label_text_size"));

    for (const auto& tab : tabs)
    {
        tabs_->add(tab);
        tab_names_.push_back(tab);
    }

    child_->add(tabs_);
    child_->connect("MouseEntered", [this]() {
        this->ui_->spawnInfo("Click left to pick object or right to edit its configuration.\n");
    });

    scroll_panel_ = tgui::ScrollablePanel::create({"100%", "&.height - 30"});
    scroll_panel_->setPosition({"0", CONF<float>("tab_height")});
    scroll_panel_->setHorizontalScrollbarPolicy(tgui::Scrollbar::Policy::Never);
    scroll_panel_->setVerticalScrollAmount(100);
    scroll_panel_->setRenderer(theme_->getRenderer("ScrollablePanel"));

    child_->add(scroll_panel_);

    for (const auto& tab : paths_to_objects)
    {
        clickables_.emplace_back();
        grids_.emplace_back();
        grids_.back() = tgui::Grid::create();
        grids_.back()->setAutoSize(true);

        auto tab_name = tab_names_.at(grids_.size() - 1);

        scroll_panel_->add(grids_.back(), tab_name);

        const auto& items = RM.getListOfObjects(tab);

        int i = 0;
        for (const auto& item : items)
        {
            auto size = RMGET<sf::Vector2f>(tab_name, item, "size", true);

            if (utils::num::isNearlyEqual(size, {}))
            {
                size.x = DecorationTile::SIZE_X_;
                size.y = DecorationTile::SIZE_Y_;
            }

            std::string full_name;
            if (tab_name == "characters")
            {
                full_name = tab_name + "/" + item + "/f";
            }
            else
            {
                full_name =
                        !RMHAS<bool>(tab_name, item, "is_drawable") ||
                        RMGET<bool>(tab_name, item, "is_drawable", false) ?
                        tab_name + "/" + item : "specials/special_event";
            }

            if (RMGET<int>(tab_name, item, "frames_number", true) == 1)
                button_textures_[full_name].load(RM.getTexture(full_name));
            else
                button_textures_[full_name].load(
                        RM.getTexture(full_name), sf::IntRect({0, 0}, sf::Vector2i(size.x, size.y)));

            auto button = tgui::Picture::create(button_textures_[full_name]);
            auto label = tgui::Label::create(item);
            label->setRenderer(theme_->getRenderer("ItemLabel"));
            label->setTextSize(CONF<float>("label_text_size"));

            if (size.x > size.y)
            {
                size.y = size.y * CONF<float>("items_size") / size.x;
                size.x = CONF<float>("items_size");
            }
            else
            {
                size.x = size.x * CONF<float>("items_size") / size.y;
                size.y = CONF<float>("items_size");
            }

            button->setSize(size.x, size.y);
            grids_.back()->addWidget(button, (i / 4) * 2, i % 4);
            grids_.back()->addWidget(label, (i / 4) * 2 + 1, i % 4);
            clickables_.back().push_back(button);
            grids_.back()->setWidgetPadding(button, {CONF<float>("items_padding"), CONF<float>("items_padding")});
            ++i;

            button->connect("Clicked", [&](const std::string& tab_name, const std::string& item_name) {
                Editor::get().setCurrentItem(tab_name, item_name);
                ui_->spawnInfo("Place it using left mouse button or delete existing one using right mouse button.");
            }, tab_name, item);
            button->connect("RightClicked", [&](const std::string& tab_name, const std::string& item_name) {
                ui_->openConfigWindow(tab_name, item_name);
            }, tab_name, item);
        }
    }

    tabs_->connect("TabSelected", [this](const std::string& tab) { this->onTabSelected(*this->gui_, tab); });

    tabs_->select(0);
}