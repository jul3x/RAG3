//
// Created by jul3x on 01.10.19.
//


#ifndef RAG3_ENGINE_GRAPHICS_ABSTRACTCAMERA_H
#define RAG3_ENGINE_GRAPHICS_ABSTRACTCAMERA_H

#include <SFML/System/Vector3.hpp>
#include <SFML/System/Vector2.hpp>


class AbstractCamera {
public:
    AbstractCamera() = default;

    virtual void update(const sf::Vector2f& position, float time_elapsed) = 0;

    sf::Vector3f& getViewCenter() {
        return center_;
    }

protected:
    sf::Vector3f center_;

    float time_elapsed_;
};

#endif // RAG3_ENGINE_GRAPHICS_ABSTRACTCAMERA_H