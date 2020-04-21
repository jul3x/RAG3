//
// Created by jul3x on 21.04.20.
//

#include <r3e/utils/Geometry.h>


namespace r3e {
    namespace utils {
        namespace geo {
            float getDistance(const sf::Vector2f& a, const sf::Vector2f& b)
            {
                return std::hypot(b.x - a.x, b.y - a.y);
            }

            sf::Vector2f polarToCartesian(float r, float theta_rad)
            {
                if (r < 0.0f)
                {
                    throw std::invalid_argument("[Utils::polarToCartesian] Radius cannot be negative!");
                }

                return {r * std::cos(theta_rad), r * std::sin(theta_rad)};
            }

            std::tuple<float, float> cartesianToPolar(const sf::Vector2f& vector)
            {
                float r = std::hypot(vector.x, vector.y);
                float theta_rad = std::atan2(vector.y, vector.x);

                return std::make_tuple(r, theta_rad);
            }

            sf::Vector2f vectorLengthLimit(const sf::Vector2f& vector_in, float max_length)
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

            sf::Vector2f getNormalized(const sf::Vector2f& vector)
            {
                float length = std::hypot(vector.x, vector.y);
                return vector / length;
            }

            bool
            isPointInRectangle(const sf::Vector2f& p, const sf::Vector2f& rect_pos, const sf::Vector2f& rect_size)
            {
                if (rect_size.x <= 0.0f || rect_size.y <= 0.0f)
                {
                    throw std::invalid_argument("[isPointInRectangle] Size of rectangle cannot be negative!");
                }

                return p.x >= rect_pos.x && p.x < rect_pos.x + rect_size.x && p.y >= rect_pos.y &&
                       p.y < rect_pos.y + rect_size.y;
            }

            short int AABB(const sf::Vector2f& a_origin, const sf::Vector2f& a_size,
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

            bool circleCircle(const sf::Vector2f& a_origin, float a_r,
                                     const sf::Vector2f& b_origin, float b_r)
            {
                return (utils::geo::getDistance(a_origin, b_origin) < a_r + b_r);
            }

            short int ABCircle(const sf::Vector2f& a_origin, const sf::Vector2f& a_size,
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
                if (utils::geo::isPointInRectangle(b_origin, a_origin - a_size / 2.0f, a_size))
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

                if (utils::geo::isPointInRectangle(b_origin, r_o1 - r_s1 / 2.0f, r_s1))
                    return 1;

                if (utils::geo::isPointInRectangle(b_origin, r_o2 - r_s2 / 2.0f, r_s2))
                    return 2;

                if (utils::geo::isPointInRectangle(b_origin, r_o3 - r_s3 / 2.0f, r_s3))
                    return 3;

                if (utils::geo::isPointInRectangle(b_origin, r_o4 - r_s4 / 2.0f, r_s4))
                    return 4;

                sf::Vector2f c_o1 = {a_origin.x - a_size.x / 2.0f, a_origin.y - a_size.y / 2.0f};
                sf::Vector2f c_o2 = {a_origin.x + a_size.x / 2.0f, a_origin.y - a_size.y / 2.0f};
                sf::Vector2f c_o3 = {a_origin.x + a_size.x / 2.0f, a_origin.y + a_size.y / 2.0f};
                sf::Vector2f c_o4 = {a_origin.x - a_size.x / 2.0f, a_origin.y + a_size.y / 2.0f};

                if (utils::geo::getDistance(b_origin, c_o1) < b_r)
                    return 6;

                if (utils::geo::getDistance(b_origin, c_o2) < b_r)
                    return 7;

                if (utils::geo::getDistance(b_origin, c_o3) < b_r)
                    return 8;

                if (utils::geo::getDistance(b_origin, c_o4) < b_r)
                    return 9;

                return 0;
            }

            std::tuple<sf::Vector2f, sf::Vector2f, sf::Vector2f> generateCollisionAABB(const StaticObject& a)
            {
                std::tuple<sf::Vector2f, sf::Vector2f, sf::Vector2f> out;
                std::get<2>(out) = {a.getCollisionArea().getA(), a.getCollisionArea().getB()};
                std::get<0>(out) = a.getPosition() + a.getCollisionArea().getOffset() - std::get<2>(out) / 2.0f;
                std::get<1>(out) = a.getPosition() + a.getCollisionArea().getOffset() + std::get<2>(out) / 2.0f;

                return out;
            }

            float dotProduct(const sf::Vector2f& a, const sf::Vector2f& b)
            {
                return (a.x * b.x + a.y * b.y);
            }

            float getAngle(const sf::Vector2f& a, const sf::Vector2f& b)
            {
                float dot = utils::geo::dotProduct(utils::geo::getNormalized(a), utils::geo::getNormalized(b));
                dot = (dot < -1.0 ? -1.0 : (dot > 1.0 ? 1.0 : dot));

                return std::acos(dot);
            }

            float wrapAngle0_360(float angle)
            {
                angle = std::fmod(angle, 360.0f);

                if (angle < 0.0f)
                    angle += 360.0f;

                return angle;
            }

            float wrapAngle0_2PI(float angle)
            {
                angle = std::fmod(angle, 2 * M_PI);

                if (angle < 0.0f)
                    angle += 2 * M_PI;

                return angle;
            }

            float getAngleBetweenDegree(float angle_1, float angle_2)
            {
                auto res = angle_1 - angle_2;
                return (static_cast<int>(res) + 180) % 360 - 180.0f;
            }

            float getAngleBetweenRadian(float angle_1, float angle_2)
            {
                return getAngleBetweenDegree(angle_1 * 180.0f / M_PI, angle_2 * 180.0f / M_PI) * M_PI / 180.0f;
            }

            sf::Vector2f getNearestForwardPointToPath(const sf::Vector2f& pos, const ai::Path& path)
            {
                if (path.empty()) return {};

                auto nearest_it = path.begin();
                auto dist_to = utils::geo::getDistance(pos, nearest_it->first);
                auto dist = dist_to + nearest_it->second;

                for (auto it = path.begin(); it != path.end(); ++it)
                {
                    auto new_dist_to = utils::geo::getDistance(pos, it->first);
                    auto new_dist = dist_to + it->second;
                    bool should_break = false;
                    if (new_dist_to > dist_to) should_break = true;

                    if (new_dist + new_dist_to <= dist + dist_to)
                    {
                        nearest_it = it;
                        dist_to = new_dist_to;
                        dist = new_dist;
                    }

                    if (should_break) break;
                }

                return nearest_it->first;
            }
        } // namespace geo
    } // namespace utils
} // namespace r3e
