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

        void initialize(const sf::Vector2f& size, float grid);

        void setWindowedCollisionCheck(AbstractPhysicalObject* obj, float bounds);

        void update(AbstractGame* game);

        void insert(StaticObject* obj);
        void insert(DynamicObject* obj);
        void insert(HoveringObject* obj);

        void erase(StaticObject* obj);
        void erase(DynamicObject* obj);
        void erase(HoveringObject* obj);

    private:
        inline void updateGridPosition(StaticObject* obj);

        template<class T>
        inline void updateGrid(std::vector<std::vector<std::list<T*>>>& cont)
        {
            for (size_t i = 0; i < grid_size_x_; ++i)
            {
                auto& col = cont[i];

                for (size_t j = 0; j < grid_size_y_; ++j)
                {
                    for (auto it = col[j].begin(); it != col[j].end();)
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

                        if (do_increment)
                            ++it;
                    }
                }
            }
        }

        template<class T>
        inline void insert(T* obj, std::vector<std::vector<std::list<T*>>>& cont)
        {
            cont[obj->grid_position_.x][obj->grid_position_.y].emplace_back(obj);
        }

        template<class T>
        inline void eraseIfExists(T* obj, std::vector<std::vector<std::list<T*>>>& cont)
        {
            auto& obj_list = cont[obj->grid_position_.x][obj->grid_position_.y];
            for (auto it = obj_list.begin(); it != obj_list.end(); ++it)
            {
                if (*it == obj)
                {
                    obj_list.erase(it);
                    return;
                }
            }
        }

        template<class T, class K>
        inline static bool ifCollideRespond(T& obj_1, K& obj_2)
        {
            const auto& a = obj_1.getCollisionArea().getType();
            const auto& b = obj_2.getCollisionArea().getType();

            if (unlikely(a == collision::Area::Type::None || b == collision::Area::Type::None))
                return false;

            if (a == collision::Area::Type::Circle)
            {
                if (b == collision::Area::Type::Circle)
                    return Collisions::circleCircleResponse(obj_1, obj_2);

                // Circle - Box response
                return Collisions::circleABResponse(obj_1, obj_2);
            }

            // Box - Circle or Box - Box response
            if (b == collision::Area::Type::Circle)
                return Collisions::ABCircleResponse(obj_1, obj_2);
            return Collisions::AABBResponse(obj_1, obj_2);
        }

        template<class T, class K>
        inline static bool isCollision(T& obj_1, K& obj_2)
        {
            const auto& a = obj_1.getCollisionArea().getType();
            const auto& b = obj_2.getCollisionArea().getType();

            if (unlikely(a == collision::Area::Type::None || b == collision::Area::Type::None))
                return false;

            if (a == collision::Area::Type::Circle)
            {
                if (b == collision::Area::Type::Circle)
                    return Collisions::circleCircle(obj_1, obj_2);

                // Circle - Box
                return static_cast<bool>(Collisions::ABCircle(obj_2, obj_1));
            }

            // Box - Circle or Box - Box
            if (b == collision::Area::Type::Circle)
                return static_cast<bool>(Collisions::ABCircle(obj_1, obj_2));
            return static_cast<bool>(Collisions::AABB(obj_1, obj_2));
        }

        template<class T, class K>
        inline void checkCollisions(AbstractGame* game,
                                    std::vector<std::vector<std::list<T*>>>& cont_1,
                                    std::vector<std::vector<std::list<K*>>>& cont_2,
                                    bool respond = false)
        {
            std::function<bool(T&, K&)> collision_func = Collisions::isCollision<T, K>;
            if (respond)
                collision_func = Collisions::ifCollideRespond<T, K>; 

            static constexpr std::array<std::pair<int, int>, 9> grids_to_check =
                    {std::make_pair(-1, -1),
                     std::make_pair(-1, 1),
                     std::make_pair(1,  -1),
                     std::make_pair(0,  -1),
                     std::make_pair(0,  1),
                     std::make_pair(0,  0),
                     std::make_pair(1,  0),
                     std::make_pair(-1, 0),
                     std::make_pair(1,  1)};

            size_t grid_x_min, grid_x_max, grid_y_min, grid_y_max;

            if (following_obj_ != nullptr)
            {
                grid_x_min = std::max(0, static_cast<int>((following_obj_->getPosition().x - bounds_) / grid_));
                grid_x_max = std::min(static_cast<int>(grid_size_x_),
                                      static_cast<int>((following_obj_->getPosition().x + bounds_) / grid_));
                grid_y_min = std::max(0, static_cast<int>((following_obj_->getPosition().y - bounds_) / grid_));
                grid_y_max = std::min(static_cast<int>(grid_size_y_),
                                      static_cast<int>((following_obj_->getPosition().y + bounds_) / grid_));
            }
            else
            {
                grid_x_min = 0;
                grid_x_max = grid_size_x_;
                grid_y_min = 0;
                grid_y_max = grid_size_y_;
            }            
            
            for (size_t i = grid_x_min; i < grid_x_max; ++i)
            {
                for (size_t j = grid_y_min; j < grid_y_max; ++j)
                {
                    for (const auto& grid : grids_to_check)
                    {
                        size_t x = grid.first + i;
                        size_t y = grid.second + j;

                        if (x < 0 || x >= grid_size_x_ || y < 0 || y >= grid_size_y_)
                            continue;

                        for (const auto& el1 : cont_1[i][j])
                        {
                            for (const auto& el2 : cont_2[x][y])
                            {
                                if (el1 == el2)
                                    continue;

                                if (collision_func(*el1, *el2))
                                    game->alertCollision(el1, el2);
                            }
                        }
                    }
                }
            }
        }

        // Detection
        inline static short int AABB(const StaticObject& a, const StaticObject& b);
        inline static bool circleCircle(const StaticObject& a, const StaticObject& b);
        inline static short int ABCircle(const StaticObject& a, const StaticObject& b);

        // Response
        inline static bool circleABResponse(DynamicObject& a, const StaticObject& b);
        inline static bool circleABResponse(DynamicObject& a, DynamicObject& b);

        inline static bool ABCircleResponse(DynamicObject& a, const StaticObject& b);
        inline static bool ABCircleResponse(DynamicObject& a, DynamicObject& b);

        inline static bool AABBResponse(DynamicObject& a, const StaticObject& b);
        inline static bool AABBResponse(DynamicObject& a, DynamicObject& b);

        inline static bool circleCircleResponse(DynamicObject& a, const StaticObject& b);
        inline static bool circleCircleResponse(DynamicObject& a, DynamicObject& b);

        inline static void blockNormalVelocity(DynamicObject& a, short int dir);

        inline static void setVerifiedPosition(StaticObject& a, const sf::Vector2f& pos);
        inline static void setVerifiedPositionX(StaticObject& a, float x);
        inline static void setVerifiedPositionY(StaticObject& a, float y);

        std::vector<std::vector<std::list<StaticObject*>>> s_grid_;
        std::vector<std::vector<std::list<DynamicObject*>>> d_grid_;
        std::vector<std::vector<std::list<HoveringObject*>>> h_grid_;

        float grid_;
        size_t grid_size_x_, grid_size_y_;

        AbstractPhysicalObject* following_obj_;
        float bounds_;

    };

} // namespace r3e

#endif //RAG3_ENGINE_INCLUDE_R3E_SYSTEM_COLLISIONS_H
