//
// Created by jul3x on 23.10.19.
//

#ifndef RAG3_ENGINE_UTILS_PARSER_H
#define RAG3_ENGINE_UTILS_PARSER_H

#include <sstream>
#include <fstream>
#include <iostream>
#include <tuple>
#include <unordered_map>


namespace r3e {
    namespace utils {

        using J3XIParameters = std::unordered_map<std::string, int>;
        using J3XFParameters = std::unordered_map<std::string, float>;
        using J3XSParameters = std::unordered_map<std::string, std::string>;
        using J3XParameters = std::tuple<J3XIParameters, J3XFParameters, J3XSParameters>;

        inline J3XParameters parse(const std::string& filename, const std::string& ns)
        {
            std::ifstream config_file(filename);

            J3XFParameters float_params;
            J3XIParameters int_params;
            J3XSParameters string_params;

            if (config_file)
            {
                std::cout << "[J3X] Started parsing of " + filename + " file." << std::endl;
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

                            key = ns.empty() ? key : ns + "/" + key;

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
                                    else if (type == "string")
                                    {
                                        string_params[key] = value;
                                    }
                                    else if (type == "bool")
                                    {
                                        int_params[key] = std::stoi(value);
                                    }
                                    else
                                    {
                                        throw std::logic_error("[J3X] Not handled type " + type +
                                                               " on line " + std::to_string(line_count) + "\n");
                                    }
                                }
                                catch (std::invalid_argument& e)
                                {
                                    throw std::invalid_argument("[J3X] Parse error: Wrong type " + type +
                                                                " of parameter on line " +
                                                                std::to_string(line_count) + "\n");
                                }
                            }
                        }
                        else
                        {
                            throw std::invalid_argument("[J3X] Parse error on line " +
                                                        std::to_string(line_count) + "\n");
                        }
                    }
                }
                std::cout << "[J3X] Parsing of " + filename + " successful!" << std::endl;
                config_file.close();
            }
            else
            {
                throw std::logic_error("[J3X] J3X " + filename + " file not found!\n");
            }

            return std::make_tuple(int_params, float_params, string_params);
        }

        inline J3XParameters parse(const std::string& filename)
        {
            return parse(filename, "");
        }

        inline int getInt(const J3XIParameters& int_params, const std::string& key)
        {
            auto it = int_params.find(key);
            if (it == int_params.end())
            {
                std::cerr << "[J3X] Param " << key << " not found!" << std::endl;
                return 0;
            }

            return it->second;
        }

        inline int getInt(const J3XParameters& params, const std::string& key)
        {
            return getInt(std::get<0>(params), key);
        }

        inline float getFloat(const J3XFParameters& float_params, const std::string& key)
        {
            auto it = float_params.find(key);
            if (it == float_params.end())
            {
                std::cerr << "[J3X] Param " << key << " not found!" << std::endl;
                return 0.0f;
            }

            return it->second;
        }

        inline float getFloat(const J3XParameters& params, const std::string& key)
        {
            return getFloat(std::get<1>(params), key);
        }

        inline const std::string& getString(const J3XSParameters& string_params, const std::string& key)
        {
            static const std::string ERROR_STRING;

            auto it = string_params.find(key);
            if (it == string_params.end())
            {
                std::cerr << "[J3X] Param " << key << " not found!" << std::endl;
                return ERROR_STRING;
            }

            return it->second;
        }

        inline const std::string& getString(const J3XParameters& params, const std::string& key)
        {
            return getString(std::get<2>(params), key);
        }

        inline void setInt(J3XIParameters& int_params, const std::string& key, int value)
        {
            int_params[key] = value;
        }

        inline void setFloat(J3XFParameters& float_params, const std::string& key, float value)
        {
            float_params[key] = value;
        }

        inline void setString(J3XSParameters& string_params, const std::string& key, const std::string& value)
        {
            string_params[key] = value;
        }

    } // namespace utils
} // namespace r3e

#endif // RAG3_ENGINE_UTILS_PARSER_H