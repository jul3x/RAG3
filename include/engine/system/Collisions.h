//
// Created by jul3x on 25.01.2020.
//

#ifndef RAG3_ENGINE_SYSTEM_COLLISIONS_H
#define RAG3_ENGINE_SYSTEM_COLLISIONS_H

#include <list>
#include <iostream>

#include <engine/objects/AbstractPhysicalObject.h>
#include <engine/utils/Numeric.h>


class Collisions {
public:
    Collisions() = default;
    Collisions(const Collisions&) = delete;
    Collisions& operator=(const Collisions&) = delete;

    void initialize(const sf::Vector2f &size, float grid);

    void update();

    void insert(StaticObject* obj);
    void insert(DynamicObject* obj);
    void insert(HoveringObject* obj);

    void erase(StaticObject* obj);
    void erase(DynamicObject* obj);
    void erase(HoveringObject* obj);

    template<class T, class K>
    static bool ifCollideResponse(T &obj_1, K &obj_2)
    {
        const auto& a = obj_1.getCollisionArea().getType();
        const auto& b = obj_2.getCollisionArea().getType();

        if (a == Collision::Area::Type::None || b == Collision::Area::Type::None)
            return false;

        if (a == Collision::Area::Type::Circle)
        {
            if (b == Collision::Area::Type::Circle)
                return Collisions::circleCircleResponse(obj_1, obj_2);
            else if (b == Collision::Area::Type::Box)
                return Collisions::circleABResponse(obj_1, obj_2);
        }

        if (a == Collision::Area::Type::Box)
        {
            if (b == Collision::Area::Type::Circle)
                return Collisions::ABCircleResponse(obj_1, obj_2);
            else if (b == Collision::Area::Type::Box)
                return Collisions::AABBResponse(obj_1, obj_2);
        }

        throw std::invalid_argument("[Collisions] This collision types is not handled yet!");
    }

    template<class T, class K>
    static bool isCollision(const T &obj_1, const K &obj_2)
    {
        const auto& a = obj_1.getCollisionArea().getType();
        const auto& b = obj_2.getCollisionArea().getType();

        if (a == Collision::Area::Type::None || b == Collision::Area::Type::None)
            return false;

        if (a == Collision::Area::Type::Circle)
        {
            if (b == Collision::Area::Type::Circle)
                return Collisions::circleCircle(obj_1, obj_2);
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

        throw std::invalid_argument("[Collisions] This collision type is not handled yet!");
    }

private:
    void updateGridPosition(StaticObject* obj);

    template <class T>
    void insert(T* obj,std::vector<std::vector<std::list<T*>>> &cont) {
            cont.at(obj->grid_position_.x).at(obj->grid_position_.y).push_back(obj);
    }

    template <class T>
    void eraseIfExists(T* obj,std::vector<std::vector<std::list<T*>>> &cont) {
        auto& obj_list = cont.at(obj->grid_position_.x).at(obj->grid_position_.y);
        for (auto it = obj_list.begin(); it != obj_list.end(); ++it)
        {
            if (*it == obj)
            {
                obj_list.erase(it);
                break;
            }
        }
    }

    // Detection
    static short int AABB(const StaticObject& a, const StaticObject& b);

    static short int AABB(const DynamicObject& a, const StaticObject& b);

    static short int AABB(const DynamicObject& a, const DynamicObject& b);

    static bool circleCircle(const StaticObject &a, const StaticObject &b);

    static bool circleCircle(const DynamicObject &a, const StaticObject &b);

    static bool circleCircle(const DynamicObject &a, const DynamicObject &b);

    static short int ABCircle(const StaticObject& a, const StaticObject& b);

    static short int ABCircle(const DynamicObject& a, const StaticObject& b);

    static short int ABCircle(const StaticObject& a, const DynamicObject& b);

    static short int ABCircle(const DynamicObject& a, const DynamicObject& b);

    // Response
    static bool circleABResponse(DynamicObject &a, const StaticObject &b);

    static bool circleABResponse(DynamicObject &a, DynamicObject &b);

    static bool ABCircleResponse(DynamicObject& a, const StaticObject& b);

    static bool ABCircleResponse(DynamicObject& a, DynamicObject& b);

    static bool AABBResponse(DynamicObject& a, const StaticObject& b);

    static bool AABBResponse(DynamicObject& a, DynamicObject& b);

    static bool circleCircleResponse(DynamicObject &a, const StaticObject &b);

    static bool circleCircleResponse(DynamicObject &a, DynamicObject &b);

    static void blockNormalVelocity(DynamicObject& a, short int dir);

    std::vector<std::vector<std::list<StaticObject*>>> s_grid_;
    std::vector<std::vector<std::list<DynamicObject*>>> d_grid_;
    std::vector<std::vector<std::list<HoveringObject*>>> h_grid_;

    float grid_;
    size_t grid_size_x_, grid_size_y_;

};


#endif //RAG3_ENGINE_SYSTEM_COLLISIONS_H
