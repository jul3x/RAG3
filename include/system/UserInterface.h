//
// Created by jprolejko on 27.02.19.
//

#ifndef RAG3_SYSTEM_USERINTERFACE_H
#define RAG3_SYSTEM_USERINTERFACE_H

#include <system/Config.h>
#include <objects/WeaponsBar.h>
#include <objects/AbstractDrawableObject.h>

class UserInterface : public AbstractDrawableObject {

public:
    explicit UserInterface();

    UserInterface(const UserInterface&) = delete;
    UserInterface& operator=(const UserInterface&) = delete;

    void handleEvents();

private:
    virtual void draw(sf::RenderTarget &targer, sf::RenderStates states) const;

    inline void handleScrolling(Player &player, float delta);
    inline void handleKeys(Player &player);
    inline void handleMouse(sf::RenderWindow &graphics_window, Player &player);

    WeaponsBar weapons_bar_;
};


#endif //RAG3_SYSTEM_USERINTERFACE_H
