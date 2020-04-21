//
// Created by jul3x on 21.04.20.
//

#include <r3e/utils/Numeric.h>


namespace r3e {
    namespace utils {
        namespace num {
            bool isBetween(float num, float a, float b)
            {
                return a < num && num < b;
            }

            bool isNearlyEqual(float a, float b, float abs_error)
            {
                return std::abs(a - b) < abs_error;
            }

            bool isNearlyEqual(const sf::Vector2f& a, const sf::Vector2f& b, float abs_error)
            {
                return std::abs(a.x - b.x) < abs_error && std::abs(a.y - b.y) < abs_error;
            }

        } // namespace num
    } // namespace utils
} // namespace r3e
