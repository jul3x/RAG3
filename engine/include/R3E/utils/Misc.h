//
// Created by jul3x on 17.11.2020.
//

#ifndef RAG3_ENGINE_INCLUDE_R3E_UTILS_MISC_H
#define RAG3_ENGINE_INCLUDE_R3E_UTILS_MISC_H

#include <chrono>
#include <list>
#include <functional>


namespace r3e::utils {
    template<class T>
    void eraseIf(std::list<T>& elements, const std::function<bool(T&)>& condition)
    {
        for (auto it = elements.begin(); it != elements.end();)
        {
            bool do_increment = true;

            if (condition(*it))
            {
                auto next_it = std::next(it);
                elements.erase(it);
                it = next_it;
                do_increment = false;
            }
            if (do_increment)
                ++it;
        }
    }

    uint64_t timeSinceEpochMillisec();

    std::string capitalFirst(const std::string& word);
}

#endif //RAG3_ENGINE_INCLUDE_R3E_UTILS_MISC_H
