//
// Created by jul3x on 17.11.2020.
//

#ifndef RAG3_ENGINE_INCLUDE_R3E_UTILS_MISC_H
#define RAG3_ENGINE_INCLUDE_R3E_UTILS_MISC_H

#include <chrono>
#include <list>
#include <functional>
#include <sstream>

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/Socket.hpp>

#include <R3E/j3x/J3X.h>


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

    template<class T>
    void eraseIfUpdated(std::list<T>& elements, float time_elapsed)
    {
        eraseIf<T>(elements, [time_elapsed](T& element) { return !element.update(time_elapsed); });
    }

    template<class T, class K>
    bool contains(const T& container, const K& element)
    {
        return std::find(container.begin(), container.end(), element) != container.end();
    }

    template<class K>
    inline bool contains(const j3x::List& container, const K& element)
    {
        for (const auto& obj : container)
        {
            if (j3x::getObj<K>(obj) == element)
                return true;
        }
        return false;
    }

    template<class T>
    std::string toString(T v)
    {
        return std::to_string(v);
    }

    template<>
    inline std::string toString<float>(float v)
    {
        static constexpr auto PRECISION = 2;
        std::ostringstream out;
        out.precision(PRECISION);
        out << std::fixed << v;
        return out.str();
    }

    template<>
    inline std::string toString<sf::Socket::Status>(sf::Socket::Status status)
    {
        switch (status)
        {
            case sf::Socket::Disconnected:
                return "Disconnected";
            case sf::Socket::Done:
                return "Done";
            case sf::Socket::Error:
                return "Error";
            case sf::Socket::NotReady:
                return "NotReady";
            case sf::Socket::Partial:
                return "Partial";
            default:
                return "Unknown";
        }
    }

    uint64_t timeSinceEpochMillisec();

    std::string capitalFirst(const std::string& word);
    std::string toLower(const std::string& word);
    std::string toUpper(const std::string& word);
    std::string humanize(const std::string& word);
    std::string wrapText(std::string text, int max_chars);
    bool startsWith(const std::string& what, const std::string& prefix);
    bool endsWith(const std::string& what, const std::string& suffix);
    std::string keyToString(sf::Keyboard::Key key);
    sf::Keyboard::Key stringToKey(const std::string& str);
    sf::IpAddress getSafeIP(const sf::IpAddress& ip, const sf::IpAddress& local);
}

#endif //RAG3_ENGINE_INCLUDE_R3E_UTILS_MISC_H
