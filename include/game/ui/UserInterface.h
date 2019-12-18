//
// Created by jul3x on 27.02.19.
//

#ifndef RAG3_GAME_UI_USERINTERFACE_H
#define RAG3_GAME_UI_USERINTERFACE_H

#include <engine/system/Config.h>
#include <game/ui/WeaponsBar.h>
#include <game/ui/HealthBar.h>
#include <engine/system/AbstractUserInterface.h>


class UserInterface : public AbstractUserInterface {

public:
    explicit UserInterface();

    UserInterface(const UserInterface&) = delete;

    UserInterface& operator=(const UserInterface&) = delete;

    void initialize() override;

    void handleEvents() override;

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    static inline void handleScrolling(Player& player, float delta);

    static inline void handleKeys(Player& player);

    static inline void handleMouse(sf::RenderWindow& graphics_window, Player& player);

    inline void updatePlayerStates(const Player& player);

    static constexpr float WEAPONS_BAR_OFF_Y_ = 70.0f;
    static constexpr float HEALTH_BAR_X_ = 150.0f;
    static constexpr float HEALTH_BAR_Y_ = 50.0f;

    WeaponsBar weapons_bar_;
    HealthBar health_bar_;
};


#endif //RAG3_GAME_UI_USERINTERFACE_H
