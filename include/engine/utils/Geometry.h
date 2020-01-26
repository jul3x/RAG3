//
// Created by jul3x on 27.06.19.
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

    inline float getDistance(const sf::Vector2f& a, const sf::Vector2f& b)
    {
        return std::hypot(b.x - a.x, b.y - a.y);
    }

    inline sf::Vector2f polarToCartesian(float r, float theta_rad)
    {
        if (r < 0.0f)
        {
            throw std::invalid_argument("[Utils::polarToCartesian] Radius cannot be negative!");
        }

        return {r * std::cos(theta_rad), r * std::sin(theta_rad)};
    }

    inline std::tuple<float, float> cartesianToPolar(const sf::Vector2f& vector)
    {
        float r = std::hypot(vector.x, vector.y);
        float theta_rad = std::atan2(vector.y, vector.x);

        return std::make_tuple(r, theta_rad);
    }

    inline sf::Vector2f vectorLengthLimit(const sf::Vector2f& vector_in, float max_length)
    {
        if (max_length < 0.0f)
        {
            throw std::invalid_argument("[utils::vectorLengthLimit] max_length cannot be negative!");
        }

        float length = std::hypot(vector_in.x, vector_in.y);

        sf::Vector2f out = vector_in;

        if (length > max_length)
        {
            out.x = out.x / length * max_length;
            out.y = out.y / length * max_length;
        }

        return out;
    }

    inline sf::Vector2f getNormalized(const sf::Vector2f& vector)
    {
        float length = std::hypot(vector.x, vector.y);

        if (length <= 0.0f)
        {
            throw std::invalid_argument("[utils::getNormalized] Input vector is invalid!");
        }

        return vector / length;
    }

    inline bool isPointInRectangle(const sf::Vector2f& p, const sf::Vector2f& rect_pos, const sf::Vector2f& rect_size)
    {
        if (rect_size.x <= 0.0f || rect_size.y <= 0.0f)
        {
            throw std::invalid_argument("[isPointInRectangle] Size of rectangle cannot be negative!");
        }

        return p.x >= rect_pos.x && p.x < rect_pos.x + rect_size.x && p.y >= rect_pos.y &&
               p.y < rect_pos.y + rect_size.y;
    }

    inline short int AABB(const sf::Vector2f& a_origin, const sf::Vector2f& a_size,
                          const sf::Vector2f& b_origin, const sf::Vector2f& b_size)
    {
        //
        // 6  ___2___  7
        //   |       |
        // 1 |       | 3
        //   |_______|
        // 9     4     8
        //

        sf::Vector2f a1 = a_origin - a_size / 2.0f;
        sf::Vector2f a2 = a_origin + a_size / 2.0f;
        sf::Vector2f b1 = b_origin - b_size / 2.0f;
        sf::Vector2f b2 = b_origin + b_size / 2.0f;

        if (a1.x > b2.x || a2.x < b1.x || a1.y > b2.y || a2.y < b1.y)
            return 0; // no collision

        if (a_origin.x < b1.x && a_origin.y >= b1.y && a_origin.y < b2.y)
            return 1;

        if (a_origin.y < b1.y && a_origin.x >= b1.x && a_origin.x < b2.x)
            return 2;

        if (a_origin.x >= b2.x && a_origin.y >= b1.y && a_origin.y < b2.y)
            return 3;

        if (a_origin.y >= b2.y && a_origin.x >= b1.x && a_origin.x < b2.x)
            return 4;

        if (a_origin.x < b1.x && a_origin.y < b1.y)
        {
            float diff_x = std::abs(b1.x - a_origin.x - a_size.x / 2.0f);
            float diff_y = std::abs(b1.y - a_origin.y - a_size.y / 2.0f);

            if (diff_x > diff_y) return 2;
            else return 1;
        }

        if (a_origin.x >= b2.x && a_origin.y < b1.y)
        {
            float diff_x = std::abs(b2.x - a_origin.x + a_size.x / 2.0f);
            float diff_y = std::abs(b1.y - a_origin.y - a_size.y / 2.0f);

            if (diff_x > diff_y) return 2;
            else return 3;
        }

        if (a_origin.x >= b2.x && a_origin.y >= b2.y)
        {
            float diff_x = std::abs(b2.x - a_origin.x + a_size.x / 2.0f);
            float diff_y = std::abs(b2.y - a_origin.y + a_size.y / 2.0f);

            if (diff_x > diff_y) return 4;
            else return 3;
        }

        if (a_origin.x < b1.x && a_origin.y >= b2.y)
        {
            float diff_x = std::abs(b1.x - a_origin.x - a_size.x / 2.0f);
            float diff_y = std::abs(b2.y - a_origin.y + a_size.y / 2.0f);

            if (diff_x > diff_y) return 4;
            else return 1;
        }

        // Origin of second rectangle inside first
        float diff[4];
        diff[0] = std::abs(b_origin.x - a_origin.x + a_size.x / 2.0f);
        diff[2] = std::abs(a_origin.x + a_size.x / 2.0f - b_origin.x);
        diff[3] = std::abs(b_origin.y - a_origin.y + a_size.y / 2.0f);
        diff[1] = std::abs(a_origin.y + a_size.y / 2.0f - b_origin.y);

        short int min_i = 0;
        for (short int i = 0; i < 4; ++i)
        {
            if (diff[i] < diff[min_i])
                min_i = i;
        }

        return min_i + 1;
    }

    inline bool CircleCircle(const sf::Vector2f& a_origin, float a_r,
                             const sf::Vector2f& b_origin, float b_r)
    {
        return (utils::getDistance(a_origin, b_origin) < a_r + b_r);
    }

    inline short int ABCircle(const sf::Vector2f& a_origin, const sf::Vector2f& a_size,
                              const sf::Vector2f& b_origin, float b_r)
    {
        //
        // 6  ___2___  7
        //   |       |
        // 1 |       | 3
        //   |_______|
        // 9     4     8
        //

        // Get difference vector between both centers
        if (utils::isPointInRectangle(b_origin, a_origin - a_size / 2.0f, a_size))
        {
            float diff[4];
            diff[0] = std::abs(b_origin.x - a_origin.x + a_size.x / 2.0f);
            diff[2] = std::abs(a_origin.x + a_size.x / 2.0f - b_origin.x);
            diff[3] = std::abs(b_origin.y - a_origin.y + a_size.y / 2.0f);
            diff[1] = std::abs(a_origin.y + a_size.y / 2.0f - b_origin.y);

            short int min_i = 0;
            for (short int i = 0; i < 4; ++i)
            {
                if (diff[i] < diff[min_i])
                    min_i = i;
            }

            return min_i + 1;
        }

        sf::Vector2f r_o1 = {a_origin.x - a_size.x / 2.0f - b_r / 2.0f, a_origin.y};
        sf::Vector2f r_s1 = {b_r, a_size.y};
        sf::Vector2f r_o2 = {a_origin.x, a_origin.y - a_size.y / 2.0f - b_r / 2.0f};
        sf::Vector2f r_s2 = {a_size.x, b_r};
        sf::Vector2f r_o3 = {a_origin.x + a_size.x / 2.0f + b_r / 2.0f, a_origin.y};
        sf::Vector2f r_s3 = {b_r, a_size.y};
        sf::Vector2f r_o4 = {a_origin.x, a_origin.y + a_size.x / 2.0f + b_r / 2.0f};
        sf::Vector2f r_s4 = {a_size.x, b_r};

        if (utils::isPointInRectangle(b_origin, r_o1 - r_s1 / 2.0f, r_s1))
            return 1;

        if (utils::isPointInRectangle(b_origin, r_o2 - r_s2 / 2.0f, r_s2))
            return 2;

        if (utils::isPointInRectangle(b_origin, r_o3 - r_s3 / 2.0f, r_s3))
            return 3;

        if (utils::isPointInRectangle(b_origin, r_o4 - r_s4 / 2.0f, r_s4))
            return 4;

        sf::Vector2f c_o1 = {a_origin.x - a_size.x / 2.0f, a_origin.y - a_size.y / 2.0f};
        sf::Vector2f c_o2 = {a_origin.x + a_size.x / 2.0f, a_origin.y - a_size.y / 2.0f};
        sf::Vector2f c_o3 = {a_origin.x + a_size.x / 2.0f, a_origin.y + a_size.y / 2.0f};
        sf::Vector2f c_o4 = {a_origin.x - a_size.x / 2.0f, a_origin.y + a_size.y / 2.0f};

        if (utils::getDistance(b_origin, c_o1) < b_r)
            return 6;

        if (utils::getDistance(b_origin, c_o2) < b_r)
            return 7;

        if (utils::getDistance(b_origin, c_o3) < b_r)
            return 8;

        if (utils::getDistance(b_origin, c_o4) < b_r)
            return 9;

        return 0;
    }

    inline std::tuple<sf::Vector2f, sf::Vector2f, sf::Vector2f> generateCollisionAABB(const StaticObject &a)
    {
        std::tuple<sf::Vector2f, sf::Vector2f, sf::Vector2f> out;
        std::get<2>(out) = {a.getCollisionArea().getA(), a.getCollisionArea().getB()};
        std::get<0>(out) = a.getPosition() + a.getCollisionArea().getOffset() - std::get<2>(out) / 2.0f;
        std::get<1>(out) = a.getPosition() + a.getCollisionArea().getOffset() + std::get<2>(out) / 2.0f;

        return out;
    };

    inline std::tuple<sf::Vector2f, sf::Vector2f, sf::Vector2f> generateCollisionAABB(const DynamicObject &a)
    {
        std::tuple<sf::Vector2f, sf::Vector2f, sf::Vector2f> out = generateCollisionAABB(static_cast<const StaticObject&>(a));
        std::get<0>(out) += a.getVelocity();
        std::get<1>(out) += a.getPosition();

        return out;
    };

} // namespace utils

#endif // RAG3_ENGINE_UTILS_GEOMETRY_H