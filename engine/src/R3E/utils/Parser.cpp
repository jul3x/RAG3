//
// Created by jul3x on 21.04.20.
//

#include <r3e/utils/Parser.h>


namespace r3e {
    namespace utils {
        namespace j3x {

            Parameters parse(const std::string &filename, const std::string &ns)
            {
                std::ifstream config_file(filename);

                FParameters float_params;
                IParameters int_params;
                SParameters string_params;

                FListParameters list_float_params;
                IListParameters list_int_params;
                SListParameters list_string_params;

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

                                            tokenize(value, ',', values);
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

            void tokenize(const std::string &str, char delimiter, std::vector<std::string>& out)
            {
                size_t start;
                size_t end = 0;

                while ((start = str.find_first_not_of(delimiter, end)) != std::string::npos)
                {
                    end = str.find(delimiter, start);
                    out.push_back(str.substr(start, end - start));
                }
            }


            int getInt(const IParameters &int_params, const std::string &key)
            {
                auto it = int_params.find(key);
                if (it == int_params.end())
                {
                    std::cerr << "[J3X] Param " << key << " not found!" << std::endl;
                    return 0;
                }

                return it->second;
            }

            int getInt(const Parameters &params, const std::string &key)
            {
                return getInt(std::get<0>(params), key);
            }

            float getFloat(const FParameters &float_params, const std::string &key)
            {
                auto it = float_params.find(key);
                if (it == float_params.end())
                {
                    std::cerr << "[J3X] Param " << key << " not found!" << std::endl;
                    return 0.0f;
                }

                return it->second;
            }

            float getFloat(const Parameters &params, const std::string &key)
            {
                return getFloat(std::get<1>(params), key);
            }

            const std::string &getString(const SParameters &string_params, const std::string &key)
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

            const std::string &getString(const Parameters &params, const std::string &key)
            {
                return getString(std::get<2>(params), key);
            }

            const std::vector<int> &getListInt(const IListParameters &list_int_params, const std::string &key)
            {
                static std::vector<int> ERR = {};
                auto it = list_int_params.find(key);
                if (it == list_int_params.end())
                {
                    std::cerr << "[J3X] Param " << key << " not found!" << std::endl;
                    return ERR;
                }

                return it->second;
            }

            const std::vector<int> &getListInt(const Parameters &params, const std::string &key)
            {
                return getListInt(std::get<3>(params), key);
            }

            const std::vector<float> &
            getListFloat(const FListParameters &list_float_params, const std::string &key)
            {
                static std::vector<float> ERR = {};
                auto it = list_float_params.find(key);
                if (it == list_float_params.end())
                {
                    std::cerr << "[J3X] Param " << key << " not found!" << std::endl;
                    return ERR;
                }

                return it->second;
            }

            const std::vector<float> &getListFloat(const Parameters &params, const std::string &key)
            {
                return getListFloat(std::get<4>(params), key);
            }

            const std::vector<std::string> &
            getListString(const SListParameters &list_string_params, const std::string &key)
            {
                static const std::vector<std::string> ERR = {};

                auto it = list_string_params.find(key);
                if (it == list_string_params.end())
                {
                    std::cerr << "[J3X] Param " << key << " not found!" << std::endl;
                    return ERR;
                }

                return it->second;
            }

            const std::vector<std::string> &getListString(const Parameters &params, const std::string &key)
            {
                return getListString(std::get<5>(params), key);
            }

            void setInt(IParameters &int_params, const std::string &key, int value)
            {
                int_params[key] = value;
            }

            void setFloat(FParameters &float_params, const std::string &key, float value)
            {
                float_params[key] = value;
            }

            void setString(SParameters &string_params, const std::string &key, const std::string &value)
            {
                string_params[key] = value;
            }

            void
            setListInt(IListParameters &list_int_params, const std::string &key, const std::vector<int> &value)
            {
                list_int_params[key] = value;
            }

            void setListFloat(FListParameters &list_float_params, const std::string &key,
                              const std::vector<float> &value)
            {
                list_float_params[key] = value;
            }

            void setListString(SListParameters &list_string_params, const std::string &key,
                               const std::vector<std::string> &value)
            {
                list_string_params[key] = value;
            }
        } // namespace j3x
    } // namespace utils
} // namespace r3e
