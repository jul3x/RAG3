//
// Created by jul3x on 13.06.19.
//

#ifndef RAG3_ENGINE_INCLUDE_R3E_UTILS_NUMERIC_H
#define RAG3_ENGINE_INCLUDE_R3E_UTILS_NUMERIC_H

#include <random>
#include <cmath>

#include <SFML/System/Vector2.hpp>


namespace r3e::utils::num {
    template<class T>
    T getRandom(const T& left, const T& right)
    {
        static std::random_device rd;
        static std::mt19937 mt(rd());
        std::uniform_real_distribution<> distribution(static_cast<double>(left), static_cast<double>(right));

        return static_cast<T>(distribution(mt));
    }

    template<>
    inline int getRandom<int>(const int& left, const int& right)
    {
        return static_cast<int>(std::lround(utils::num::getRandom<float>(left, right)));
    }

    template<class T>
    bool isBetween(const T& num, const T& a, const T& b)
    {
        return a < num && num < b;
    }

    template<class T>
    bool isNearlyEqual(const T& a, const T& b, float abs_error = 0.001f)
    {
        return std::abs(a - b) < abs_error;
    }

    template<class T>
    bool isNearlyEqual(const sf::Vector2<T>& a, const sf::Vector2<T>& b, float abs_error = 0.001f)
    {
        return std::abs(a.x - b.x) < abs_error && std::abs(a.y - b.y) < abs_error;
    }

    template <class T>
    inline void hashCombine(std::size_t& seed, const T& v)
    {
        std::hash<T> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
    }

    template <class T>
    inline size_t getHash(const T& objs)
    {
        size_t seed = 0;
        for (const auto& obj : objs)
        {
            hashCombine(seed, obj);
        }

        return seed;
    }

} // namespace r3e::utils::num

#endif //RAG3_ENGINE_INCLUDE_R3E_UTILS_NUMERIC_H