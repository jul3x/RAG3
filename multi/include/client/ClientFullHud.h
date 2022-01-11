//
// Created by jul3x on 11.01.22.
//

#ifndef RAG3_GAME_INCLUDE_UI_CLIENTFULLHUD_H
#define RAG3_GAME_INCLUDE_UI_CLIENTFULLHUD_H

#include <common/ui/hud/FullHud.h>

#include <packets/PlayersStatePacket.h>


using namespace r3e;

class UserInterface;

class Client;


class MultiStats {
public:
    explicit MultiStats(Client* client);

    void update(const std::string& player_name, const PlayerStats& stats,
                const std::unordered_map<sf::Uint32, ConnectedPlayer>& other_players);
    void show(bool show);
private:
    Client* client_;
    tgui::Grid::Ptr grid_;

};


class ClientFullHud : public FullHud {
public:
    explicit ClientFullHud(UserInterface* ui, Client* client, const sf::Vector2f& size);

    void update(float time_elapsed) override;
    void show(bool show) override;

    MultiStats* getStats();

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    Client* client_;
    MultiStats multi_stats_;

};

#endif //RAG3_GAME_INCLUDE_UI_CLIENTFULLHUD_H
