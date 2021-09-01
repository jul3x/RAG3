//
// Created by jul3x on 15.08.21.
//

#ifndef RAG3_ENGINE_INCLUDE_R3E_J3X_HELPERS_H
#define RAG3_ENGINE_INCLUDE_R3E_J3X_HELPERS_H

#include <any>
#include <unordered_map>
#include <vector>


namespace r3e::j3x {
    constexpr char DELIMITER_ = ';';

    using Obj = std::any;
    using List = std::vector<Obj>;
    using Parameters = std::unordered_map<std::string, Obj>;

} // namespace r3e::j3x

#endif //RAG3_ENGINE_INCLUDE_R3E_J3X_HELPERS_H