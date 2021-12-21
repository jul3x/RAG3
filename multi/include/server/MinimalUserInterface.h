//
// Created by jul3x on 16.03.2021.
//

#ifndef RAG3_MULTI_INCLUDE_MINIMALUSERINTERFACE_H
#define RAG3_MULTI_INCLUDE_MINIMALUSERINTERFACE_H

#include <common/ui/UserInterface.h>


class Server;

class MinimalUserInterface : public UserInterface {

public:
    explicit MinimalUserInterface(Server* server);
    void initialize(graphics::Graphics& graphics) override;
    void spawnAchievement(const std::string& title, const std::string& text, const std::string& tex) override;
    void spawnThought(Character* user, const std::string& text) override;
    void spawnBonusText(const sf::Vector2f& pos, const std::string& text) override;
    void spawnAcceptWindow(const std::string& text, const std::function<void()>& func) override;
    void spawnNoteWindow(const std::string& text, bool note_info = true) override;
    void closeWindow(Window* window) override;

    void update(graphics::Graphics& graphics, float time_elapsed) override;
    void draw(graphics::Graphics& graphics) override;

    void openMenu() override;

protected:
    void handleScrolling(float delta) override;
    void handleKeys() override;
    void handleMouse(sf::RenderWindow& graphics_window) override;
    void updatePlayerStates(float time_elapsed) override;

    Server* server_;

//    sf::Text debug_info_;

};

#endif //RAG3_MULTI_INCLUDE_MINIMALUSERINTERFACE_H
