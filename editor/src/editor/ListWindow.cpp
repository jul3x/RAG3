//
// Created by jul3x on 14.03.20.
//

#include <iostream>

#include <R3E/system/Config.h>

#include <common/ResourceManager.h>

#include <editor/ListWindow.h>
#include <editor/UserInterface.h>
#include <Editor.h>


using namespace editor;

ListWindow::ListWindow(UserInterface* ui, tgui::Gui* gui, tgui::Theme* theme, std::string title, const sf::Vector2f& pos, std::string id) :
        ui_(ui),
        ChildWindow(gui, theme, title, pos, {CFG.getFloat("list_windows_size_x"), CFG.getFloat("list_windows_size_y")}, std::move(id))
{

}

void ListWindow::initialize(const std::vector<std::string>& tabs, const std::vector<std::string>& paths_to_objects)
{
    tabs_ = tgui::Tabs::create();
    tabs_->setRenderer(theme_->getRenderer("Tabs"));
    tabs_->setTabHeight(30);
    tabs_->setPosition(0, 0);

    for (const auto& tab : tabs)
    {
        tabs_->add(tab);
        tab_names_.push_back(tab);
    }

    child_->add(tabs_);
    child_->connect("MouseEntered", [this]() {
        this->ui_->spawnInfo("Click left to pick object or right to edit its configuration.\n");
    });

    auto scroll_panel = tgui::ScrollablePanel::create({"100%", "&.height - 30"});
    scroll_panel->setPosition({"0", "30"});
    scroll_panel->setRenderer(theme_->getRenderer("ScrollablePanel"));

    child_->add(scroll_panel);

    for (const auto &tab : paths_to_objects)
    {
        clickables_.emplace_back();
        grids_.emplace_back();
        grids_.back() = tgui::Grid::create();
        grids_.back()->setAutoSize(true);

        auto tab_name = tab_names_.at(grids_.size() - 1);

        scroll_panel->add(grids_.back(), tab_name);

        const auto& items = RM.getListOfObjects(tab);

        int i = 0;
        for (const auto& item : items)
        {
            auto size = sf::Vector2f(utils::j3x::getFloat(RM.getObjectParams(tab_name, item), "size_x"),
                                     utils::j3x::getFloat(RM.getObjectParams(tab_name, item), "size_y"));

            if (utils::j3x::getInt(RM.getObjectParams(tab_name, item), "frames_number") == 1)
                button_textures_[tab_name + "/" + item].load(RM.getTexture(tab_name + "/" + item));
            else
                button_textures_[tab_name + "/" + item].load(
                        RM.getTexture(tab_name + "/" + item), sf::IntRect({0, 0}, sf::Vector2i(size.x, size.y)));

            auto button = tgui::Picture::create(button_textures_[tab_name + "/" + item]);
            auto label = tgui::Label::create(item);
            label->setRenderer(theme_->getRenderer("ItemLabel"));

            if (size.x > size.y)
            {
                size.y = size.y * CFG.getFloat("items_size") / size.x;
                size.x = CFG.getFloat("items_size");
            }
            else
            {
                size.x = size.x * CFG.getFloat("items_size") / size.y;
                size.y = CFG.getFloat("items_size");
            }

            button->setSize(size.x, size.y);
            grids_.back()->addWidget(button, (i / 4) * 2, i % 4);
            grids_.back()->addWidget(label, (i / 4) * 2 + 1, i % 4);
            clickables_.back().push_back(button);
            grids_.back()->setWidgetPadding(button, {CFG.getFloat("items_padding"), CFG.getFloat("items_padding")});
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

    tabs_->connect("TabSelected", onTabSelected, this, std::ref(*gui_));

    tabs_->select(0);
}