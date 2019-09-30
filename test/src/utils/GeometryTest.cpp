//
// Created by jprolejko on 17.07.19.
//

#include <gtest/gtest.h>

#include <utils/Numeric.h>
#include <utils/Geometry.h>

TEST(GeometryTest, GetDistance) {
    // same vector
    sf::Vector2f a{}, b{};
    float err = 0.001;

    EXPECT_NEAR(utils::getDistance(a, b), 0.0f, err);
    a = {1.0f, 0.0f}; b = {1.0f, 0.0f};
    EXPECT_NEAR(utils::getDistance(a, b), 0.0f, err);
    a = {1.0f, 1.0f}; b = {1.0f, 1.0f};
    EXPECT_NEAR(utils::getDistance(a, b), 0.0f, err);
    a = {0.0f, 1.0f}; b = {0.0f, 1.0f};
    EXPECT_NEAR(utils::getDistance(a, b), 0.0f, err);
    a = {-1.0f, 0.0f}; b = {-1.0f, 0.0f};
    EXPECT_NEAR(utils::getDistance(a, b), 0.0f, err);
    a = {-1.0f, -1.0f}; b = {-1.0f, -1.0f};
    EXPECT_NEAR(utils::getDistance(a, b), 0.0f, err);
    a = {0.0f, -1.0f}; b = {0.0f, -1.0f};
    EXPECT_NEAR(utils::getDistance(a, b), 0.0f, err);

    // different signum
    a = {1.0f, 1.0f}; b = {-1.0f, 1.0f};
    EXPECT_FALSE(utils::isNearlyEqual(utils::getDistance(a, b), 0.0f, err));
    a = {1.0f, 1.0f}; b = {1.0f, -1.0f};
    EXPECT_FALSE(utils::isNearlyEqual(utils::getDistance(a, b), 0.0f, err));

    // different vector
    a = {1.0f, 0.0f}; b = {0.0f, 0.0f};
    EXPECT_NEAR(utils::getDistance(a, b), 1.0f, err);
    a = {1.0f, 0.0f}; b = {2.0f, 0.0f};
    EXPECT_NEAR(utils::getDistance(a, b), 1.0f, err);
    a = {-1.0f, 0.0f}; b = {0.0f, 0.0f};
    EXPECT_NEAR(utils::getDistance(a, b), 1.0f, err);

    a = {0.0f, 1.0f}; b = {0.0f, 0.0f};
    EXPECT_NEAR(utils::getDistance(a, b), 1.0f, err);
    a = {0.0f, 1.0f}; b = {0.0f, 2.0f};
    EXPECT_NEAR(utils::getDistance(a, b), 1.0f, err);
    a = {0.0f, -1.0f}; b = {0.0f, 0.0f};
    EXPECT_NEAR(utils::getDistance(a, b), 1.0f, err);

    a = {1.0f, 1.0f}; b = {2.0f, 2.0f};
    EXPECT_NEAR(utils::getDistance(a, b), 1.41f, 0.1);
    a = {0.0f, 0.0f}; b = {3.0f, 4.0f};
    EXPECT_NEAR(utils::getDistance(a, b), 5.0f, err);
    a = {0.0f, 0.0f}; b = {4.0f, 3.0f};
    EXPECT_NEAR(utils::getDistance(a, b), 5.0f, err);

    a = {-912.0f, 123.0f}; b = {45.0f, -332.0f};
    EXPECT_NEAR(utils::getDistance(a, b), utils::getDistance(b, a), err);
}

TEST(GeometryTest, CartesianPolarConversion) {
    sf::Vector2f vec, converted;
    float err = 0.001f;

    // formal test
    vec = {1.0f, 1.0f};
    float r, theta_rad;
    std::tie(r, theta_rad) = utils::cartesianToPolar(vec);
    EXPECT_NEAR(r, 1.41f, 0.01f);
    EXPECT_NEAR(theta_rad, M_PI_4, err);

    r = 10.0f; theta_rad = M_PI;
    vec = utils::polarToCartesian(r, theta_rad);
    EXPECT_NEAR(vec.x, -10.0f, err);
    EXPECT_NEAR(vec.y, 0.0f, err);

    // double test
    vec = {};
    converted = utils::polarToCartesian(std::get<0>(utils::cartesianToPolar(vec)),
                                        std::get<1>(utils::cartesianToPolar(vec)));
    EXPECT_NEAR(converted.x, vec.x, err);
    EXPECT_NEAR(converted.y, vec.y, err);

    vec = {1.0f, 0.0f};
    converted = utils::polarToCartesian(std::get<0>(utils::cartesianToPolar(vec)),
                                        std::get<1>(utils::cartesianToPolar(vec)));
    EXPECT_NEAR(converted.x, vec.x, err);
    EXPECT_NEAR(converted.y, vec.y, err);

    vec = {0.0f, 1.0f};
    converted = utils::polarToCartesian(std::get<0>(utils::cartesianToPolar(vec)),
                                        std::get<1>(utils::cartesianToPolar(vec)));
    EXPECT_NEAR(converted.x, vec.x, err);
    EXPECT_NEAR(converted.y, vec.y, err);

    vec = {1.0f, 1.0f};
    converted = utils::polarToCartesian(std::get<0>(utils::cartesianToPolar(vec)),
                                        std::get<1>(utils::cartesianToPolar(vec)));
    EXPECT_NEAR(converted.x, vec.x, err);
    EXPECT_NEAR(converted.y, vec.y, err);

    vec = {-1.0f, 0.0f};
    converted = utils::polarToCartesian(std::get<0>(utils::cartesianToPolar(vec)),
                                        std::get<1>(utils::cartesianToPolar(vec)));
    EXPECT_NEAR(converted.x, vec.x, err);
    EXPECT_NEAR(converted.y, vec.y, err);

    vec = {0.0f, -1.0f};
    converted = utils::polarToCartesian(std::get<0>(utils::cartesianToPolar(vec)),
                                        std::get<1>(utils::cartesianToPolar(vec)));
    EXPECT_NEAR(converted.x, vec.x, err);
    EXPECT_NEAR(converted.y, vec.y, err);

    vec = {-23.0f, -24.0f};
    converted = utils::polarToCartesian(std::get<0>(utils::cartesianToPolar(vec)),
                                        std::get<1>(utils::cartesianToPolar(vec)));
    EXPECT_NEAR(converted.x, vec.x, err);
    EXPECT_NEAR(converted.y, vec.y, err);
}

TEST(GeometryTest, VectorLengthLimit) {
    sf::Vector2f vec, converted;
    float max_length, r1, theta_rad1, r2, theta_rad2;
    float err = 0.001f;

    // zero vector
    vec = {};
    max_length = 0.0f;
    converted = utils::vectorLengthLimit(vec, max_length);
    EXPECT_NEAR(converted.x, vec.x, err);
    EXPECT_NEAR(converted.y, vec.y, err);

    vec = {};
    max_length = 1.0f;
    converted = utils::vectorLengthLimit(vec, max_length);
    EXPECT_NEAR(converted.x, vec.x, err);
    EXPECT_NEAR(converted.y, vec.y, err);

    vec = {};
    max_length = -1.0f;
    EXPECT_THROW(utils::vectorLengthLimit(vec, max_length), std::invalid_argument);

    // I quarter
    vec = {0.5f, 0.5f};
    max_length = 1.0f;
    converted = utils::vectorLengthLimit(vec, max_length);
    EXPECT_NEAR(converted.x, vec.x, err);
    EXPECT_NEAR(converted.y, vec.y, err);

    vec = {1.0f, 1.0f};
    max_length = 1.0f;
    converted = utils::vectorLengthLimit(vec, max_length);
    std::tie(r1, theta_rad1) = utils::cartesianToPolar(vec);
    std::tie(r2, theta_rad2) = utils::cartesianToPolar(converted);
    EXPECT_NEAR(r2, max_length, err);
    EXPECT_NEAR(theta_rad1, theta_rad2, err);

    vec = {231.0f, 198.0f};
    max_length = 51.0f;
    converted = utils::vectorLengthLimit(vec, max_length);
    std::tie(r1, theta_rad1) = utils::cartesianToPolar(vec);
    std::tie(r2, theta_rad2) = utils::cartesianToPolar(converted);
    EXPECT_NEAR(r2, max_length, err);
    EXPECT_NEAR(theta_rad1, theta_rad2, err);

    // II quarter
    vec = {-0.5f, 0.5f};
    max_length = 1.0f;
    converted = utils::vectorLengthLimit(vec, max_length);
    EXPECT_NEAR(converted.x, vec.x, err);
    EXPECT_NEAR(converted.y, vec.y, err);

    vec = {-1.0f, 1.0f};
    max_length = 1.0f;
    converted = utils::vectorLengthLimit(vec, max_length);
    std::tie(r1, theta_rad1) = utils::cartesianToPolar(vec);
    std::tie(r2, theta_rad2) = utils::cartesianToPolar(converted);
    EXPECT_NEAR(r2, max_length, err);
    EXPECT_NEAR(theta_rad1, theta_rad2, err);

    vec = {-231.0f, 198.0f};
    max_length = 51.0f;
    converted = utils::vectorLengthLimit(vec, max_length);
    std::tie(r1, theta_rad1) = utils::cartesianToPolar(vec);
    std::tie(r2, theta_rad2) = utils::cartesianToPolar(converted);
    EXPECT_NEAR(r2, max_length, err);
    EXPECT_NEAR(theta_rad1, theta_rad2, err);

    // III quarter
    vec = {-0.5f, -0.5f};
    max_length = 1.0f;
    converted = utils::vectorLengthLimit(vec, max_length);
    EXPECT_NEAR(converted.x, vec.x, err);
    EXPECT_NEAR(converted.y, vec.y, err);

    vec = {-1.0f, -1.0f};
    max_length = 1.0f;
    converted = utils::vectorLengthLimit(vec, max_length);
    std::tie(r1, theta_rad1) = utils::cartesianToPolar(vec);
    std::tie(r2, theta_rad2) = utils::cartesianToPolar(converted);
    EXPECT_NEAR(r2, max_length, err);
    EXPECT_NEAR(theta_rad1, theta_rad2, err);

    vec = {-231.0f, -198.0f};
    max_length = 51.0f;
    converted = utils::vectorLengthLimit(vec, max_length);
    std::tie(r1, theta_rad1) = utils::cartesianToPolar(vec);
    std::tie(r2, theta_rad2) = utils::cartesianToPolar(converted);
    EXPECT_NEAR(r2, max_length, err);
    EXPECT_NEAR(theta_rad1, theta_rad2, err);

    // IV quarter
    vec = {0.5f, -0.5f};
    max_length = 1.0f;
    converted = utils::vectorLengthLimit(vec, max_length);
    EXPECT_NEAR(converted.x, vec.x, err);
    EXPECT_NEAR(converted.y, vec.y, err);

    vec = {1.0f, -1.0f};
    max_length = 1.0f;
    converted = utils::vectorLengthLimit(vec, max_length);
    std::tie(r1, theta_rad1) = utils::cartesianToPolar(vec);
    std::tie(r2, theta_rad2) = utils::cartesianToPolar(converted);
    EXPECT_NEAR(r2, max_length, err);
    EXPECT_NEAR(theta_rad1, theta_rad2, err);

    vec = {231.0f, -198.0f};
    max_length = 51.0f;
    converted = utils::vectorLengthLimit(vec, max_length);
    std::tie(r1, theta_rad1) = utils::cartesianToPolar(vec);
    std::tie(r2, theta_rad2) = utils::cartesianToPolar(converted);
    EXPECT_NEAR(r2, max_length, err);
    EXPECT_NEAR(theta_rad1, theta_rad2, err);
}

TEST(GeometryTest, IsPointInRectangle) {
    EXPECT_TRUE(utils::isPointInRectangle({0, 0}, {0, 0}, {10, 10}));
    EXPECT_FALSE(utils::isPointInRectangle({-0.01, 0}, {0, 0}, {10, 10}));
    EXPECT_TRUE(utils::isPointInRectangle({-0.01, 0}, {-0.02, 0}, {10, 10}));

    EXPECT_FALSE(utils::isPointInRectangle({10, 0}, {0, 0}, {10, 10}));
    EXPECT_TRUE(utils::isPointInRectangle({9.99, 0}, {0, 0}, {10, 10}));
    EXPECT_TRUE(utils::isPointInRectangle({0, 9.99}, {0, 0}, {10, 10}));
    EXPECT_FALSE(utils::isPointInRectangle({0, 10}, {0, 0}, {10, 10}));

    EXPECT_TRUE(utils::isPointInRectangle({-5, -3}, {-5, -3}, {10, 1}));
    EXPECT_FALSE(utils::isPointInRectangle({-5.01, -3}, {-5, -3}, {10, 1}));
    EXPECT_FALSE(utils::isPointInRectangle({-5, -3.01}, {-5, -3}, {10, 1}));

    EXPECT_FALSE(utils::isPointInRectangle({5, -3}, {-5, -3}, {10, 1}));
    EXPECT_TRUE(utils::isPointInRectangle({4.99, -3}, {-5, -3}, {10, 1}));
    EXPECT_FALSE(utils::isPointInRectangle({4.99, -2}, {-5, -3}, {10, 1}));
    EXPECT_TRUE(utils::isPointInRectangle({4.99, -2.01}, {-5, -3}, {10, 1}));

    EXPECT_FALSE(utils::isPointInRectangle({5, -2}, {-5, -3}, {10, 1}));
    EXPECT_FALSE(utils::isPointInRectangle({-5, -2}, {-5, -3}, {10, 1}));
    EXPECT_TRUE(utils::isPointInRectangle({-5, -2.01}, {-5, -3}, {10, 1}));

    EXPECT_THROW(utils::isPointInRectangle({0, 0}, {0, 0}, {-1, 0}), std::invalid_argument);
    EXPECT_THROW(utils::isPointInRectangle({0, 0}, {0, 0}, {-1, -1}), std::invalid_argument);
    EXPECT_THROW(utils::isPointInRectangle({0, 0}, {0, 0}, {1, -1}), std::invalid_argument);
}