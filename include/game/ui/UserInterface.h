//
// Created by jul3x on 27.02.19.
//

#ifndef RAG3_GAME_UI_USERINTERFACE_H
#define RAG3_GAME_UI_USERINTERFACE_H

#include <engine/system/Config.h>
#include <game/ui/WeaponsBar.h>
#include <game/ui/HealthBar.h>
#include <game/characters/Player.h>
#include <game/misc/Camera.h>
#include <engine/system/AbstractUserInterface.h>


class UserInterface : public AbstractUserInterface {

public:
    explicit UserInterface();

    UserInterface(const UserInterface&) = delete;

    UserInterface& operator=(const UserInterface&) = delete;

    void initialize() override;

    void handleEvents(Graphics &graphics) override;

    void registerPlayer(Player* player);

    void registerCamera(Camera* camera);

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    inline void handleScrolling(float delta);

    inline void handleKeys();

    inline void handleMouse(sf::RenderWindow& graphics_window);

    inline void updatePlayerStates();

    static constexpr float WEAPONS_BAR_OFF_Y_ = 70.0f;
    static constexpr float HEALTH_BAR_X_ = 150.0f;
    static constexpr float HEALTH_BAR_Y_ = 50.0f;

    WeaponsBar weapons_bar_;
    HealthBar health_bar_;
    Player* player_;
    Camera* camera_;
};


#endif //RAG3_GAME_UI_USERINTERFACE_H
