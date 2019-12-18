//
// Created by jprolejko on 27.02.19.
//

#ifndef RAG3_ENGINE_SYSTEM_CONFIG_H
#define RAG3_ENGINE_SYSTEM_CONFIG_H

#include <map>
#include <iostream>

#include <engine/utils/Parser.h>


class Config {

public:
    Config(const Config&) = delete;

    Config& operator=(const Config&) = delete;

    static Config& getInstance()
    {
        static Config instance;
        return instance;
    }

    void initialize(const std::string& filename)
    {
        std::tie(int_params_, float_params_, string_params_) = utils::parse(filename);
    }

    int getInt(const std::string& key) const
    {
        return utils::getInt(int_params_, key);
    }

    float getFloat(const std::string& key) const
    {
        return utils::getFloat(float_params_, key);
    }

    const std::string& getString(const std::string& key) const
    {
        return utils::getString(string_params_, key);
    }

    void setInt(const std::string& key, int value)
    {
        utils::setInt(int_params_, key, value);
    }

    void setFloat(const std::string& key, float value)
    {
        utils::setFloat(float_params_, key, value);
    }

    void setString(const std::string& key, const std::string& value)
    {
        utils::setString(string_params_, key, value);
    }

private:
    Config() = default;

    utils::J3XIParameters int_params_;
    utils::J3XFParameters float_params_;
    utils::J3XSParameters string_params_;

};


#define CFG Config::getInstance()

#endif //RAG3_ENGINE_SYSTEM_CONFIG_H
