//
// Created by jul3x on 21.02.20.
//

#include <R3E/system/Config.h>

#include <common/ResourceManager.h>
#include <ui/BloodSplash.h>

#include <Game.h>


BloodSplash::BloodSplash(Game* game, const sf::Vector2f& size) :
        game_(game),
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

bool BloodSplash::update(float time_elapsed)
{
    time_elapsed_ += time_elapsed;
    auto period = game_->getRag3Time() > 0.0f ? CONF<float>("rag3_time") / 5.0f :
                  CONF<float>("graphics/blood_pulsating_time");
    transparency_ = std::abs(255.0f * std::sin(time_elapsed_ / period * M_PI * 2.0f));

    if (time_elapsed_ > period)
        time_elapsed_ -= period;

    critical_.setColor(255, 255, 255, transparency_);
    low_.setColor(255, 255, 255, transparency_);

    if (game_->getRag3Time() <= 0.0f && player_life_state_ != Player::LifeState::Critical &&
        player_life_state_ != Player::LifeState::Dead)
        time_elapsed_ = 0.0f;

    return true;
}


void BloodSplash::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (game_->getRag3Time() > 0.0f)
    {
        target.draw(low_, states);
    }
    else
    {
        switch (player_life_state_)
        {
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
}
