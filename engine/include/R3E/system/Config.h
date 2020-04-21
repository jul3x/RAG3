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
            std::tie(int_params_, float_params_, string_params_,
                     list_int_params_, list_float_params_, list_string_params_) = utils::j3x::parse(filename);
        }

        void appendConfig(const std::string& filename, std::string ns = "")
        {
            utils::j3x::IParameters new_int_params;
            utils::j3x::FParameters new_float_params;
            utils::j3x::SParameters new_string_params;

            utils::j3x::IListParameters new_list_int_params;
            utils::j3x::FListParameters new_list_float_params;
            utils::j3x::SListParameters new_list_string_params;

            std::tie(new_int_params, new_float_params, new_string_params,
                     new_list_int_params, new_list_float_params, new_list_string_params) = utils::j3x::parse(filename, std::move(ns));

            int_params_.insert(new_int_params.begin(), new_int_params.end());
            float_params_.insert(new_float_params.begin(), new_float_params.end());
            string_params_.insert(new_string_params.begin(), new_string_params.end());

            list_int_params_.insert(new_list_int_params.begin(), new_list_int_params.end());
            list_float_params_.insert(new_list_float_params.begin(), new_list_float_params.end());
            list_string_params_.insert(new_list_string_params.begin(), new_list_string_params.end());
        }

        int getInt(const std::string& key) const
        {
            return utils::j3x::getInt(int_params_, key);
        }

        float getFloat(const std::string& key) const
        {
            return utils::j3x::getFloat(float_params_, key);
        }

        const std::string& getString(const std::string& key) const
        {
            return utils::j3x::getString(string_params_, key);
        }

        const std::vector<int>& getListInt(const std::string& key) const
        {
            return utils::j3x::getListInt(list_int_params_, key);
        }

        const std::vector<float>& getListFloat(const std::string& key) const
        {
            return utils::j3x::getListFloat(list_float_params_, key);
        }

        const std::vector<std::string>& getListString(const std::string& key) const
        {
            return utils::j3x::getListString(list_string_params_, key);
        }

        void setInt(const std::string& key, int value)
        {
            utils::j3x::setInt(int_params_, key, value);
        }

        void setFloat(const std::string& key, float value)
        {
            utils::j3x::setFloat(float_params_, key, value);
        }

        void setString(const std::string& key, const std::string& value)
        {
            utils::j3x::setString(string_params_, key, value);
        }

        void setListInt(const std::string& key, const std::vector<int>& value)
        {
            utils::j3x::setListInt(list_int_params_, key, value);
        }

        void setListFloat(const std::string& key, const std::vector<float>& value)
        {
            utils::j3x::setListFloat(list_float_params_, key, value);
        }

        void setListString(const std::string& key, const std::vector<std::string>& value)
        {
            utils::j3x::setListString(list_string_params_, key, value);
        }

    private:
        Config() = default;

        utils::j3x::IParameters int_params_;
        utils::j3x::FParameters float_params_;
        utils::j3x::SParameters string_params_;

        utils::j3x::IListParameters list_int_params_;
        utils::j3x::FListParameters list_float_params_;
        utils::j3x::SListParameters list_string_params_;

    };

#define CFG Config::getInstance()
} // namespace r3e

#endif //RAG3_ENGINE_SYSTEM_CONFIG_H
