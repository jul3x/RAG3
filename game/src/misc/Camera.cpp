//
// Created by jul3x on 01.10.19.
//

#include <cmath>

#include <R3E/system/Config.h>

#include <misc/Camera.h>


Camera::Camera() : state_(State::NORMAL) { center_ = {}; }

void Camera::setShaking()
{
    state_ = State::SHOOTING;
    time_elapsed_ = CFG.getFloat("graphics/camera_shaking_time");
}

void Camera::update(float time_elapsed)
{
    auto center_diff = pointing_to_ - sf::Vector2f{center_.x, center_.y};

    center_.x += center_diff.x * time_elapsed * CFG.getFloat("graphics/camera_speed");
    center_.y += center_diff.y * time_elapsed * CFG.getFloat("graphics/camera_speed");

    if (state_ == State::SHOOTING)
    {
        center_.z = CFG.getFloat("graphics/camera_rotation_recoil") *
                    std::sin(time_elapsed_ / CFG.getFloat("graphics/camera_shaking_time") * M_PI * 2.0f);
        center_.x += time_elapsed_ * CFG.getFloat("graphics/camera_position_recoil") *
                     std::sin(time_elapsed_ / CFG.getFloat("graphics/camera_shaking_time") * M_PI * 2.0f);
        center_.y += time_elapsed_ * CFG.getFloat("graphics/camera_position_recoil") *
                     std::sin(time_elapsed_ / CFG.getFloat("graphics/camera_shaking_time") * M_PI * 2.0f);
        time_elapsed_ -= time_elapsed;
    }
    else
    {
        center_.z = 0.0f;
    }

    auto zoom_diff = zoom_to_ * view_normal_size_ - view_size_;
    view_size_.x += zoom_diff.x * time_elapsed * CFG.getFloat("graphics/camera_zoom_speed");
    view_size_.y += zoom_diff.y * time_elapsed * CFG.getFloat("graphics/camera_zoom_speed");

    if (time_elapsed_ < 0.0f)
    {
        time_elapsed_ = -1.0f;
        state_ = State::NORMAL;
    }
}
