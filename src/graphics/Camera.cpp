//
// Created by jprolejko on 01.10.19.
//

#include <system/Config.h>

#include <graphics/Camera.h>


void Camera::update(const sf::Vector2f &position, float time_elapsed) {
    auto diff = position - center_;

    center_ += diff * time_elapsed * CFG.getFloat("camera_speed");
}

sf::Vector2f& Camera::getViewCenter() {
    return center_;
}