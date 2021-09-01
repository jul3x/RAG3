//
// Created by jul3x on 17.11.2020.
//

#ifndef RAG3_ENGINE_INCLUDE_R3E_UTILS_MISC_H
#define RAG3_ENGINE_INCLUDE_R3E_UTILS_MISC_H

#include <chrono>
#include <list>
#include <functional>

#include <SFML/Window/Keyboard.hpp>


namespace r3e::utils {
    template<class T>
    void eraseIf(std::list<T>& elements, const std::function<bool(T&)>& condition)
    {
        for (auto it = elements.begin(); it != elements.end();)
        {
            bool do_increment = true;

            if (condition(*it))
            {
                auto next_it = std::next(it);
                elements.erase(it);
                it = next_it;
                do_increment = false;
            }
            if (do_increment)
                ++it;
        }
    }

    template<class T, class K>
    bool contains(const T& container, const K& element) {
        return std::find(container.begin(), container.end(), element) != container.end();
    }

    uint64_t timeSinceEpochMillisec();

    std::string capitalFirst(const std::string& word);
    std::string toLower(const std::string& word);
    std::string toUpper(const std::string& word);
    std::string humanize(const std::string& word);
    std::string floatToString(float v, float precision = 2);
    bool startsWith(const std::string& what, const std::string& prefix);
    std::string keyToString(sf::Keyboard::Key key);
    sf::Keyboard::Key stringToKey(const std::string& str);
}

#endif //RAG3_ENGINE_INCLUDE_R3E_UTILS_MISC_H
