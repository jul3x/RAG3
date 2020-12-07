//
// Created by jul3x on 01.10.19.
//

#include <cmath>

#include <R3E/system/Config.h>
#include <R3E/utils/Utils.h>

#include <misc/Camera.h>



Camera::Camera() : state_(State::Normal) {
    center_ = {};
}

void Camera::setShaking(float factor)
{
    if (state_ == State::Normal || state_ == State::Rag3)
    {
        time_elapsed_ = CONF<float>("graphics/camera_shaking_time");
        shaking_map_[factor] = time_elapsed_;
    }
}

void Camera::setZoomInOut()
{
    state_ = State::ZoomInOut;
    time_elapsed_ = CONF<float>("graphics/camera_zoom_inout_time");
}

void Camera::setNormal()
{
    shaking_map_.clear();
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

    center_.x += center_diff.x * time_elapsed * CONF<float>("graphics/camera_speed");
    center_.y += center_diff.y * time_elapsed * CONF<float>("graphics/camera_speed");

    auto zoom_diff = zoom_to_ * view_normal_size_ - view_size_;
    view_size_.x += zoom_diff.x * time_elapsed * CONF<float>("graphics/camera_zoom_speed");
    view_size_.y += zoom_diff.y * time_elapsed * CONF<float>("graphics/camera_zoom_speed");

    for (auto & it : shaking_map_)
    {
        it.second -= time_elapsed;
    }

    if (!shaking_map_.empty() && (state_ == State::Normal || state_ == State::Rag3))
    {
        auto element = shaking_map_.begin();
        for (auto it = element; it != shaking_map_.end(); ++it)
        {
            if (element->first < it->first)
            {
                element = it;
            }
        }

        center_.z = element->first * CONF<float>("graphics/camera_rotation_recoil") *
                    std::sin(element->first * time_elapsed_ / CONF<float>("graphics/camera_shaking_time") * M_PI * 2.0f);
        center_.x += element->first * time_elapsed * CONF<float>("graphics/camera_position_recoil") *
                     std::cos(element->first * time_elapsed_ / CONF<float>("graphics/camera_shaking_time") * M_PI * 2.0f);
        center_.y -= element->first * time_elapsed * CONF<float>("graphics/camera_position_recoil") *
                     std::cos(element->first * time_elapsed_ / CONF<float>("graphics/camera_shaking_time") * M_PI * 2.0f);

        if (state_ != State::Rag3)
            time_elapsed_ -= time_elapsed;

        if (element->second <= 0.0f)
        {
            shaking_map_.erase(element);
        }
    }
    else if (state_ == State::ZoomInOut)
    {
        center_.z = CONF<float>("graphics/camera_rotation_recoil") *
                    std::sin(time_elapsed_ / CONF<float>("graphics/camera_shaking_time") * M_PI * 2.0f);
        center_.x += time_elapsed * CONF<float>("graphics/camera_position_recoil") *
                     std::sin(time_elapsed_ / CONF<float>("graphics/camera_shaking_time") * M_PI * 2.0f);
        center_.y += time_elapsed * CONF<float>("graphics/camera_position_recoil") *
                     std::sin(time_elapsed_ / CONF<float>("graphics/camera_shaking_time") * M_PI * 2.0f);

        auto normalized_size = utils::geo::getNormalized(view_size_);
        view_size_.x += std::sin(time_elapsed_ / CONF<float>("graphics/camera_zoom_inout_time") * M_PI * 2.0f) *
                        normalized_size.x * time_elapsed * CONF<float>("graphics/camera_zoom_inout_speed");
        view_size_.y += std::sin(time_elapsed_ / CONF<float>("graphics/camera_zoom_inout_time") * M_PI * 2.0f) *
                        normalized_size.y * time_elapsed * CONF<float>("graphics/camera_zoom_inout_speed");

        time_elapsed_ -= time_elapsed;
    }
    else if (state_ == State::Reverse)
    {
        center_.z += 1.0f / (center_.z * center_.z + 1.0f) * time_elapsed *
                     CONF<float>("graphics/camera_reverse_rotation");

        auto normalized_size = utils::geo::getNormalized(view_size_);
        view_size_.x += normalized_size.x * time_elapsed * CONF<float>("graphics/camera_zoom_inout_speed");
        view_size_.y += normalized_size.y * time_elapsed * CONF<float>("graphics/camera_zoom_inout_speed");

        time_elapsed_ += time_elapsed;
    }
    else if (state_ != State::Rag3)
    {
        center_.z -= center_.z * time_elapsed * CONF<float>("graphics/camera_reverse_return_rotation");
    }

    if (state_ == State::Rag3)
    {
        center_.z += 1.0f / (center_.z * center_.z + 1.0f) * time_elapsed *
                     CONF<float>("graphics/camera_rag3_rotation");

        time_elapsed_ += time_elapsed;
    }

    if (time_elapsed_ < 0.0f)
    {
        setNormal();
    }
}

void Camera::setRag3()
{
    time_elapsed_ = 0.1f;
    state_ = State::Rag3;
}
