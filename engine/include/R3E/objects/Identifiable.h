//
// Created by jul3x on 18.03.20.
//

#ifndef RAG3_ENGINE_OBJECTS_IDENTIFIABLE_H
#define RAG3_ENGINE_OBJECTS_IDENTIFIABLE_H

#include <string>


namespace r3e {
    class Identifiable {
    public:
        Identifiable() : id_() {}

        Identifiable(const std::string &id) : id_(id) {}

        const std::string& getId() const
        {
            return id_;
        }

    private:
        std::string id_;

    };
} // namespace r3e

#endif //RAG3_ENGINE_OBJECTS_IDENTIFIABLE_H
