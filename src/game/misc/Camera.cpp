//
// Created by jul3x on 01.10.19.
//

#include <cmath>

#include <engine/system/Config.h>

#include <game/misc/Camera.h>


Camera::Camera() : state_(State::NORMAL) {}

void Camera::setShaking()
{
    state_ = State::SHOOTING;
    time_elapsed_ = SHAKING_TIME_;
}

void Camera::update(const sf::Vector2f& position, float time_elapsed)
{
    auto diff = position - sf::Vector2f{center_.x, center_.y};

    center_.x += diff.x * time_elapsed * CFG.getFloat("camera_speed");
    center_.y += diff.y * time_elapsed * CFG.getFloat("camera_speed");

    if (state_ == State::SHOOTING)
    {
        center_.z = CFG.getFloat("camera_recoil") * std::sin(time_elapsed_ / SHAKING_TIME_ * M_PI * 2.0f);
        center_.x += CFG.getFloat("camera_recoil") * std::sin(time_elapsed_ / SHAKING_TIME_ * M_PI * 2.0f);
        center_.y += CFG.getFloat("camera_recoil") * std::sin(time_elapsed_ / SHAKING_TIME_ * M_PI * 2.0f);
        time_elapsed_ -= time_elapsed;
    }
    else
    {
        center_.z = 0.0f;
    }

    if (time_elapsed_ < 0.0f)
    {
        time_elapsed_ = -1.0f;
        state_ = State::NORMAL;
    }
}
