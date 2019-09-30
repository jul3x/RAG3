//
// Created by jprolejko on 27.02.19.
//

#ifndef RAG3_SYSTEM_USERINTERFACE_H
#define RAG3_SYSTEM_USERINTERFACE_H

#include <system/Config.h>


class UserInterface {

public:
    explicit UserInterface();

    UserInterface(const UserInterface&) = delete;
    UserInterface& operator=(const UserInterface&) = delete;

    void handleEvents();

private:
    inline void handleKeys(Player &player);
    inline void handleMouse(sf::RenderWindow &graphics_window, Player &player);
};


#endif //RAG3_SYSTEM_USERINTERFACE_H
