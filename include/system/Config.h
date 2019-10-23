//
// Created by jprolejko on 27.02.19.
//

#ifndef RAG3_SYSTEM_CONFIG_H
#define RAG3_SYSTEM_CONFIG_H

#include <map>
#include <iostream>

#include <utils/Parser.h>


class Config {

public:
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    static Config& getInstance() {
        static Config instance;
        return instance;
    }

    void initialize(const std::string &filename) {
        std::tie(int_params_, float_params_) = utils::parse(filename);
    }

    int getInt(const std::string &key) const {
        return utils::getInt(int_params_, key);
    }

    float getFloat(const std::string &key) const {
        return utils::getFloat(float_params_, key);
    }

    void setInt(const std::string &key, int value) {
        utils::setInt(int_params_, key, value);
    }

    void setFloat(const std::string &key, float value) {
        utils::setFloat(float_params_, key, value);
    }

private:
    Config() = default;

    std::map<std::string, int> int_params_;
    std::map<std::string, float> float_params_;

};


#define CFG Config::getInstance()

#endif //RAG3_SYSTEM_CONFIG_H
