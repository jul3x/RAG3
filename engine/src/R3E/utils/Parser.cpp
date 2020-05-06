//
// Created by jul3x on 21.04.20.
//

#include <utility>

#include <R3E/utils/Parser.h>


namespace r3e {
    namespace utils {
        namespace j3x {

            Parameters parse(const std::string &filename, const std::string &ns)
            {
                std::ifstream config_file(filename);

                Params<float> float_params;
                Params<int> int_params;
                Params<std::string> string_params;

                Params<std::vector<float>> list_float_params;
                Params<std::vector<int>> list_int_params;
                Params<std::vector<std::string>> list_string_params;

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
                                    try {
                                        std::vector<std::string> values;
                                        bool vectorized = false;
                                        if (type.substr(0, 5) == "list[") {
                                            if (value.length() < 3 || type.length() < 7) {
                                                throw std::invalid_argument("");
                                            }

                                            type = type.substr(5, type.length() - 1);
                                            type.pop_back();

                                            value = value.substr(1, value.length() - 2);

                                            tokenize(value, utils::j3x::DELIMITER_, values);
                                            vectorized = true;
                                        } else {
                                            vectorized = false;
                                            values.emplace_back(value);
                                        }

                                        for (const auto &curr_value : values)
                                        {
                                            if (type == "int" || type == "bool")
                                            {
                                                if (vectorized)
                                                    list_int_params[key].emplace_back(std::stoi(curr_value));
                                                else
                                                    int_params[key] = std::stoi(curr_value);
                                            }
                                            else if (type == "int32")
                                            {
                                                if (vectorized)
                                                    list_int_params[key].emplace_back(std::stoll(curr_value, 0, 16));
                                                else
                                                    int_params[key] = std::stoll(curr_value, 0, 16);
                                            }
                                            else if (type == "float")
                                            {
                                                if (vectorized)
                                                    list_float_params[key].emplace_back(std::stof(curr_value));
                                                else
                                                    float_params[key] = std::stof(curr_value);
                                            }
                                            else if (type == "string")
                                            {
                                                if (vectorized)
                                                    list_string_params[key].emplace_back(curr_value);
                                                else
                                                    string_params[key] = curr_value;
                                            }
                                            else
                                            {
                                                throw std::logic_error("[J3X] Not handled type " + type +
                                                                       " on line " + std::to_string(line_count) + "\n");
                                            }
                                        }
                                    }
                                    catch (std::invalid_argument &e)
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

                return std::make_tuple(int_params, float_params, string_params,
                                       list_int_params, list_float_params, list_string_params);
            }

            Parameters parse(const std::string &filename)
            {
                return parse(filename, "");
            }

            void mergeParams(Parameters& params, const Parameters& new_params)
            {
                std::get<0>(params).insert(std::get<0>(new_params).begin(), std::get<0>(new_params).end());
                std::get<1>(params).insert(std::get<1>(new_params).begin(), std::get<1>(new_params).end());
                std::get<2>(params).insert(std::get<2>(new_params).begin(), std::get<2>(new_params).end());
                std::get<3>(params).insert(std::get<3>(new_params).begin(), std::get<3>(new_params).end());
                std::get<4>(params).insert(std::get<4>(new_params).begin(), std::get<4>(new_params).end());
                std::get<5>(params).insert(std::get<5>(new_params).begin(), std::get<5>(new_params).end());
            }

            void tokenize(const std::string &str, char delimiter, std::vector<std::string>& out)
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

        } // namespace j3x
    } // namespace utils
} // namespace r3e
