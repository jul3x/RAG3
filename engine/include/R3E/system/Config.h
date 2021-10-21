//
// Created by jul3x on 27.02.19.
//

#ifndef RAG3_ENGINE_INCLUDE_R3E_SYSTEM_CONFIG_H
#define RAG3_ENGINE_INCLUDE_R3E_SYSTEM_CONFIG_H

#include <map>
#include <iostream>

#include <R3E/j3x/J3X.h>


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
            params_ = j3x::parse(filename);
        }

        void appendConfig(const std::string& filename, const std::string& ns = "", bool force_update = false)
        {
            auto new_params = j3x::parse(filename, ns);

            j3x::mergeParams(*params_, *new_params, force_update);
        }

        template<class T>
        const T& get(const std::string& key, bool ignore_warn = false) const
        {
            return j3x::get<T>(*params_, key, ignore_warn);
        }

        template<class T>
        void set(const std::string& key, const T& value)
        {
            j3x::set<T>(*params_, key, value);
        }

        j3x::Parameters& getParameters()
        {
            return *params_;
        }

        const j3x::Parameters& getParameters() const
        {
            return *params_;
        }

    private:
        Config() = default;

        std::shared_ptr<j3x::Parameters> params_;

    };

#define CFG Config::getInstance()
#define CONF CFG.get

} // namespace r3e

#endif //RAG3_ENGINE_INCLUDE_R3E_SYSTEM_CONFIG_H
