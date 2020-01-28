//
// Created by jul3x on 13.06.19.
//

#ifndef RAG3_ENGINE_UTILS_NUMERIC_H
#define RAG3_ENGINE_UTILS_NUMERIC_H

#include <random>
#include <cmath>

#include <SFML/System/Vector2.hpp>


namespace utils {
namespace num {
    template<class T>
    T getRandom(const T &left, const T &right) {
        static std::random_device rd;
        static std::mt19937 mt(rd());
        std::uniform_real_distribution<> distribution(static_cast<double>(left), static_cast<double>(right));

        return static_cast<T>(distribution(mt));
    }

    template<>
    inline int getRandom<int>(const int &left, const int &right) {
        return static_cast<int>(std::lround(utils::num::getRandom<float>(left, right)));
    }

    inline bool isBetween(float num, float a, float b) {
        return a < num && num < b;
    }

    inline bool isNearlyEqual(float a, float b, float abs_error = 0.001f) {
        return std::abs(a - b) < abs_error;
    }

    inline bool isNearlyEqual(const sf::Vector2f &a, const sf::Vector2f &b, float abs_error = 0.001f) {
        return std::abs(a.x - b.x) < abs_error && std::abs(a.y - b.y) < abs_error;
    }

} // namespace num
} // namespace utils


#endif // RAG3_ENGINE_UTILS_NUMERIC_H