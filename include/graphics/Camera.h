//
// Created by jprolejko on 01.10.19.
//


#ifndef RAG3_GRAPHICS_CAMERA_H
#define RAG3_GRAPHICS_CAMERA_H

#include <SFML/System/Vector2.hpp>


class Camera  {
public:
    Camera() = default;

    void update(const sf::Vector2f &position, float time_elapsed);
    sf::Vector2f& getViewCenter();

private:
    sf::Vector2f center_;
};

#endif // RAG3_GRAPHICS_CAMERA_H