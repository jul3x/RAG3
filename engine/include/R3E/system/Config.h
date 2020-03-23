//
// Created by jul3x on 27.02.19.
//

#ifndef RAG3_ENGINE_SYSTEM_CONFIG_H
#define RAG3_ENGINE_SYSTEM_CONFIG_H

#include <map>
#include <iostream>

#include "R3E/utils/Parser.h"


namespace r3e {
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

        void appendConfig(const std::string& filename, std::string ns = "")
        {
            utils::J3XIParameters new_int_params;
            utils::J3XFParameters new_float_params;
            utils::J3XSParameters new_string_params;

            std::tie(new_int_params, new_float_params, new_string_params) = utils::parse(filename, std::move(ns));

            int_params_.insert(new_int_params.begin(), new_int_params.end());
            float_params_.insert(new_float_params.begin(), new_float_params.end());
            string_params_.insert(new_string_params.begin(), new_string_params.end());
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
} // namespace r3e

#endif //RAG3_ENGINE_SYSTEM_CONFIG_H
