//
// Created by jul3x on 25.01.2020.
//

#ifndef RAG3_ENGINE_INCLUDE_R3E_SYSTEM_COLLISIONS_H
#define RAG3_ENGINE_INCLUDE_R3E_SYSTEM_COLLISIONS_H

#include <list>
#include <iostream>

#include <R3E/objects/AbstractPhysicalObject.h>
#include <R3E/utils/Numeric.h>
#include <R3E/system/AbstractGame.h>


namespace r3e {
    class Collisions {
    public:
        Collisions() = default;
        Collisions(const Collisions&) = delete;
        Collisions& operator=(const Collisions&) = delete;

        void initialize(const sf::Vector2f &size, float grid);

        void update(AbstractGame *game);

        void insert(StaticObject* obj);
        void insert(DynamicObject* obj);
        void insert(HoveringObject* obj);

        void erase(StaticObject* obj);
        void erase(DynamicObject* obj);
        void erase(HoveringObject* obj);

    private:
        inline void updateGridPosition(StaticObject* obj);

        template <class T>
        inline void updateGrid(std::vector<std::vector<std::list<T*>>> &cont) {
            for (size_t i = 0; i < grid_size_x_; ++i)
            {
                auto& col = cont.at(i);

                for (size_t j = 0; j < grid_size_y_; ++j)
                {
                    for (auto it = col.at(j).begin(); it != col.at(j).end();)
                    {
                        bool do_increment = true;
                        auto old_grid_pos = (*it)->grid_position_;
                        updateGridPosition(*it);

                        if (old_grid_pos != (*it)->grid_position_)
                        {
                            auto next_it = std::next(it);
                            insert(*it, cont);
                            col.at(j).erase(it);
                            it = next_it;
                            do_increment = false;
                        }

                        if (do_increment) ++it;
                    }
                }
            }
        }

        template <class T>
        inline void insert(T* obj,std::vector<std::vector<std::list<T*>>> &cont) {
                cont.at(obj->grid_position_.x).at(obj->grid_position_.y).push_back(obj);
        }

        template <class T>
        inline void eraseIfExists(T* obj,std::vector<std::vector<std::list<T*>>> &cont) {
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

        template<class T, class K>
        inline static bool ifCollideRespond(T &obj_1, K &obj_2)
        {
            const auto& a = obj_1.getCollisionArea().getType();
            const auto& b = obj_2.getCollisionArea().getType();

            if (a == collision::Area::Type::None || b == collision::Area::Type::None)
                return false;

            if (a == collision::Area::Type::Circle)
            {
                if (b == collision::Area::Type::Circle)
                    return Collisions::circleCircleResponse(obj_1, obj_2);
                else if (b == collision::Area::Type::Box)
                    return Collisions::circleABResponse(obj_1, obj_2);
            }

            if (a == collision::Area::Type::Box)
            {
                if (b == collision::Area::Type::Circle)
                    return Collisions::ABCircleResponse(obj_1, obj_2);
                else if (b == collision::Area::Type::Box)
                    return Collisions::AABBResponse(obj_1, obj_2);
            }

            throw std::invalid_argument("[Collisions - Response] This collision type (" +
                    std::to_string(static_cast<int>(a)) + "," + std::to_string(static_cast<int>(b)) + ") is not handled yet!");
        }

        template<class T, class K>
        inline static bool isCollision(const T &obj_1, const K &obj_2)
        {
            const auto& a = obj_1.getCollisionArea().getType();
            const auto& b = obj_2.getCollisionArea().getType();

            if (a == collision::Area::Type::None || b == collision::Area::Type::None)
                return false;

            if (a == collision::Area::Type::Circle)
            {
                if (b == collision::Area::Type::Circle)
                    return Collisions::circleCircle(obj_1, obj_2);
                else if (b == collision::Area::Type::Box)
                    return static_cast<bool>(Collisions::ABCircle(obj_2, obj_1));
            }

            if (a == collision::Area::Type::Box)
            {
                if (b == collision::Area::Type::Circle)
                    return static_cast<bool>(Collisions::ABCircle(obj_1, obj_2));
                else if (b == collision::Area::Type::Box)
                    return static_cast<bool>(Collisions::AABB(obj_1, obj_2));
            }

            throw std::invalid_argument("[Collisions] This collision type (" +
                    std::to_string(static_cast<int>(a)) + "," + std::to_string(static_cast<int>(b)) + ") is not handled yet!");
        }

        template <class T, class K>
        inline void checkCollisions(AbstractGame *game,
                             std::vector<std::vector<std::list<T*>>> &cont_1,
                             std::vector<std::vector<std::list<K*>>> &cont_2,
                             bool respond = false)
        {
            static std::vector<sf::Vector2<int>> grids_to_check =
                    {{-1, -1}, {-1, 1}, {1, -1}, {0, -1}, {0, 1}, {0, 0}, {1, 0}, {-1, 0}, {1, 1}};
            for (size_t i = 0; i < grid_size_x_; ++i)
            {
                for (size_t j = 0; j < grid_size_y_; ++j)
                {
                    for (const auto &grid : grids_to_check)
                    {
                        size_t x = grid.x + i;
                        size_t y = grid.y + j;

                        if (x < 0 || x >= grid_size_x_ || y < 0 || y >= grid_size_y_) continue;

                        for (const auto &el1 : cont_1.at(i).at(j))
                        {
                            for (const auto &el2 : cont_2.at(x).at(y))
                            {
                                if (x == i && y == j && el1 == el2) continue;

                                if ((respond && Collisions::ifCollideRespond(*el1, *el2)) ||
                                    Collisions::isCollision(*el1, *el2))
                                {
                                    game->alertCollision(el1, el2);
                                }
                            }
                        }
                    }
                }
            }
        }

        // Detection
        inline static short int AABB(const StaticObject& a, const StaticObject& b);
        inline static bool circleCircle(const StaticObject &a, const StaticObject &b);
        inline static short int ABCircle(const StaticObject& a, const StaticObject& b);

        // Response
        inline static bool circleABResponse(DynamicObject &a, const StaticObject &b);
        inline static bool circleABResponse(DynamicObject &a, DynamicObject &b);

        inline static bool ABCircleResponse(DynamicObject& a, const StaticObject& b);
        inline static bool ABCircleResponse(DynamicObject& a, DynamicObject& b);

        inline static bool AABBResponse(DynamicObject& a, const StaticObject& b);
        inline static bool AABBResponse(DynamicObject& a, DynamicObject& b);

        inline static bool circleCircleResponse(DynamicObject &a, const StaticObject &b);
        inline static bool circleCircleResponse(DynamicObject &a, DynamicObject &b);

        inline static void blockNormalVelocity(DynamicObject& a, short int dir);

        inline static void setVerifiedPosition(StaticObject &a, const sf::Vector2f &pos);
        inline static void setVerifiedPositionX(StaticObject &a, float x);
        inline static void setVerifiedPositionY(StaticObject &a, float y);

        std::vector<std::vector<std::list<StaticObject*>>> s_grid_;
        std::vector<std::vector<std::list<DynamicObject*>>> d_grid_;
        std::vector<std::vector<std::list<HoveringObject*>>> h_grid_;

        float grid_;
        size_t grid_size_x_, grid_size_y_;

    };

} // namespace r3e

#endif //RAG3_ENGINE_INCLUDE_R3E_SYSTEM_COLLISIONS_H
