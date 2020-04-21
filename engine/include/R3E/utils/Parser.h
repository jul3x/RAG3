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
#include <vector>


namespace r3e {
    namespace utils {
        namespace j3x {
            using IParameters = std::unordered_map<std::string, int>;
            using FParameters = std::unordered_map<std::string, float>;
            using SParameters = std::unordered_map<std::string, std::string>;

            using IListParameters = std::unordered_map<std::string, std::vector<int>>;
            using FListParameters = std::unordered_map<std::string, std::vector<float>>;
            using SListParameters = std::unordered_map<std::string, std::vector<std::string>>;
            using Parameters = std::tuple<IParameters, FParameters, SParameters,
                                             IListParameters, FListParameters, SListParameters>;

            Parameters parse(const std::string& filename, const std::string& ns);

            Parameters parse(const std::string& filename);

            void tokenize(const std::string &str, char delimiter, std::vector<std::string>& out);

            int getInt(const IParameters& int_params, const std::string& key);

            int getInt(const Parameters& params, const std::string& key);

            float getFloat(const FParameters& float_params, const std::string& key);

            float getFloat(const Parameters& params, const std::string& key);

            const std::string& getString(const SParameters& string_params, const std::string& key);

            const std::string& getString(const Parameters& params, const std::string& key);

            const std::vector<int>& getListInt(const IListParameters& list_int_params, const std::string& key);

            const std::vector<int>& getListInt(const Parameters& params, const std::string& key);

            const std::vector<float>& getListFloat(const FListParameters& list_float_params, const std::string& key);

            const std::vector<float>& getListFloat(const Parameters& params, const std::string& key);

            const std::vector<std::string>& getListString(const SListParameters& list_string_params, const std::string& key);

            const std::vector<std::string>& getListString(const Parameters& params, const std::string& key);

            void setInt(IParameters& int_params, const std::string& key, int value);

            void setFloat(FParameters& float_params, const std::string& key, float value);

            void setString(SParameters& string_params, const std::string& key, const std::string& value);

            void setListInt(IListParameters& list_int_params, const std::string& key, const std::vector<int>& value);

            void setListFloat(FListParameters& list_float_params, const std::string& key, const std::vector<float>& value);

            void setListString(SListParameters& list_string_params, const std::string& key, const std::vector<std::string>& value);

        } // namespace j3x
    } // namespace utils
} // namespace r3e

#endif // RAG3_ENGINE_UTILS_PARSER_H