//
// Created by jul3x on 27.02.19.
//

#ifndef RAG3_ENGINE_INCLUDE_R3E_SYSTEM_CONFIG_H
#define RAG3_ENGINE_INCLUDE_R3E_SYSTEM_CONFIG_H

#include <map>
#include <iostream>

#include <R3E/utils/Parser.h>


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
            params_ = utils::j3x::parse(filename);
        }

        void appendConfig(const std::string& filename, const std::string& ns = "")
        {
            utils::j3x::Parameters new_params;

            new_params = utils::j3x::parse(filename, ns);

            utils::j3x::mergeParams(params_, new_params);
        }

        template <class T>
        const T& get(const std::string& key) const
        {
            return utils::j3x::get<T>(params_, key);
        }

        template <class T>
        void set(const std::string& key, const T& value)
        {
            utils::j3x::set<T>(params_, key, value);
        }

    private:
        Config() = default;

        utils::j3x::Parameters params_;

    };

#define CFG Config::getInstance()
} // namespace r3e

#endif //RAG3_ENGINE_INCLUDE_R3E_SYSTEM_CONFIG_H
