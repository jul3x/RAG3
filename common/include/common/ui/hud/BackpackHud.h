//
// Created by jul3x on 16.10.21.
//

#ifndef RAG3_GAME_INCLUDE_UI_BACKPACKHUD_H
#define RAG3_GAME_INCLUDE_UI_BACKPACKHUD_H

#include <SFML/Graphics/Text.hpp>
#include <TGUI/TGUI.hpp>

#include <R3E/objects/AbstractDrawableObject.h>
#include <R3E/graphics/LineEffect.h>

#include <common/characters/Player.h>
#include <common/ui/Tooltip.h>
#include <common/ui/TextButton.h>


using namespace r3e;

class UserInterface;

class Framework;

class BackpackHud : public sf::Drawable {
public:
    explicit BackpackHud(UserInterface* ui, Framework* framework, const sf::Vector2f& pos, int x, int y);

    void update(float time_elapsed);
    void show(bool show = true);
    void setOpacity(sf::Uint8 a);

    void clickPlaceholder(Tooltip& tooltip);
    const std::vector<size_t>& getActiveTooltips();
    void resetActiveTooltips();
    void combineBackpackItems(size_t first, size_t second);
private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    std::vector<AbstractDrawableObject> placeholders_;
    std::list<std::pair<AbstractDrawableObject, std::string>> weapons_;
    std::vector<Tooltip> tooltips_;
    std::vector<sf::Text> numbers_;

    UserInterface* ui_;
    Framework* framework_;
};

#endif //RAG3_GAME_INCLUDE_UI_BACKPACKHUD_H
