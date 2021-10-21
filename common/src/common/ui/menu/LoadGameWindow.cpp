//
// Created by jul3x on 14.08.21.
//

#include <R3E/system/Config.h>

#include <common/Framework.h>

#include <common/ui/menu/LoadGameWindow.h>


LoadGameWindow::LoadGameWindow(tgui::Gui* gui, tgui::Theme* theme, Framework* framework) :
        MenuWindow(gui, theme, "> Load game"),
        framework_(framework)
{
    auto pos = CONF<sf::Vector2f>("graphics/menu_window_pos") +
               sf::Vector2f((CONF<float>("graphics/menu_bar_width_px") + CONF<int>("graphics/window_width_px")) / 2.0f,
                            CONF<float>("graphics/menu_window_header_size") * 2.0f);

    text_ = tgui::Label::create("Select map which you want to play:");
    text_->setRenderer(theme->getRenderer("Text"));
    text_->setInheritedFont(RM.getFont("default"));
    text_->setTextSize(CONF<float>("graphics/menu_window_text_size"));
    text_->setPosition(sf::Vector2f{pos.x - CONF<sf::Vector2f>("graphics/menu_window_size").x / 2.0f, pos.y} +
                       CONF<sf::Vector2f>("graphics/menu_window_header_pos"));

    gui->add(text_);

    scroll_panel_ = tgui::ScrollablePanel::create({CONF<sf::Vector2f>("graphics/menu_window_size").x,
                                                   CONF<sf::Vector2f>("graphics/menu_window_size").y * 0.7f});
    scroll_panel_->setPosition(pos + sf::Vector2f{-CONF<sf::Vector2f>("graphics/menu_window_size").x * 0.5f,
                                                  CONF<float>("graphics/menu_window_header_size") * 2.0f});
    scroll_panel_->setHorizontalScrollbarPolicy(tgui::Scrollbar::Policy::Never);
    scroll_panel_->setVerticalScrollAmount(100);
    scroll_panel_->setRenderer(theme->getRenderer("ScrollablePanelGame"));
    scroll_panel_->getRenderer()->setBackgroundColor(sf::Color::Transparent);
    scroll_panel_->getRenderer()->setScrollbarWidth(CONF<float>("graphics/menu_scrollbar_width"));

    gui->add(scroll_panel_);

    auto grid = tgui::Grid::create();
    grid->setAutoSize(true);

    scroll_panel_->add(grid);

    int i = 0;
    for (const auto& map : CONF<j3x::List>("maps_order"))
    {
        const auto& item = j3x::getObj<std::string>(map);
        static constexpr auto button_opacity = 0.8f;
        auto button = tgui::Picture::create(RM.getTexture("maps/" + item));
        auto label = tgui::Label::create(CONF<std::string>("map_name_" + item));
        button->setInheritedOpacity(button_opacity);
        label->setRenderer(theme->getRenderer("Text"));
        label->setTextSize(CONF<float>("graphics/menu_window_text_size"));
        label->setInheritedFont(RM.getFont("default"));

        button->setSize(CONF<sf::Vector2f>("graphics/menu_map_screen_size"));
        grid->addWidget(button, (i / 4) * 2, i % 4);
        grid->addWidget(label, (i / 4) * 2 + 1, i % 4);
        grid->setWidgetPadding(button,
                               {CONF<float>("graphics/menu_maps_padding"), CONF<float>("graphics/menu_maps_padding")});

        button->connect("mouseentered", [button, this]() {
            button->setInheritedOpacity(1.0f);
            framework_->spawnSound(RM.getSound("ui_hover"));
        });
        button->connect("mouseleft", [button]() {
            button->setInheritedOpacity(button_opacity);
        });
        button->connect("Clicked", [this](const std::string& map_name) {
            framework_->startGame(map_name);
        }, item);

        ++i;
    }

    doShow(false);
}

void LoadGameWindow::doShow(bool show)
{
    MenuWindow::doShow(show);
    scroll_panel_->setVisible(show);
    text_->setVisible(show);
}