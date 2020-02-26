//
// Created by jul3x on 21.02.20.
//

#include <engine/system/Config.h>

#include <game/misc/ResourceManager.h>
#include <game/ui/BloodSplash.h>


BloodSplash::BloodSplash(const sf::Vector2f& size) :
        dead_(size / 2.0f, size, &RM.getTexture("blood_hud_3")),
        critical_(size / 2.0f, size, &RM.getTexture("blood_hud_2")),
        low_(size / 2.0f, size, &RM.getTexture("blood_hud_1")),
        player_life_state_(Player::LifeState::High),
        transparency_(255),
        time_elapsed_(0)
{
}

void BloodSplash::updateLifeState(Player::LifeState state)
{
    if (state != player_life_state_)
    {
        transparency_ = 255;
        time_elapsed_ = 0.0f;
        dead_.setColor(255, 255, 255, 255);
        low_.setColor(255, 255, 255, 255);
        critical_.setColor(255, 255, 255, 255);
        player_life_state_ = state;
    }
}

void BloodSplash::resizeWindow(const sf::Vector2f& new_size)
{
    dead_.setSize(new_size);
    low_.setSize(new_size);
    critical_.setSize(new_size);
}

void BloodSplash::update(float time_elapsed)
{
    time_elapsed_ += time_elapsed;
    transparency_ = std::abs(255.0f * std::cos(time_elapsed_ / CFG.getFloat("blood_pulsating_time") * M_PI * 2.0f));

    if (time_elapsed_ > CFG.getFloat("blood_pulsating_time")) time_elapsed_ -= CFG.getFloat("blood_pulsating_time");

    critical_.setColor(255, 255, 255, transparency_);
    low_.setColor(255, 255, 255, transparency_);
}


void BloodSplash::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    switch (player_life_state_)
    {
        case Player::LifeState::Low:
        {
            target.draw(low_, states);
            break;
        }
        case Player::LifeState::Critical:
        {
            target.draw(critical_, states);
            break;
        }
        case Player::LifeState::Dead:
        {
            target.draw(dead_, states);
            break;
        }
        default:
        {
            break;
        }
    }
}
