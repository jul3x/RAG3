//
// Created by jul3x on 27.06.19.
//

#ifndef RAG3_ENGINE_INCLUDE_R3E_UTILS_GEOMETRY_H
#define RAG3_ENGINE_INCLUDE_R3E_UTILS_GEOMETRY_H

#include <numeric>
#include <cmath>
#include <stdexcept>
#include <tuple>

#include <SFML/System/Vector2.hpp>

#include <R3E/objects/AbstractPhysicalObject.h>
#include <R3E/ai/DataTypes.h>


namespace r3e::utils::geo {
    float getDistance(const sf::Vector2f& a, const sf::Vector2f& b);

    sf::Vector2f polarToCartesian(float r, float theta_rad);

    std::tuple<float, float> cartesianToPolar(const sf::Vector2f& vector);

    sf::Vector2f vectorLengthLimit(const sf::Vector2f& vector_in, float max_length);

    sf::Vector2f getNormalized(const sf::Vector2f& vector);

    bool isPointInRectangle(const sf::Vector2f& p, const sf::Vector2f& rect_pos, const sf::Vector2f& rect_size);

    short int AABB(const sf::Vector2f& a_origin, const sf::Vector2f& a_size,
                   const sf::Vector2f& b_origin, const sf::Vector2f& b_size);

    bool circleCircle(const sf::Vector2f& a_origin, float a_r,
                      const sf::Vector2f& b_origin, float b_r);

    short int ABCircle(const sf::Vector2f& a_origin, const sf::Vector2f& a_size,
                       const sf::Vector2f& b_origin, float b_r);

    std::tuple<sf::Vector2f, sf::Vector2f, sf::Vector2f> generateCollisionAABB(const StaticObject& a);

    float dotProduct(const sf::Vector2f& a, const sf::Vector2f& b);

    float getAngle(const sf::Vector2f& a, const sf::Vector2f& b);

    float wrapAngle0_360(float angle);

    double wrapAngle0_2PI(double angle);

    float getAngleBetweenDegree(float angle_1, float angle_2);

    float getAngleBetweenRadian(float angle_1, float angle_2);

    sf::Vector2f getNearestForwardPointToPath(const sf::Vector2f& pos, const ai::Path& path);

} // namespace r3e::utils::geo

#endif //RAG3_ENGINE_INCLUDE_R3E_UTILS_GEOMETRY_H