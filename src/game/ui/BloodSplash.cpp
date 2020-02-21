//
// Created by jul3x on 21.02.20.
//

#include <engine/system/Config.h>

#include <game/misc/ResourceManager.h>
#include <game/ui/BloodSplash.h>


BloodSplash::BloodSplash(const sf::Vector2f& size) :
        high_(size / 2.0f, size, &ResourceManager::getInstance().getTexture("blood_hud_1")),
        low_(size / 2.0f, size, &ResourceManager::getInstance().getTexture("blood_hud_2")),
        critical_(size / 2.0f, size, &ResourceManager::getInstance().getTexture("blood_hud_3")),
        player_life_state_(Player::LifeState::High)
{
}

void BloodSplash::updateLifeState(Player::LifeState state)
{
    player_life_state_ = state;
}

void BloodSplash::resizeWindow(const sf::Vector2f& new_size)
{
    high_.setSize(new_size);
    low_.setSize(new_size);
    critical_.setSize(new_size);
}

void BloodSplash::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    switch (player_life_state_)
    {
        case Player::LifeState::High:
            target.draw(high_, states);
            break;
        case Player::LifeState::Low:
            target.draw(low_, states);
            break;
        case Player::LifeState::Critical:
            target.draw(critical_, states);
            break;
        default:
            std::cerr << "[BloodSplash] Not handled player_life_state!" << std::endl;
    }
}
