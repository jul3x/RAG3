//
// Created by jprolejko on 17.07.19.
//

#include <gtest/gtest.h>

#include <utils/Numeric.h>

TEST(NumericTest, IsNearlyEqual) {
    // positive values
    EXPECT_TRUE(utils::isNearlyEqual(0.0f, 0.0f, 0.001f));
    EXPECT_TRUE(utils::isNearlyEqual(0.0f, 0.999f, 1.000f));
    EXPECT_TRUE(utils::isNearlyEqual(0.999f, 0.0f, 1.000f));
    EXPECT_TRUE(utils::isNearlyEqual(0.999f, 0.999f, 0.001f));

    EXPECT_FALSE(utils::isNearlyEqual(0.999f, 0.995f, 0.001f));
    EXPECT_FALSE(utils::isNearlyEqual(0.0f, 0.1f, 0.001f));
    EXPECT_FALSE(utils::isNearlyEqual(0.1f, 0.0f, 0.001f));

    // negative values
    EXPECT_TRUE(utils::isNearlyEqual(0.0f, -0.999f, 1.000f));
    EXPECT_TRUE(utils::isNearlyEqual(-0.999f, 0.0f, 1.000f));
    EXPECT_TRUE(utils::isNearlyEqual(-0.999f, -0.999f, 0.001f));

    EXPECT_FALSE(utils::isNearlyEqual(-0.999f, -0.995f, 0.001f));
    EXPECT_FALSE(utils::isNearlyEqual(0.0f, -0.1f, 0.001f));
    EXPECT_FALSE(utils::isNearlyEqual(-0.1f, 0.0f, 0.001f));

    // positive-negative values
    EXPECT_FALSE(utils::isNearlyEqual(-0.999f, 0.999f, 0.001f));

    EXPECT_FALSE(utils::isNearlyEqual(0.999f, -0.995f, 0.001f));
    EXPECT_FALSE(utils::isNearlyEqual(1.0f, -0.1f, 0.001f));
    EXPECT_FALSE(utils::isNearlyEqual(-0.1f, 1.0f, 0.001f));

    EXPECT_TRUE(utils::isNearlyEqual(0.001f, -0.001f, 0.003f));
    EXPECT_TRUE(utils::isNearlyEqual(-0.001f, 0.001f, 0.003f));

    EXPECT_FALSE(utils::isNearlyEqual(0.001f, -0.003f, 0.003f));
    EXPECT_FALSE(utils::isNearlyEqual(-0.001f, 0.003f, 0.003f));

    // negavite error
    EXPECT_FALSE(utils::isNearlyEqual(0.001f, 0.001f, -0.003f));
}

TEST(NumericTest, IsNearlyEqualVec) {
    // positive values
    EXPECT_TRUE(utils::isNearlyEqual({0.0f, 0.0f}, {0.0f, 0.0f}, 0.001f));
    EXPECT_TRUE(utils::isNearlyEqual({0.0f, 0.0f}, {0.999f, 0.0f}, 1.000f));
    EXPECT_TRUE(utils::isNearlyEqual({0.0f, 0.0f}, {0.999f, 0.999f}, 1.000f));
    EXPECT_TRUE(utils::isNearlyEqual({0.0f, 0.0f}, {0.0f, 0.999f}, 1.000f));
    EXPECT_TRUE(utils::isNearlyEqual({0.999f, 0.999f}, {0.999f, 0.999f}, 0.001f));

    EXPECT_FALSE(utils::isNearlyEqual({0.999f, 0.0f}, {0.995f, 0.0f}, 0.001f));
    EXPECT_FALSE(utils::isNearlyEqual({0.0f, 0.999f}, {0.0f, 0.995f}, 0.001f));

    // negative values
    EXPECT_TRUE(utils::isNearlyEqual({0.0f, 0.0f}, {-0.999f, 0.0f}, 1.000f));
    EXPECT_TRUE(utils::isNearlyEqual({0.0f, 0.0f}, {-0.999f, -0.999f}, 1.000f));
    EXPECT_TRUE(utils::isNearlyEqual({0.0f, 0.0f}, {0.0f, -0.999f}, 1.000f));
    EXPECT_TRUE(utils::isNearlyEqual({-0.999f, -0.999f}, {-0.999f, -0.999f}, 0.001f));

    EXPECT_FALSE(utils::isNearlyEqual({-0.999f, 0.0f}, {-0.995f, 0.0f}, 0.001f));
    EXPECT_FALSE(utils::isNearlyEqual({0.0f, -0.999f}, {0.0f, -0.995f}, 0.001f));

    // positive-negative values
    EXPECT_TRUE(utils::isNearlyEqual({0.0f, 0.0f}, {-0.999f, 0.0f}, 1.000f));
    EXPECT_TRUE(utils::isNearlyEqual({0.0f, 0.0f}, {-0.999f, 0.999f}, 1.000f));
    EXPECT_TRUE(utils::isNearlyEqual({-0.999f, 0.999f}, {-0.999f, 0.999f}, 0.001f));
    EXPECT_TRUE(utils::isNearlyEqual({0.999f, -0.999f}, {0.999f, -0.999f}, 0.001f));

    EXPECT_FALSE(utils::isNearlyEqual({-0.999f, 0.0f}, {0.995f, 0.0f}, 0.001f));
    EXPECT_FALSE(utils::isNearlyEqual({0.0f, -0.999f}, {0.0f, 0.995f}, 0.001f));

    // negavite error
    EXPECT_FALSE(utils::isNearlyEqual({0.0f, 0.0f}, {0.0f, 0.0f}, -0.003f));
}

TEST(NumericTest, IsBetween) {
    EXPECT_FALSE(utils::isBetween(0.0f, 0.0f, 0.0f));

    EXPECT_TRUE(utils::isBetween(0.5f, 0.0f, 1.0f));
    EXPECT_FALSE(utils::isBetween(1.1f, 0.0f, 1.0f));
    EXPECT_FALSE(utils::isBetween(-0.1f, 0.0f, 1.0f));

    EXPECT_TRUE(utils::isBetween(0.0f, -0.5f, 0.5f));
    EXPECT_FALSE(utils::isBetween(-0.51f, -0.5f, 0.5f));
    EXPECT_FALSE(utils::isBetween(0.51f, -0.5f, 0.5f));

    EXPECT_TRUE(utils::isBetween(-0.5f, -1.0f, 0.0f));
    EXPECT_FALSE(utils::isBetween(-1.01f, -1.0f, 0.0f));
    EXPECT_FALSE(utils::isBetween(0.01f, -1.0f, 0.0f));
}

TEST(NumericTest, GetRandom) {
    float rand_f;

    EXPECT_NO_THROW(rand_f = utils::getRandom<float>(0.0f, 1.0f));
    EXPECT_LT(rand_f, 1.0f);
    EXPECT_GT(rand_f, 0.0f);

    EXPECT_NO_THROW(rand_f = utils::getRandom<float>(-1.0f, 0.0f));
    EXPECT_LT(rand_f, 0.0f);
    EXPECT_GT(rand_f, -1.0f);

    EXPECT_NO_THROW(rand_f = utils::getRandom<float>(-1.0f, 1.0f));
    EXPECT_LT(rand_f, 1.0f);
    EXPECT_GT(rand_f, -1.0f);

    int rand_i;

    EXPECT_NO_THROW(rand_i = utils::getRandom<int>(0, 1));
    EXPECT_LE(rand_i, 1);
    EXPECT_GE(rand_i, 0);
    
    EXPECT_NO_THROW(rand_i = utils::getRandom<int>(-1, 0));
    EXPECT_LE(rand_i, 0);
    EXPECT_GE(rand_i, -1);

    EXPECT_NO_THROW(rand_i = utils::getRandom<int>(-1, 1));
    EXPECT_LE(rand_i, 1);
    EXPECT_GE(rand_i, -1);
}