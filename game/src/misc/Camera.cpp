//
// Created by jul3x on 01.10.19.
//

#include <cmath>

#include <R3E/system/Config.h>
#include <R3E/utils/Utils.h>

#include <misc/Camera.h>



Camera::Camera() : state_(State::Normal) { center_ = {}; }

void Camera::setShaking(float factor)
{
    if (state_ == State::Normal || state_ == State::Shooting)
    {
        state_ = State::Shooting;
        time_elapsed_ = CFG.get<float>("graphics/camera_shaking_time");
        factor_ = factor;
    }
}

void Camera::setZoomInOut()
{
    state_ = State::ZoomInOut;
    time_elapsed_ = CFG.get<float>("graphics/camera_zoom_inout_time");
}

void Camera::setNormal()
{
    time_elapsed_ = -1.0f;
    state_ = State::Normal;
}

void Camera::setReverse()
{
    time_elapsed_ = 0.1f;
    state_ = State::Reverse;
}


void Camera::update(float time_elapsed)
{
    auto center_diff = pointing_to_ - sf::Vector2f{center_.x, center_.y};

    center_.x += center_diff.x * time_elapsed * CFG.get<float>("graphics/camera_speed");
    center_.y += center_diff.y * time_elapsed * CFG.get<float>("graphics/camera_speed");

    auto zoom_diff = zoom_to_ * view_normal_size_ - view_size_;
    view_size_.x += zoom_diff.x * time_elapsed * CFG.get<float>("graphics/camera_zoom_speed");
    view_size_.y += zoom_diff.y * time_elapsed * CFG.get<float>("graphics/camera_zoom_speed");

    if (state_ == State::Shooting)
    {
        center_.z = factor_ * CFG.get<float>("graphics/camera_rotation_recoil") *
                    std::sin(factor_ * time_elapsed_ / CFG.get<float>("graphics/camera_shaking_time") * M_PI * 2.0f);
        center_.x += factor_ * time_elapsed_ * CFG.get<float>("graphics/camera_position_recoil") *
                     std::sin(factor_ * time_elapsed_ / CFG.get<float>("graphics/camera_shaking_time") * M_PI * 2.0f);
        center_.y += factor_ * time_elapsed_ * CFG.get<float>("graphics/camera_position_recoil") *
                     std::sin(factor_ * time_elapsed_ / CFG.get<float>("graphics/camera_shaking_time") * M_PI * 2.0f);
        time_elapsed_ -= time_elapsed;
    }
    else if (state_ == State::ZoomInOut)
    {
        center_.z = CFG.get<float>("graphics/camera_rotation_recoil") *
                    std::sin(time_elapsed_ / CFG.get<float>("graphics/camera_shaking_time") * M_PI * 2.0f);
        center_.x += time_elapsed_ * CFG.get<float>("graphics/camera_position_recoil") *
                     std::sin(time_elapsed_ / CFG.get<float>("graphics/camera_shaking_time") * M_PI * 2.0f);
        center_.y += time_elapsed_ * CFG.get<float>("graphics/camera_position_recoil") *
                     std::sin(time_elapsed_ / CFG.get<float>("graphics/camera_shaking_time") * M_PI * 2.0f);

        auto normalized_size = utils::geo::getNormalized(view_size_);
        view_size_.x += std::sin(time_elapsed_ / CFG.get<float>("graphics/camera_zoom_inout_time") * M_PI * 2.0f) *
                        normalized_size.x * time_elapsed * CFG.get<float>("graphics/camera_zoom_inout_speed");
        view_size_.y += std::sin(time_elapsed_ / CFG.get<float>("graphics/camera_zoom_inout_time") * M_PI * 2.0f) *
                        normalized_size.y * time_elapsed * CFG.get<float>("graphics/camera_zoom_inout_speed");

        time_elapsed_ -= time_elapsed;
    }
    else if (state_ == State::Reverse)
    {
        center_.z += 1.0f / (center_.z * center_.z + 1.0f) * time_elapsed *
                     CFG.get<float>("graphics/camera_reverse_rotation");

        auto normalized_size = utils::geo::getNormalized(view_size_);
        view_size_.x += normalized_size.x * time_elapsed * CFG.get<float>("graphics/camera_zoom_inout_speed");
        view_size_.y += normalized_size.y * time_elapsed * CFG.get<float>("graphics/camera_zoom_inout_speed");

        time_elapsed_ += time_elapsed;
    }
    else
    {
        center_.z -= center_.z * time_elapsed * CFG.get<float>("graphics/camera_reverse_return_rotation");
    }

    if (time_elapsed_ < 0.0f)
    {
        setNormal();
    }
}
