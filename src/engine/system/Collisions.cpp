//
// Created by jul3x on 25.01.2020.
//

#include <engine/system/Collisions.h>
#include <engine/utils/Geometry.h>

bool Collisions::ifCollidableResponse(DynamicObject& d_obj, const StaticObject& s_obj)
{
    const auto& d = d_obj.getCollisionArea().getType();
    const auto& s = s_obj.getCollisionArea().getType();

    if (d == Collision::Area::Type::None || s == Collision::Area::Type::None)
    {
        return false;
    }

    if (d == Collision::Area::Type::Circle)
    {
        if (s == Collision::Area::Type::Circle)
        {
            return utils::CircleCircleResponse(d_obj, s_obj);
        }
        else if (s == Collision::Area::Type::Box)
        {
            return utils::CircleABResponse(d_obj, s_obj);
        }
    }

    if (d == Collision::Area::Type::Box)
    {
        if (s == Collision::Area::Type::Circle)
        {
            return utils::ABCircleResponse(d_obj, s_obj);
        }
        else if (s == Collision::Area::Type::Box)
        {
            return utils::AABBResponse(d_obj, s_obj);
        }
    }

    throw std::invalid_argument("[Engine] This collision types is not handled yet!");
}

bool Collisions::ifCollidableResponse(DynamicObject& d_obj, DynamicObject& s_obj)
{
    const auto& d = d_obj.getCollisionArea().getType();
    const auto& s = s_obj.getCollisionArea().getType();

    if (d == Collision::Area::Type::None || s == Collision::Area::Type::None)
    {
        return false;
    }

    if (d == Collision::Area::Type::Circle)
    {
        if (s == Collision::Area::Type::Circle)
        {
            return utils::CircleCircleResponse(d_obj, s_obj);
        }
        else if (s == Collision::Area::Type::Box)
        {
            return utils::CircleABResponse(d_obj, s_obj);
        }
    }

    if (d == Collision::Area::Type::Box)
    {
        if (s == Collision::Area::Type::Circle)
        {
            return utils::ABCircleResponse(d_obj, s_obj);
        }
        else if (s == Collision::Area::Type::Box)
        {
            return utils::AABBResponse(d_obj, s_obj);
        }
    }

    throw std::invalid_argument("[Engine] This collision types is not handled yet!");
}

bool Collisions::areCollidable(const StaticObject& obj_1, const StaticObject& obj_2)
{
    const auto& a = obj_1.getCollisionArea().getType();
    const auto& b = obj_2.getCollisionArea().getType();

    if (a == Collision::Area::Type::None || b == Collision::Area::Type::None)
    {
        return false;
    }

    if (a == Collision::Area::Type::Circle)
    {
        if (b == Collision::Area::Type::Circle)
        {
            return utils::CircleCircle(obj_1, obj_2);
        }
        else if (b == Collision::Area::Type::Box)
        {
            return utils::ABCircle(obj_2, obj_1);
        }
    }

    if (a == Collision::Area::Type::Box)
    {
        if (b == Collision::Area::Type::Circle)
        {
            return utils::ABCircle(obj_1, obj_2);
        }
        else if (b == Collision::Area::Type::Box)
        {
            return utils::AABB(obj_1, obj_2);
        }
    }

    throw std::invalid_argument("[Engine] This collision type is not handled yet!");
}

bool Collisions::areCollidable(const DynamicObject& obj_1, const StaticObject& obj_2)
{
    const auto& a = obj_1.getCollisionArea().getType();
    const auto& b = obj_2.getCollisionArea().getType();

    if (a == Collision::Area::Type::None || b == Collision::Area::Type::None)
    {
        return false;
    }

    if (a == Collision::Area::Type::Circle)
    {
        if (b == Collision::Area::Type::Circle)
        {
            return utils::CircleCircle(obj_1, obj_2);
        }
        else if (b == Collision::Area::Type::Box)
        {
            return utils::ABCircle(obj_2, obj_1);
        }
    }

    if (a == Collision::Area::Type::Box)
    {
        if (b == Collision::Area::Type::Circle)
        {
            return utils::ABCircle(obj_1, obj_2);
        }
        else if (b == Collision::Area::Type::Box)
        {
            return utils::AABB(obj_1, obj_2);
        }
    }

    throw std::invalid_argument("[Engine] This collision type is not handled yet!");
}

bool Collisions::areCollidable(const DynamicObject& obj_1, const DynamicObject& obj_2)
{
    const auto& a = obj_1.getCollisionArea().getType();
    const auto& b = obj_2.getCollisionArea().getType();

    if (a == Collision::Area::Type::None || b == Collision::Area::Type::None)
    {
        return false;
    }

    if (a == Collision::Area::Type::Circle)
    {
        if (b == Collision::Area::Type::Circle)
        {
            return utils::CircleCircle(obj_1, obj_2);
        }
        else if (b == Collision::Area::Type::Box)
        {
            return utils::ABCircle(obj_2, obj_1);
        }
    }

    if (a == Collision::Area::Type::Box)
    {
        if (b == Collision::Area::Type::Circle)
        {
            return utils::ABCircle(obj_1, obj_2);
        }
        else if (b == Collision::Area::Type::Box)
        {
            return utils::AABB(obj_1, obj_2);
        }
    }

    throw std::invalid_argument("[Engine] This collision type is not handled yet!");
}
