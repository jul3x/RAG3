//
// Created by jul3x on 31.03.20.
//

#ifndef RAG3_ENGINE_INCLUDE_R3E_OBJECTS_UNIQUE_H
#define RAG3_ENGINE_INCLUDE_R3E_OBJECTS_UNIQUE_H


namespace r3e {
    class Unique {
    public:
        explicit Unique(int id)
        {
            if (id < 0)
                u_id_ = ++Unique::current_max_id_;
            else
                u_id_ = id;

            if (id > Unique::current_max_id_)
                current_max_id_ = id;
        }

        [[nodiscard]] int getUniqueId() const
        {
            return u_id_;
        }

    private:
        inline static int current_max_id_ = 0;

        int u_id_;

    };
} // namespace r3e

#endif //RAG3_ENGINE_INCLUDE_R3E_OBJECTS_UNIQUE_H
