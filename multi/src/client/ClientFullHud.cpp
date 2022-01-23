//
// Created by jul3x on 11.01.2022.
//

#include <client/Client.h>
#include <client/ClientFullHud.h>


MultiStats::MultiStats(Client* client) : client_(client)
{
    auto center = sf::Vector2f(CONF<int>("graphics/window_width_px"), CONF<int>("graphics/window_height_px")) / 2.0f;
    grid_ = tgui::Grid::create();
    grid_->setPosition(center + CONF<sf::Vector2f>("graphics/multi_stats_offset"));
    grid_->setAutoSize(false);
    grid_->setVisible(false);
    client_->getUI()->getGui()->add(grid_);
}

void MultiStats::update(const std::string& player_name, const PlayerStats& stats,
                        const std::unordered_map<sf::Uint32, ConnectedPlayer>& other_players)
{
    std::vector<std::tuple<std::string, int, int>> to_sort;
    to_sort.emplace_back(player_name, stats.kills_, stats.deaths_);
    for (const auto& item : other_players)
    {
        to_sort.emplace_back(item.second.player->getName(), item.second.stats.kills_, item.second.stats.deaths_);
    }

    sortPlayersResult(to_sort);

    size_t i = 0;
    auto* theme = client_->getUI()->getTheme();

    grid_->removeAllWidgets();
    grid_->setSize({CONF<float>("graphics/multi_stats_width"),
                    to_sort.size() * CONF<float>("graphics/multi_stats_row_height")});

    size_t j = 0;
    for (const auto& th : {"Player name", "Kills", "Deaths"})
    {
        auto label = tgui::Label::create(th);
        label->setRenderer(theme->getRenderer("StatsHeader"));
        label->setTextSize(CONF<float>("graphics/multi_stats_header_text_size"));
        grid_->addWidget(label, 0, j);
        grid_->setWidgetAlignment(label, tgui::Grid::Alignment::Left);
        ++j;
    }

    for (const auto& item : to_sort)
    {
        ++i;

        j = 0;
        auto kills = utils::toString(std::get<1>(item));
        auto deaths = utils::toString(std::get<2>(item));
        for (const auto& th : {std::get<0>(item), kills, deaths})
        {
            auto label = tgui::Label::create(th);
            label->setRenderer(theme->getRenderer("StatsLabel"));
            label->setTextSize(CONF<float>("graphics/multi_stats_text_size"));
            grid_->addWidget(label, i, j);
            grid_->setWidgetAlignment(label, tgui::Grid::Alignment::Left);
            ++j;
        }
    }
}

void MultiStats::show(bool show)
{
    static const auto show_duration = CONF<float>("graphics/full_hud_show_duration");
    if (show)
        grid_->showWithEffect(tgui::ShowAnimationType::Fade, sf::seconds(show_duration));
    else
        grid_->hideWithEffect(tgui::ShowAnimationType::Fade, sf::seconds(show_duration / 2.0f));
}


ClientFullHud::ClientFullHud(UserInterface* ui, Client* client, const sf::Vector2f& size) :
        FullHud(ui, client, size),
        multi_stats_(client),
        client_(client)
{
    auto button_size = CONF<float>("graphics/menu_button_text_size");
    auto center_top = sf::Vector2f(CONF<int>("graphics/window_width_px") / 2.0f, 0);
    auto show_duration = CONF<float>("graphics/full_hud_show_duration");
    buttons_.emplace_back(std::make_unique<TextButton>(client, "Respawn",
                                                       center_top +
                                                       CONF<sf::Vector2f>("graphics/respawn_button_pos"),
                                                       button_size, show_duration));
    buttons_.back()->bindFunction([this, ui]() {
        if (!client_->canRespawn())
            return;

        ui->spawnAcceptWindow(
            "Are you sure you want to respawn?",
            [this, ui]() {
                if (client_->respawnWithoutReload())
                {
                    this->show(false);
                    ui->clearWindows();
                }
            });
    });
}

void ClientFullHud::update(float time_elapsed)
{
    FullHud::update(time_elapsed);
}

void ClientFullHud::show(bool show)
{
    if (show != show_)
        multi_stats_.show(show);
    FullHud::show(show);

    buttons_.back()->setEnabled(client_->canRespawn());
}

void ClientFullHud::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (show_ || time_elapsed_ > 0.0f)
    {
        target.draw(bg_, states);
    }
}

MultiStats* ClientFullHud::getStats()
{
    return &multi_stats_;
}
