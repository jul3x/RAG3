//
// Created by jul3x on 09.02.21.
//

#include <R3E/system/Config.h>

#include <common/ResourceManager.h>

#include <common/Framework.h>
#include <common/ui/UserInterface.h>

#include <common/ui/menu/AboutWindow.h>
#include <common/ui/menu/LoadGameWindow.h>
#include <common/ui/menu/SettingsWindow.h>
#include <common/ui/menu/PickServerWindow.h>
#include <common/ui/menu/Menu.h>


Menu::Menu(Framework* framework, UserInterface* ui, tgui::Gui* gui, tgui::Theme* theme) :
        AbstractDrawableObject(sf::Vector2f{static_cast<float>(CONF<int>("graphics/window_width_px")),
                                            static_cast<float>(CONF<int>("graphics/window_height_px"))} / 2.0f,
                               {static_cast<float>(CONF<int>("graphics/window_width_px")),
                                static_cast<float>(CONF<int>("graphics/window_height_px"))},
                               &RM.getTexture("menu/bg")),
        bar_(sf::Vector2f{CONF<float>("graphics/menu_bar_width_px"),
                          static_cast<float>(CONF<int>("graphics/window_height_px"))} / 2.0f,
             {CONF<float>("graphics/menu_bar_width_px"), static_cast<float>(CONF<int>("graphics/window_height_px"))},
             &RM.getTexture("menu/main_panel")),
        logo_(CONF<sf::Vector2f>("graphics/menu_logo_pos"), sf::Vector2f{},
              CONF<sf::Vector2f>("graphics/menu_logo_size"), collision::None(),
              &RM.getTexture("menu/logo"), 0, 0, 0, 200.0f),
        gui_(gui), theme_(theme), ui_(ui), framework_(framework),
        opacity_(1.0f / CONF<float>("graphics/menu_show_duration")),
        logo_rotation_(1.0f / CONF<float>("graphics/menu_logo_rotation_duration")),
        time_elapsed_(0.0f), explosion_elapsed_(0.0f)
{
    windows_[Window::LoadGame] = std::make_unique<LoadGameWindow>(gui, theme, framework);
    windows_[Window::Settings] = std::make_unique<SettingsWindow>(gui, theme, framework);
    windows_[Window::About] = std::make_unique<AboutWindow>(gui, theme);
    windows_[Window::PickServer] = std::make_unique<PickServerWindow>(gui, theme, framework);

    RM.getTexture("menu/logo").setSmooth(true);
    RM.getTexture("menu/main_panel").setSmooth(true);

    opacity_.setForcedState(0.0f);
    opacity_.setState(0.0f);
    logo_rotation_.setForcedState(0.0f);
    logo_rotation_.setState(0.0f);
}

void Menu::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(shape_, states);
    target.draw(bar_, states);

    for (auto& window : windows_)
    {
        target.draw(*window.second, states);
    }

    for (auto& event : animation_events_)
    {
        target.draw(event, states);
    }

    target.draw(logo_, states);
}

void Menu::update(float time_elapsed)
{
    opacity_.update(time_elapsed);
    logo_rotation_.update(time_elapsed);
    this->setColor(255, 255, 255, static_cast<int>(opacity_.getState() * 255));
    bar_.setColor(255, 255, 255, static_cast<int>(opacity_.getState() * 255));

    utils::eraseIf<Event>(animation_events_, [time_elapsed](Event& a) { return a.update(time_elapsed); });

    if (explosion_elapsed_ >= CONF<float>("graphics/menu_explosion_period"))
    {
        animation_events_.emplace_back(nullptr, logo_.getPosition() + sf::Vector2f{0.0f, -150.0f}, "explosion_1", 0.0f,
                                       RMGET<sf::Vector2f>("animations", "explosion_1", "size").x * 4);
        framework_->registerLight(&animation_events_.back());
        explosion_elapsed_ -= CONF<float>("graphics/menu_explosion_period");

        logo_rotation_.setChangeSpeed(3.0f / CONF<float>("graphics/menu_logo_rotation_duration"));
        logo_rotation_.setState(utils::num::getRandom(-90.0f, 90.0f));
    }
    else if (explosion_elapsed_ > CONF<float>("graphics/menu_explosion_period") / 20.0f)
    {
        logo_rotation_.setChangeSpeed(1.0f / CONF<float>("graphics/menu_logo_rotation_duration"));
        logo_rotation_.setState(0.0f);
    }

    logo_.setPosition(CONF<sf::Vector2f>("graphics/menu_logo_pos") +
                      sf::Vector2f{25.0f * static_cast<float>(std::sin(time_elapsed_ * M_PI / 2.0f)),
                                   12.0f * static_cast<float>(std::sin(time_elapsed_ * M_PI / 2.5f + 0.1))});
    logo_.setColor(255, 255, 255, static_cast<int>(opacity_.getState() * 255));
    logo_.setRotation(logo_rotation_.getState());
    logo_.update(time_elapsed);

    time_elapsed_ += time_elapsed;
    explosion_elapsed_ += time_elapsed;
}

void Menu::doShow(bool show)
{
    if (show)
    {
        explosion_elapsed_ = 0.0f;
        opacity_.setState(1.0f);
    }
    else
    {
        opacity_.setState(0.0f);
        opacity_.setForcedState(0.0f);
    }

    for (auto& button : buttons_)
    {
        if (show)
            button->showWithEffect(tgui::ShowAnimationType::Fade,
                                   sf::seconds(CONF<float>("graphics/menu_show_duration")));
        else
            button->hideWithEffect(tgui::ShowAnimationType::Fade,
                                   sf::seconds(CONF<float>("graphics/menu_show_duration") / 10.0f));
    }
}

void Menu::showWindow(Menu::Window window)
{
    for (auto& w : windows_)
    {
        w.second->doShow(false);
    }

    if (window != Window::None)
        windows_[window]->doShow(true);
}

bool Menu::isOpen(Menu::Window window) const
{
    return windows_.at(window)->isOpen();
}

MenuWindow& Menu::getWindow(Menu::Window window)
{
    return *windows_.at(window);
}

void Menu::makeMenuElements(const std::vector<std::pair<std::string, std::function<void()>>>& elements)
{
    elements_ = elements;
    short int i = 0;
    for (const auto& elem : elements_)
    {
        auto label = tgui::Button::create();
        label->setRenderer(theme_->getRenderer("MenuButton"));
        label->setText(elem.first);
        label->setTextSize(CONF<float>("graphics/menu_button_text_size"));
        label->setPosition(CONF<float>("graphics/menu_button_pos_x"),
                           CONF<float>("graphics/menu_button_pos_y") +
                           i * CONF<float>("graphics/menu_button_offset_y"));

        label->connect("mouseentered", [this, i]() {
            framework_->spawnSound(RM.getSound("ui_hover"));
            this->buttons_[i]->setText("> " + elements_[i].first);
        });
        label->connect("mouseleft", [this, i]() {
            this->buttons_[i]->setText(elements_[i].first);
        });
        label->connect("pressed", [&elem, this]() {
            framework_->spawnSound(RM.getSound("ui_upgrade"));
            elem.second();
        });
        gui_->add(label);
        buttons_.emplace_back(label);
        label->moveToFront();
        ++i;
    }

    std::vector<std::pair<std::string, sf::Vector2f>> additional_texts =
            {{"2022",   sf::Vector2f{0.0f, static_cast<float>(CONF<int>("graphics/window_height_px"))} +
                        CONF<sf::Vector2f>("graphics/menu_year_pos")},
             {"~jul3x", sf::Vector2f{static_cast<float>(CONF<int>("graphics/window_width_px")),
                                     static_cast<float>(CONF<int>("graphics/window_height_px"))} +
                        CONF<sf::Vector2f>("graphics/menu_copyright_pos")}};
    for (const auto& text : additional_texts)
    {
        auto label = tgui::Button::create();
        label->setRenderer(theme_->getRenderer("MenuButton"));
        label->setText(text.first);
        label->setTextSize(CONF<float>("graphics/menu_button_text_size"));
        label->setPosition(text.second);
        label->setFocusable(false);
        label->setEnabled(false);
        gui_->add(label);
        label->moveToBack();
        buttons_.emplace_back(label);
    }
}
