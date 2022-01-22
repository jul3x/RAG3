//
// Created by jul3x on 11.01.2022.
//

#ifndef RAG3_GAME_INCLUDE_UI_GAMEFULLHUD_H
#define RAG3_GAME_INCLUDE_UI_GAMEFULLHUD_H

#include <common/ui/hud/FullHud.h>


using namespace r3e;

class UserInterface;

class Framework;


class GameFullHud : public FullHud {
public:
    explicit GameFullHud(UserInterface* ui, Framework* framework, const sf::Vector2f& size);

    void update(float time_elapsed) override;
    void show(bool show) override;
    void bindRespawn(const std::function<void()>& func);

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    Framework* framework_;
    AbstractDrawableObject player_;
    BackpackHud backpack_hud_;
    SkillsHud skills_hud_;

};


#endif //RAG3_GAME_INCLUDE_UI_GAMEFULLHUD_H
