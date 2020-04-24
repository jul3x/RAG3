//
// Created by jul3x on 10.03.20.
//

#include <R3E/system/Config.h>

#include <editor/Camera.h>


using namespace editor;

Camera::Camera() { center_ = {}; zoom_to_ = 1.0f; }

void Camera::update(float time_elapsed)
{
    auto center_diff = pointing_to_ - sf::Vector2f{center_.x, center_.y};

    center_.x += center_diff.x * time_elapsed * CFG.get<float>("camera_speed");
    center_.y += center_diff.y * time_elapsed * CFG.get<float>("camera_speed");

    center_.z = 0.0f;

    auto zoom_diff = zoom_to_ * view_normal_size_ - view_size_;
    view_size_.x += zoom_diff.x * time_elapsed * CFG.get<float>("camera_zoom_speed");
    view_size_.y += zoom_diff.y * time_elapsed * CFG.get<float>("camera_zoom_speed");
}
