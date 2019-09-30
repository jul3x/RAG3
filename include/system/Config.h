//
// Created by jprolejko on 27.02.19.
//

#ifndef RAG3_SYSTEM_CONFIG_H
#define RAG3_SYSTEM_CONFIG_H

#include <map>
#include <iostream>
#include <sstream>
#include <fstream>


class Config {

public:
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    static Config& getInstance() {
        static Config instance;
        return instance;
    }

    void initialize(const std::string &filename) {
        std::ifstream config_file(filename);

        if (config_file)
        {
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
                                    int_params_[key] = std::stoi(value);
                                }
                                else if (type == "int32")
                                {
                                    int_params_[key] = std::stoll(value, 0, 16);
                                }
                                else if (type == "float")
                                {
                                    float_params_[key] = std::stof(value);
                                }
                                else if (type == "bool")
                                {
                                    int_params_[key] = std::stoi(value);
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

            config_file.close();
        }
        else
        {
            throw std::logic_error("Config file not found!");
        }
    }

    int getInt(const std::string &key) const {
        if (int_params_.find(key) == int_params_.end())
        {
            std::cerr << "[Config] Param " << key << " not found!" << std::endl;
            return 0;
        }

        return int_params_.at(key);
    }

    float getFloat(const std::string &key) const {
        if (float_params_.find(key) == float_params_.end())
        {
            std::cerr << "[Config] Param " << key << " not found!" << std::endl;
            return 0.0f;
        }

        return float_params_.at(key);
    }

    void setInt(const std::string &key, int value) {
        int_params_[key] = value;
    }

    void setFloat(const std::string &key, float value) {
        float_params_[key] = value;
    }

private:
    Config() = default;

    std::map<std::string, int> int_params_;
    std::map<std::string, float> float_params_;

};

#define CFG Config::getInstance()

#endif //RAG3_SYSTEM_CONFIG_H
