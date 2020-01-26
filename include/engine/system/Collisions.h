//
// Created by jul3x on 25.01.2020.
//

#ifndef RAG3_ENGINE_SYSTEM_COLLISIONS_H
#define RAG3_ENGINE_SYSTEM_COLLISIONS_H

#include <engine/objects/AbstractPhysicalObject.h>


class Collisions {
public:
    Collisions() = delete;
    Collisions(const Collisions&) = delete;
    Collisions& operator=(const Collisions&) = delete;

    template<class T, class K>
    static bool ifCollidableResponse(T& obj_1, K& obj_2)
    {
        const auto& a = obj_1.getCollisionArea().getType();
        const auto& b = obj_2.getCollisionArea().getType();

        if (a == Collision::Area::Type::None || b == Collision::Area::Type::None)
            return false;

        if (a == Collision::Area::Type::Circle)
        {
            if (b == Collision::Area::Type::Circle)
                return Collisions::CircleCircleResponse(obj_1, obj_2);
            else if (b == Collision::Area::Type::Box)
                return Collisions::CircleABResponse(obj_1, obj_2);
        }

        if (a == Collision::Area::Type::Box)
        {
            if (b == Collision::Area::Type::Circle)
                return Collisions::ABCircleResponse(obj_1, obj_2);
            else if (b == Collision::Area::Type::Box)
                return Collisions::AABBResponse(obj_1, obj_2);
        }

        throw std::invalid_argument("[Engine] This collision types is not handled yet!");
    }

    template<class T, class K>
    static bool areCollidable(const T& obj_1, const K& obj_2)
    {
        const auto& a = obj_1.getCollisionArea().getType();
        const auto& b = obj_2.getCollisionArea().getType();

        if (a == Collision::Area::Type::None || b == Collision::Area::Type::None)
            return false;

        if (a == Collision::Area::Type::Circle)
        {
            if (b == Collision::Area::Type::Circle)
                return Collisions::CircleCircle(obj_1, obj_2);
            else if (b == Collision::Area::Type::Box)
                return static_cast<bool>(Collisions::ABCircle(obj_2, obj_1));
        }

        if (a == Collision::Area::Type::Box)
        {
            if (b == Collision::Area::Type::Circle)
                return static_cast<bool>(Collisions::ABCircle(obj_1, obj_2));
            else if (b == Collision::Area::Type::Box)
                return static_cast<bool>(Collisions::AABB(obj_1, obj_2));
        }

        throw std::invalid_argument("[Engine] This collision type is not handled yet!");
    }

private:
    // Detection
    static short int AABB(const StaticObject& a, const StaticObject& b);

    static short int AABB(const DynamicObject& a, const StaticObject& b);

    static short int AABB(const DynamicObject& a, const DynamicObject& b);

    static bool CircleCircle(const StaticObject& a, const StaticObject& b);

    static bool CircleCircle(const DynamicObject& a, const StaticObject& b);

    static bool CircleCircle(const DynamicObject& a, const DynamicObject& b);

    static short int ABCircle(const StaticObject& a, const StaticObject& b);

    static short int ABCircle(const DynamicObject& a, const StaticObject& b);

    static short int ABCircle(const StaticObject& a, const DynamicObject& b);

    static short int ABCircle(const DynamicObject& a, const DynamicObject& b);

    // Response
    static bool CircleABResponse(DynamicObject& a, const StaticObject& b);

    static bool CircleABResponse(DynamicObject& a, DynamicObject& b);

    static bool ABCircleResponse(DynamicObject& a, const StaticObject& b);

    static bool ABCircleResponse(DynamicObject& a, DynamicObject& b);

    static bool AABBResponse(DynamicObject& a, const StaticObject& b);

    static bool AABBResponse(DynamicObject& a, DynamicObject& b);

    static bool CircleCircleResponse(DynamicObject& a, const StaticObject& b);

    static bool CircleCircleResponse(DynamicObject& a, DynamicObject& b);

};


#endif //RAG3_ENGINE_SYSTEM_COLLISIONS_H
