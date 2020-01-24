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
        // 1 |   5   | 3
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

            //std::cout << diff_x << " " << diff_y << std::endl;

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

        return 5;

    }

    inline short int AABB(const StaticObject& a, const StaticObject& b)
    {
        return AABB(a.getPosition() + a.getCollisionArea().getOffset(),
                    {a.getCollisionArea().getA(), a.getCollisionArea().getB()},
                    b.getPosition() + b.getCollisionArea().getOffset(),
                    {b.getCollisionArea().getA(), b.getCollisionArea().getB()});
    }

    inline bool CircleCircle(const sf::Vector2f& a_origin, float a_r,
                             const sf::Vector2f& b_origin, float b_r)
    {
        return (utils::getDistance(a_origin, b_origin) < a_r + b_r);
    }

    inline bool CircleCircle(const StaticObject& a, const StaticObject& b)
    {
        return CircleCircle(a.getPosition() + a.getCollisionArea().getOffset(), a.getCollisionArea().getA(),
                            b.getPosition() + b.getCollisionArea().getOffset(), b.getCollisionArea().getA());
    }

    inline short int ABCircle(const sf::Vector2f& a_origin, const sf::Vector2f& a_size,
                              const sf::Vector2f& b_origin, float b_r)
    {
        //
        // 6  ___2___  7
        //   |       |
        // 1 |   5   | 3
        //   |_______|
        // 9     4     8
        //

        // Get difference vector between both centers
        if (utils::isPointInRectangle(b_origin, a_origin - a_size / 2.0f, a_size))
            return 5;

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

    inline short int ABCircle(const StaticObject& a, const StaticObject& b)
    {
        return ABCircle(a.getPosition() + a.getCollisionArea().getOffset(),
                        {a.getCollisionArea().getA(), a.getCollisionArea().getB()},
                        b.getPosition() + b.getCollisionArea().getOffset(), b.getCollisionArea().getA());
    }

    inline bool CircleABResponse(DynamicObject& a, const StaticObject& b)
    {
        short int direction = ABCircle(b, a);

        float a_r = a.getCollisionArea().getA();
        sf::Vector2f b_size = {b.getCollisionArea().getA(), b.getCollisionArea().getB()};
        sf::Vector2f b1 = b.getPosition() + b.getCollisionArea().getOffset() - b_size / 2.0f;
        sf::Vector2f b2 = b.getPosition() + b.getCollisionArea().getOffset() + b_size / 2.0f;

        if (direction == 1)
        {
            a.setForcedVelocity({0.0f, a.getVelocity().y});
            a.setPosition(b1.x - a.getCollisionArea().getOffset().x - a_r - 1.0f, a.getPosition().y);
        }
        else if (direction == 2)
        {
            a.setForcedVelocity({a.getVelocity().x, 0.0f});
            a.setPosition(a.getPosition().x, b1.y - a.getCollisionArea().getOffset().y - a_r - 1.0f);
        }
        else if (direction == 3)
        {
            a.setForcedVelocity({0.0f, a.getVelocity().y});
            a.setPosition(b2.x - a.getCollisionArea().getOffset().x + a_r + 1.0f, a.getPosition().y);
        }
        else if (direction == 4)
        {
            a.setForcedVelocity({a.getVelocity().x, 0.0f});
            a.setPosition(a.getPosition().x, b2.y - a.getCollisionArea().getOffset().y + a_r + 1.0f);
        }
        else if (direction == 5)
        {
            a.setForcedVelocity({0.0f, 0.0f});
            a.setPosition(a.getPosition() - a.getVelocity());
        }
        else if (direction == 6)
        {
            sf::Vector2f distance = a.getPosition() + a.getCollisionArea().getOffset() - b1;

            sf::Vector2f unit = utils::getNormalized(distance);

            a.setPosition(b1 - a.getCollisionArea().getOffset() +
                          unit * (a_r + 1));
        }
        else if (direction == 7)
        {
            sf::Vector2f distance = a.getPosition() + a.getCollisionArea().getOffset() - sf::Vector2f{b2.x, b1.y};

            sf::Vector2f unit = utils::getNormalized(distance);

            a.setPosition(sf::Vector2f{b2.x, b1.y} - a.getCollisionArea().getOffset() +
                          unit * (a_r + 1));
        }
        else if (direction == 8)
        {
            sf::Vector2f distance = a.getPosition() + a.getCollisionArea().getOffset() - b2;

            sf::Vector2f unit = utils::getNormalized(distance);

            a.setPosition(b2 - a.getCollisionArea().getOffset() +
                          unit * (a_r + 1));
        }
        else if (direction == 9)
        {
            sf::Vector2f distance = a.getPosition() + a.getCollisionArea().getOffset() - sf::Vector2f{b1.x, b2.y};

            sf::Vector2f unit = utils::getNormalized(distance);

            a.setPosition(sf::Vector2f{b1.x, b2.y} - a.getCollisionArea().getOffset() +
                          unit * (a_r + 1));
        }
        else
        {
            return false;
        }

        return false;
    }

    inline bool AABBResponse(DynamicObject& a, const StaticObject& b)
    {
        sf::Vector2f a_size = {a.getCollisionArea().getA(), a.getCollisionArea().getB()};
        sf::Vector2f b_size = {b.getCollisionArea().getA(), b.getCollisionArea().getB()};
        sf::Vector2f a1 = a.getPosition() + a.getCollisionArea().getOffset() + a.getVelocity() - a_size / 2.0f;
        sf::Vector2f a2 = a.getPosition() + a.getCollisionArea().getOffset() + a.getVelocity() + a_size / 2.0f;
        sf::Vector2f b1 = b.getPosition() + b.getCollisionArea().getOffset() - b_size / 2.0f;
        sf::Vector2f b2 = b.getPosition() + b.getCollisionArea().getOffset() + b_size / 2.0f;

        short int direction = AABB(a, b);
        if (direction == 1)
        {
            a.setForcedVelocity({0.0f, a.getVelocity().y});
            a.setPosition(b1.x - a.getCollisionArea().getOffset().x - a_size.x / 2.0f - 1.0f, a.getPosition().y);
        }
        else if (direction == 2)
        {
            a.setForcedVelocity({a.getVelocity().x, 0.0f});
            a.setPosition(a.getPosition().x, b1.y - a.getCollisionArea().getOffset().y - a_size.y / 2.0f - 1.0f);
        }
        else if (direction == 3)
        {
            a.setForcedVelocity({0.0f, a.getVelocity().y});
            a.setPosition(b2.x - a.getCollisionArea().getOffset().x + a_size.x / 2.0f + 1.0f, a.getPosition().y);
        }
        else if (direction == 4)
        {
            a.setForcedVelocity({a.getVelocity().x, 0.0f});
            a.setPosition(a.getPosition().x, b2.y - a.getCollisionArea().getOffset().y + a_size.y / 2.0f + 1.0f);
        }
        else if (direction == 5)
        {
            a.setForcedVelocity({0.0f, 0.0f});
            a.setPosition(a.getPosition() - a.getVelocity());
        }
        else
        {
            return false;
        }

        return true;
    }

    inline bool CircleCircleResponse(DynamicObject& a, const StaticObject& b)
    {
        if (CircleCircle(a, b))
        {
            sf::Vector2f distance = a.getPosition() + a.getCollisionArea().getOffset() - b.getPosition() -
                                    b.getCollisionArea().getOffset();

            sf::Vector2f unit = utils::getNormalized(distance);

            a.setPosition(b.getPosition() + b.getCollisionArea().getOffset() - a.getCollisionArea().getOffset() +
                          unit * (a.getCollisionArea().getA() + b.getCollisionArea().getA() + 1));
            return true;
        }

        return false;
    }

} // namespace utils

#endif // RAG3_ENGINE_UTILS_GEOMETRY_H