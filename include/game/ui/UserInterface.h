//
// Created by jprolejko on 27.02.19.
//

#ifndef RAG3_GAME_UI_USERINTERFACE_H
#define RAG3_GAME_UI_USERINTERFACE_H

#include <engine/system/Config.h>
#include <game/ui/WeaponsBar.h>
#include <game/ui/HealthBar.h>
#include <engine/objects/AbstractDrawableObject.h>

class UserInterface : public AbstractDrawableObject {

public:
    explicit UserInterface();

    UserInterface(const UserInterface&) = delete;
    UserInterface& operator=(const UserInterface&) = delete;

    void initialize();
    void handleEvents();

private:
    virtual void draw(sf::RenderTarget &targer, sf::RenderStates states) const;

    inline void handleScrolling(Player &player, float delta);
    inline void handleKeys(Player &player);
    inline void handleMouse(sf::RenderWindow &graphics_window, Player &player);

    inline void updatePlayerStates(const Player &player);

    static constexpr float WEAPONS_BAR_OFF_Y_ = 70.0f;
    static constexpr float HEALTH_BAR_X_ = 150.0f;
    static constexpr float HEALTH_BAR_Y_ = 50.0f;

    WeaponsBar weapons_bar_;
    HealthBar health_bar_;
};


#endif //RAG3_GAME_UI_USERINTERFACE_H
