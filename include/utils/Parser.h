//
// Created by jprolejko on 23.10.19.
//

#ifndef RAG3_UTILS_PARSER_H
#define RAG3_UTILS_PARSER_H

#include <sstream>
#include <fstream>
#include <iostream>
#include <tuple>
#include <map>


namespace utils {

using J3XIParameters = std::map<std::string, int>;
using J3XFParameters = std::map<std::string, float>;
using J3XParameters = std::tuple<J3XIParameters, J3XFParameters>;

inline J3XParameters parse(const std::string &filename) {
    std::ifstream config_file(filename);

    J3XFParameters float_params;
    J3XIParameters int_params;

    if (config_file)
    {
        std::cout << "Started parsing of " + filename + " file." << std::endl;
        std::stringstream s_file;
        s_file << config_file.rdbuf();

        std::string line;
        int line_count = 0;
        while (std::getline(s_file, line))
        {
            ++line_count;
            std::istringstream s_line(line);
            std::string type, key;
            if (std::getline(s_line, type, ' '))
            {
                if (std::getline(s_line, key, '='))
                {
                    std::string value;
                    if (std::getline(s_line, value))
                    {
                        try
                        {
                            if (type == "int")
                            {
                                int_params[key] = std::stoi(value);
                            }
                            else if (type == "int32")
                            {
                                int_params[key] = std::stoll(value, 0, 16);
                            }
                            else if (type == "float")
                            {
                                float_params[key] = std::stof(value);
                            }
                            else if (type == "bool")
                            {
                                int_params[key] = std::stoi(value);
                            }
                            else
                            {
                                throw std::logic_error("Not handled type " + type +
                                        " on line " + std::to_string(line_count));
                            }
                        }
                        catch (std::invalid_argument &e)
                        {
                            throw std::invalid_argument("Parse error: Wrong type " + type +
                                        " of parameter on line " +
                                        std::to_string(line_count));
                        }
                    }
                }
                else
                {
                    throw std::invalid_argument("Parse error on line " +
                            std::to_string(line_count));
                }
            }
        }
        std::cout << "Parsing of " + filename + " successful!" << std::endl;
        config_file.close();
    }
    else
    {
        throw std::logic_error("J3X " + filename + " file not found!");
    }

    return std::make_tuple(int_params, float_params);
}

inline int getInt(const J3XIParameters &int_params, const std::string &key) {
    if (int_params.find(key) == int_params.end())
    {
        std::cerr << "[J3X] Param " << key << " not found!" << std::endl;
        return 0;
    }

    return int_params.at(key);
}

inline float getFloat(const J3XFParameters &float_params, const std::string &key) {
    if (float_params.find(key) == float_params.end())
    {
        std::cerr << "[J3X] Param " << key << " not found!" << std::endl;
        return 0.0f;
    }

    return float_params.at(key);
}

inline void setInt(J3XIParameters &int_params, const std::string &key, int value) {
    int_params[key] = value;
}

inline void setFloat(J3XFParameters &float_params, const std::string &key, float value) {
    float_params[key] = value;
}

} // namespace utils

#endif // RAG3_UTILS_PARSER_H