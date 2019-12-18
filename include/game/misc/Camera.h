//
// Created by jprolejko on 01.10.19.
//


#ifndef RAG3_GAME_MISC_CAMERA_H
#define RAG3_GAME_MISC_CAMERA_H

#include <SFML/System/Vector3.hpp>
#include <SFML/System/Vector2.hpp>


class Camera  {
public:
    Camera();

    void setShaking();

    void update(const sf::Vector2f &position, float time_elapsed);
    sf::Vector3f& getViewCenter();

private:
    enum class State {
        NORMAL,
        SHOOTING
    };

    static constexpr float SHAKING_TIME_ = 0.3f;

    Camera::State state_;

    sf::Vector3f center_;

    float time_elapsed_;
};

#endif // RAG3_GAME_MISC_CAMERA_H