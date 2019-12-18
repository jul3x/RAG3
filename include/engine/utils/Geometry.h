//
// Created by jprolejko on 27.06.19.
//

#ifndef RAG3_ENGINE_UTILS_GEOMETRY_H
#define RAG3_ENGINE_UTILS_GEOMETRY_H

#include <numeric>
#include <cmath>
#include <stdexcept>
#include <tuple>

#include <SFML/System/Vector2.hpp>

#include <engine/objects/AbstractPhysicalObject.h>


namespace utils {

inline float getDistance(const sf::Vector2f &a, const sf::Vector2f &b) {
    return std::hypot(b.x - a.x, b.y - a.y);
}

inline sf::Vector2f polarToCartesian(float r, float theta_rad) {
    if (r < 0.0f)
    {
        throw std::invalid_argument("[Utils::polarToCartesian] Radius cannot be negative!");
    }

    return {r * std::cos(theta_rad), r * std::sin(theta_rad)}; 
}

inline std::tuple<float, float> cartesianToPolar(const sf::Vector2f &vector) {
    float r = std::hypot(vector.x, vector.y);
    float theta_rad = std::atan2(vector.y, vector.x);

    return std::make_tuple(r, theta_rad);
}

inline sf::Vector2f vectorLengthLimit(const sf::Vector2f &vector_in, float max_length) {
    if (max_length < 0.0f)
    {
        throw std::invalid_argument("[utils::vectorLengthLimit] max_length cannot be negative!");
    }

    float length = std::hypot(vector_in.x, vector_in.y);

    sf::Vector2f out = vector_in;

    if (length > max_length) {
        out.x = out.x / length * max_length;
        out.y = out.y / length * max_length;
    }

    return out;
}

inline sf::Vector2f getNormalized(const sf::Vector2f &vector) {
    float length = std::hypot(vector.x, vector.y);

    if (length <= 0.0f)
    {
        throw std::invalid_argument("[utils::getNormalized] Input vector is invalid!");
    }

    return vector / length;
}

inline bool isPointInRectangle(const sf::Vector2f &p, const sf::Vector2f &rect_pos, const sf::Vector2f &rect_size) {
    if (rect_size.x <= 0.0f || rect_size.y <= 0.0f)
    {
        throw std::invalid_argument("[isPointInRectangle] Size of rectangle cannot be negative!");
    }

    return p.x >= rect_pos.x && p.x < rect_pos.x + rect_size.x && p.y >= rect_pos.y && p.y < rect_pos.y + rect_size.y;
}

inline short int AABB(const sf::Vector2f &a_origin, const sf::Vector2f &a_size,
                      const sf::Vector2f &b_origin, const sf::Vector2f &b_size) {
    sf::Vector2f a1 = a_origin - a_size / 2.0f;
    sf::Vector2f a2 = a_origin + a_size / 2.0f;
    sf::Vector2f b1 = b_origin - b_size / 2.0f;
    sf::Vector2f b2 = b_origin + b_size / 2.0f;

    short int direction = 0;
    // 0 - none, 1 - left, 2 - top, 3 - right, 4 - bottom
    if (a1.x > b2.x || a2.x < b1.x || a1.y > b2.y || a2.y < b1.y)
    {
        return direction; // no collision
    }

    auto angle = std::get<1>(utils::cartesianToPolar(b_origin - a_origin));

    if (angle >= M_PI_4 && angle <= M_PI_4 + M_PI_2)
    {
        direction = 2;
    }
    else if (angle <= -M_PI_4 && angle >= - M_PI_4 - M_PI_2)
    {
        direction = 4;
    }
    else if ((angle >= 0.0f && angle < M_PI_4) ||
             (angle < 0.0f && angle > -M_PI_4))
    {
        direction = 1;
    }
    else
    {
        direction = 3;
    }

    return direction;
}

inline bool AABB(DynamicObject &a, StaticObject &b) {
    return AABB(a.getPosition() + a.getVelocity(), a.getSize(), b.getPosition(), b.getSize());
}

inline bool AABB(DynamicObject &a, DynamicObject &b) {
    return AABB(a.getPosition() + a.getVelocity() - b.getVelocity(), a.getSize(), b.getPosition(), b.getSize());
}

inline bool AABBwithDS(DynamicObject &a, const StaticObject &b) {
    sf::Vector2f a1 = a.getPosition() + a.getVelocity() - a.getSize() / 2.0f;
    sf::Vector2f a2 = a.getPosition() + a.getVelocity() + a.getSize() / 2.0f;
    sf::Vector2f b1 = b.getPosition() - b.getSize() / 2.0f;
    sf::Vector2f b2 = b.getPosition() + b.getSize() / 2.0f;

    short int direction = AABB(a.getPosition() + a.getVelocity(), a.getSize(),
                               b.getPosition(), b.getSize());
    if (direction == 2)
    {
        a.setForcedVelocity({a.getVelocity().x, 0.0f});
        a.setPosition(a.getPosition().x, b1.y - a.getSize().y / 2.0f - 1.0f);
    }
    else if (direction == 4)
    {
        a.setForcedVelocity({a.getVelocity().x, 0.0f});
        a.setPosition(a.getPosition().x, b2.y + a.getSize().y / 2.0f + 1.0f);
    }
    else if (direction == 3)
    {
        a.setForcedVelocity({0.0f, a.getVelocity().y});
        a.setPosition(b2.x + a.getSize().x / 2.0f + 1.0f, a.getPosition().y);
    }
    else if (direction == 1)
    {
        a.setForcedVelocity({0.0f, a.getVelocity().y});
        a.setPosition(b1.x - a.getSize().x / 2.0f - 1.0f, a.getPosition().y);
    }
    else
    {
        return false;
    }

    return true;
}

inline bool AABBwithDD(DynamicObject &a, DynamicObject &b) {
    sf::Vector2f a1 = a.getPosition() + a.getVelocity() - b.getVelocity() - a.getSize() / 2.0f;
    sf::Vector2f a2 = a.getPosition() + a.getVelocity() - b.getVelocity() + a.getSize() / 2.0f;
    sf::Vector2f b1 = b.getPosition() - b.getSize() / 2.0f;
    sf::Vector2f b2 = b.getPosition() + b.getSize() / 2.0f;

    short int direction = AABB(a.getPosition() + a.getVelocity() - b.getVelocity(), a.getSize(),
                               b.getPosition(), b.getSize());
    if (direction == 2)
    {
        a.setForcedVelocity({a.getVelocity().x, 0.0f});
        a.setPosition(a.getPosition().x, b1.y - a.getSize().y / 2.0f - 1.0f);
        b.setForcedVelocity({b.getVelocity().x, 0.0f});
        b.setPosition(b.getPosition().x, a2.y + b.getSize().y / 2.0f + 1.0f);
    }
    else if (direction == 4)
    {
        a.setForcedVelocity({a.getVelocity().x, 0.0f});
        a.setPosition(a.getPosition().x, b2.y + a.getSize().y / 2.0f + 1.0f);
        b.setForcedVelocity({b.getVelocity().x, 0.0f});
        b.setPosition(b.getPosition().x, a1.y - b.getSize().y / 2.0f - 1.0f);
    }
    else if (direction == 3)
    {
        a.setForcedVelocity({0.0f, a.getVelocity().y});
        a.setPosition(b2.x + a.getSize().x / 2.0f + 1.0f, a.getPosition().y);
        b.setForcedVelocity({0.0f, b.getVelocity().y});
        b.setPosition(a1.x + b.getSize().x / 2.0f + 1.0f, b.getPosition().y);
    }
    else if (direction == 1)
    {
        a.setForcedVelocity({0.0f, a.getVelocity().y});
        a.setPosition(b1.x - a.getSize().x / 2.0f - 1.0f, a.getPosition().y);
        b.setForcedVelocity({0.0f, b.getVelocity().y});
        b.setPosition(a2.x - b.getSize().x / 2.0f - 1.0f, b.getPosition().y);
    }
    else
    {
        return false;
    }

    return true;
}

} // namespace utils

#endif // RAG3_ENGINE_UTILS_GEOMETRY_H