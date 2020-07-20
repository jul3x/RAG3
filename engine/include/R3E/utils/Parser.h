//
// Created by jul3x on 23.10.19.
//

#ifndef RAG3_ENGINE_INCLUDE_R3E_UTILS_PARSER_H
#define RAG3_ENGINE_INCLUDE_R3E_UTILS_PARSER_H

#include <sstream>
#include <fstream>
#include <iostream>
#include <tuple>
#include <unordered_map>
#include <vector>

#include <SFML/System/Vector2.hpp>


namespace r3e::utils::j3x {
    constexpr char DELIMITER_ = ';';

    template<class T>
    using Params = std::unordered_map<std::string, T>;

    using Parameters = std::tuple<Params<int>, Params<float>, Params<std::string>,
            Params<std::vector<int>>, Params<std::vector<float>>, Params<std::vector<std::string>>>;

    Parameters parse(const std::string& filename, const std::string& ns);

    Parameters parse(const std::string& filename);

    void mergeParams(Parameters& params, const Parameters& new_params);

    template<class T>
    void tokenize(const std::string &str, char delimiter, T& out)
    {
        out.clear();

        size_t start;
        size_t end = 0;

        while ((start = str.find_first_not_of(delimiter, end)) != std::string::npos)
        {
            end = str.find(delimiter, start);
            out.push_back(str.substr(start, end - start));
        }
    }

    template<class T>
    T convert(const std::string& str)
    {
        return {};
    }

    template<>
    inline sf::Vector2f convert<sf::Vector2f>(const std::string& str)
    {
        auto data = str.substr(1, str.length() - 1);
        std::vector<std::string> data_str;

        utils::j3x::tokenize(data, ',', data_str);

        return {std::stof(data_str.at(0)), std::stof(data_str.at(1))};
    }

    template <class T>
    const T& get(const Params<T>& params, const std::string& key, bool ignore_warn = false)
    {
        static T ERR = {};
        auto it = params.find(key);
        if (it == params.end())
        {
            if (!ignore_warn)
                std::cerr << "[J3X] Param " << key << " not found!" << std::endl;

            return ERR;
        }

        return it->second;
    }

    template <class T>
    const T& get(const Parameters& params, const std::string& key, bool ignore_warn = false)
    {
        throw std::invalid_argument("[J3X] Parameter type not handled!");
    }

    template <class T>
    void set(Params<T>& params, const std::string& key, const T& value)
    {
        params[key] = value;
    }

    template <class T>
    void set(Parameters& params, const std::string& key, const T& value)
    {
        throw std::invalid_argument("[J3X] Parameter type not handled!");
    }

    template <>
    inline const int& get<int>(const Parameters& params, const std::string& key, bool ignore_warn)
    {
        return get<int>(std::get<0>(params), key, ignore_warn);
    }

    template <>
    inline const float& get<float>(const Parameters& params, const std::string& key, bool ignore_warn)
    {
        return get<float>(std::get<1>(params), key, ignore_warn);
    }

    template <>
    inline const std::string& get<std::string>(const Parameters& params, const std::string& key, bool ignore_warn)
    {
        return get<std::string>(std::get<2>(params), key, ignore_warn);
    }

    template <>
    inline const std::vector<int>& get<std::vector<int>>(const Parameters& params, const std::string& key, bool ignore_warn)
    {
        return get<std::vector<int>>(std::get<3>(params), key, ignore_warn);
    }

    template <>
    inline const std::vector<float>& get<std::vector<float>>(const Parameters& params, const std::string& key, bool ignore_warn)
    {
        return get<std::vector<float>>(std::get<4>(params), key, ignore_warn);
    }

    template <>
    inline const std::vector<std::string>& get<std::vector<std::string>>(const Parameters& params, const std::string& key, bool ignore_warn)
    {
        return get<std::vector<std::string>>(std::get<5>(params), key, ignore_warn);
    }

    template <>
    inline void set<int>(Parameters& params, const std::string& key, const int& value)
    {
        set<int>(std::get<0>(params), key, value);
    }

    template <>
    inline void set<float>(Parameters& params, const std::string& key, const float& value)
    {
        set<float>(std::get<1>(params), key, value);
    }

    template <>
    inline void set<std::string>(Parameters& params, const std::string& key, const std::string& value)
    {
        set<std::string>(std::get<2>(params), key, value);
    }

    template <>
    inline void set<std::vector<int>>(Parameters& params, const std::string& key, const std::vector<int>& value)
    {
        set<std::vector<int>>(std::get<3>(params), key, value);
    }

    template <>
    inline void set<std::vector<float>>(Parameters& params, const std::string& key, const std::vector<float>& value)
    {
        set<std::vector<float>>(std::get<4>(params), key, value);
    }

    template <>
    inline void set<std::vector<std::string>>(Parameters& params, const std::string& key, const std::vector<std::string>& value)
    {
        set<std::vector<std::string>>(std::get<5>(params), key, value);
    }

} // namespace r3e::utils::j3x

#endif //RAG3_ENGINE_INCLUDE_R3E_UTILS_PARSER_H