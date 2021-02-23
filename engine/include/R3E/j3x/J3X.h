//
// Created by jul3x on 23.10.19.
//

#ifndef RAG3_ENGINE_INCLUDE_R3E_UTILS_PARSER_H
#define RAG3_ENGINE_INCLUDE_R3E_UTILS_PARSER_H

#include <any>
#include <sstream>
#include <fstream>
#include <iostream>
#include <tuple>
#include <unordered_map>
#include <vector>
#include <memory>

#include <SFML/System/Vector2.hpp>

#include <R3E/system/Logger.h>
#include "Parser.h"
#include "J3XVisitor.h"


namespace r3e::j3x {
    constexpr char DELIMITER_ = ';';

    using Obj = std::any;
    using List = std::vector<Obj>;
    using Parameters = std::unordered_map<std::string, Obj>;

    std::shared_ptr<Parameters> parse(const std::string& filename, const std::string& ns);

    std::shared_ptr<Parameters> parse(const std::string& filename);

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

        j3x::tokenize(data, ',', data_str);

        return {std::stof(data_str.at(0)), std::stof(data_str.at(1))};
    }

    template <class T>
    bool has(const Parameters& params, const std::string& key)
    {
        auto it = params.find(key);
        return (it != params.end());
    }

    template <class T>
    const T& get(const Parameters& params, const std::string& key, bool ignore_warn = false)
    {
        static T ERR = {};
        auto it = params.find(key);
        if (it == params.end())
        {
            if (!ignore_warn)
                LOG.error("[J3X] Param " + key + " not found!");

            return ERR;
        }

        try {
            return std::any_cast<const T&>(it->second);
        }
        catch (const std::bad_any_cast& exception) {
            if (!ignore_warn)
                LOG.error("[J3X] Param " + key + " not matching return type! It should be \"" + it->second.type().name() + "\".");

            return ERR;
        }
    }

    template <class T>
    const T& getObj(const Obj& param, bool ignore_warn = false)
    {
        static T ERR = {};
        try
        {
            return std::any_cast<const T&>(param);
        }
        catch (const std::bad_any_cast& exception)
        {
            if (!ignore_warn)
            {
                LOG.error("[J3X] Param of type " + std::string(param.type().name()) + " not matching " +
                          std::string(typeid(T).name()) + " return type!");
            }

            return ERR;
        }
    }

    template <class T>
    const T& getObj(const j3x::List& list, size_t index, bool ignore_warn = false)
    {
        return getObj<T>(list.at(index), ignore_warn);
    }

    template <class T>
    void set(Parameters& params, const std::string& key, const T& value)
    {
        params[key] = value;
    }

    void serialize(const j3x::List& data, std::string& out);

    void serialize(const j3x::Obj& obj, std::string& out);

    void serializeAssign(const std::string& variable, const j3x::Obj& obj, std::string& out);

    template<class T>
    T parseObj(const std::string& type, const std::string& str)
    {
        J3XVisitor visitor("");
        try
        {
            std::string in_str = type + " tmp = " + str;
            Script *parse_tree = pScript(in_str.c_str());
            if (!parse_tree) {
                throw std::logic_error("parse error");
            }
            parse_tree->accept(&visitor);

            delete parse_tree;
        }
        catch (const std::exception& e)
        {
            LOG.error("[J3X] Error while reading " + str + ".\nError message: " + e.what() + "!\n");
            return {};
        }

        return getObj<T>(visitor.getParams().at("tmp"));
    }

} // namespace r3e::j3x

#endif //RAG3_ENGINE_INCLUDE_R3E_UTILS_PARSER_H