//
// Created by jul3x on 25.01.2020.
//

#include <engine/system/Collisions.h>
#include <engine/utils/Geometry.h>
#include <cmath>

void Collisions::initialize(const sf::Vector2f &size, float grid)
{
    grid_ = grid;
    grid_size_x_ = static_cast<size_t>(std::ceil(size.x / grid));
    grid_size_y_ = static_cast<size_t>(std::ceil(size.y / grid));

    s_grid_.resize(grid_size_x_);
    d_grid_.resize(grid_size_x_);
    h_grid_.resize(grid_size_x_);

    for (size_t i = 0; i < grid_size_x_; ++i)
    {
        s_grid_.at(i).resize(grid_size_y_);
        d_grid_.at(i).resize(grid_size_y_);
        h_grid_.at(i).resize(grid_size_y_);
    }
}

void Collisions::update()
{
    for (size_t i = 0; i < grid_size_x_; ++i)
    {
        auto& d_col = d_grid_.at(i);
        auto& h_col = h_grid_.at(i);

        for (size_t j = 0; j < grid_size_y_; ++j)
        {
            for (auto it = d_col.at(j).begin(); it != d_col.at(j).end();)
            {
                bool do_increment = true;
                auto old_grid_pos = (*it)->grid_position_;
                updateGridPosition(*it);

                if (old_grid_pos != (*it)->grid_position_)
                {
                    auto next_it = std::next(it);
                    insert(*it, d_grid_);
                    d_col.at(j).erase(it);
                    it = next_it;
                    do_increment = false;
                }

                if (do_increment) ++it;
            }

            for (auto it = h_col.at(j).begin(); it != h_col.at(j).end();)
            {
                bool do_increment = true;
                auto old_grid_pos = (*it)->grid_position_;
                updateGridPosition(*it);

                if (old_grid_pos != (*it)->grid_position_)
                {
                    auto next_it = std::next(it);
                    insert(*it, h_grid_);
                    h_col.at(j).erase(it);
                    it = next_it;
                    do_increment = false;
                }

                if (do_increment) ++it;
            }
        }
    }
}

void Collisions::insert(StaticObject* obj)
{
    updateGridPosition(obj);
    insert(obj, s_grid_);
}

void Collisions::insert(DynamicObject* obj)
{
    updateGridPosition(obj);
    insert(obj, d_grid_);
}

void Collisions::insert(HoveringObject* obj)
{
    updateGridPosition(obj);
    insert(obj, h_grid_);
}

void Collisions::erase(StaticObject* obj)
{
    eraseIfExists(obj, s_grid_);
    obj->grid_position_ = {};
}

void Collisions::erase(DynamicObject* obj)
{
    eraseIfExists(obj, d_grid_);
    obj->grid_position_ = {};
}

void Collisions::erase(HoveringObject* obj)
{
    eraseIfExists(obj, h_grid_);
    obj->grid_position_ = {};
}

void Collisions::updateGridPosition(StaticObject* obj)
{
    auto position = sf::Vector2<size_t>{obj->getPosition() / grid_};

    position.x = std::max<size_t>(0, std::min(grid_size_x_ - 1, position.x));
    position.y = std::max<size_t>(0, std::min(grid_size_y_ - 1, position.y));
    obj->grid_position_ = position;
}

short int Collisions::AABB(const StaticObject& a, const StaticObject& b)
{
    return utils::geo::AABB(a.getPosition() + a.getCollisionArea().getOffset(),
                       {a.getCollisionArea().getA(), a.getCollisionArea().getB()},
                       b.getPosition() + b.getCollisionArea().getOffset(),
                       {b.getCollisionArea().getA(), b.getCollisionArea().getB()});
}

short int Collisions::AABB(const DynamicObject& a, const StaticObject& b)
{
    return utils::geo::AABB(a.getPosition() + a.getCollisionArea().getOffset() + a.getVelocity(),
                       {a.getCollisionArea().getA(), a.getCollisionArea().getB()},
                       b.getPosition() + b.getCollisionArea().getOffset(),
                       {b.getCollisionArea().getA(), b.getCollisionArea().getB()});
}

short int Collisions::AABB(const DynamicObject& a, const DynamicObject& b)
{
    return utils::geo::AABB(a.getPosition() + a.getCollisionArea().getOffset() + a.getVelocity(),
                       {a.getCollisionArea().getA(), a.getCollisionArea().getB()},
                       b.getPosition() + b.getCollisionArea().getOffset() + b.getVelocity(),
                       {b.getCollisionArea().getA(), b.getCollisionArea().getB()});
}

bool Collisions::circleCircle(const StaticObject &a, const StaticObject &b)
{
    return utils::geo::circleCircle(a.getPosition() + a.getCollisionArea().getOffset(), a.getCollisionArea().getA(),
                               b.getPosition() + b.getCollisionArea().getOffset(), b.getCollisionArea().getA());
}

bool Collisions::circleCircle(const DynamicObject &a, const StaticObject &b)
{
    return utils::geo::circleCircle(a.getPosition() + a.getCollisionArea().getOffset() + a.getVelocity(), a.getCollisionArea().getA(),
                               b.getPosition() + b.getCollisionArea().getOffset(), b.getCollisionArea().getA());
}

bool Collisions::circleCircle(const DynamicObject &a, const DynamicObject &b)
{
    return utils::geo::circleCircle(a.getPosition() + a.getCollisionArea().getOffset() + a.getVelocity(), a.getCollisionArea().getA(),
                               b.getPosition() + b.getCollisionArea().getOffset() + b.getVelocity(), b.getCollisionArea().getA());
}

short int Collisions::ABCircle(const StaticObject& a, const StaticObject& b)
{
    return utils::geo::ABCircle(a.getPosition() + a.getCollisionArea().getOffset(),
                           {a.getCollisionArea().getA(), a.getCollisionArea().getB()},
                           b.getPosition() + b.getCollisionArea().getOffset(), b.getCollisionArea().getA());
}

short int Collisions::ABCircle(const DynamicObject& a, const StaticObject& b)
{
    return utils::geo::ABCircle(a.getPosition() + a.getCollisionArea().getOffset() + a.getVelocity(),
                           {a.getCollisionArea().getA(), a.getCollisionArea().getB()},
                           b.getPosition() + b.getCollisionArea().getOffset(), b.getCollisionArea().getA());
}

short int Collisions::ABCircle(const StaticObject& a, const DynamicObject& b)
{
    return utils::geo::ABCircle(a.getPosition() + a.getCollisionArea().getOffset(),
                           {a.getCollisionArea().getA(), a.getCollisionArea().getB()},
                           b.getPosition() + b.getCollisionArea().getOffset() + b.getVelocity(), b.getCollisionArea().getA());
}

short int Collisions::ABCircle(const DynamicObject& a, const DynamicObject& b)
{
    return utils::geo::ABCircle(a.getPosition() + a.getCollisionArea().getOffset() + a.getVelocity(),
                           {a.getCollisionArea().getA(), a.getCollisionArea().getB()},
                           b.getPosition() + b.getCollisionArea().getOffset() + b.getVelocity(), b.getCollisionArea().getA());
}

bool Collisions::circleABResponse(DynamicObject &a, const StaticObject &b)
{
    short int direction = ABCircle(b, a);

    if (!direction) return false;

    float a_r = a.getCollisionArea().getA();
    auto b_bounds = utils::geo::generateCollisionAABB(b);
    auto offset = a.getCollisionArea().getOffset();

    static auto CircleCircleResponse_ = [](DynamicObject &a, const sf::Vector2f &p)
    {
        sf::Vector2f distance = a.getPosition() + a.getVelocity() + a.getCollisionArea().getOffset() - p;

        sf::Vector2f unit = utils::geo::getNormalized(distance);

        a.setPosition(p - a.getCollisionArea().getOffset() +
                      unit * (a.getCollisionArea().getA() + 1));

        a.setForcedVelocity(a.getVelocity() - utils::geo::dotProduct(a.getVelocity(), unit) * unit);
    };

    switch (direction)
    {
        case 1:
            a.setPositionX(std::get<0>(b_bounds).x - offset.x - a_r - 1.0f);
            break;
        case 2:
            a.setPositionY(std::get<0>(b_bounds).y - offset.y - a_r - 1.0f);
            break;
        case 3:
            a.setPositionX(std::get<1>(b_bounds).x - offset.x + a_r + 1.0f);
            break;
        case 4:
            a.setPositionY(std::get<1>(b_bounds).y - offset.y + a_r + 1.0f);
            break;
        case 5:
            a.setPosition(a.getPosition() - a.getVelocity());
            break;
        case 6:
            CircleCircleResponse_(a, std::get<0>(b_bounds));
            break;
        case 7:
            CircleCircleResponse_(a, sf::Vector2f{std::get<1>(b_bounds).x, std::get<0>(b_bounds).y});
            break;
        case 8:
            CircleCircleResponse_(a, std::get<1>(b_bounds));
            break;
        case 9:
            CircleCircleResponse_(a, sf::Vector2f{std::get<0>(b_bounds).x, std::get<1>(b_bounds).y});
            break;
        default:
            return false;
    }

    Collisions::blockNormalVelocity(a, direction);

    return true;
}

bool Collisions::circleABResponse(DynamicObject &a, DynamicObject &b)
{
    short int direction = ABCircle(b, a);

    if (!direction) return false;

    float a_r = a.getCollisionArea().getA();
    auto b_bounds = utils::geo::generateCollisionAABB(b);
    auto a_offset = a.getCollisionArea().getOffset();
    auto b_offset = b.getCollisionArea().getOffset();
    auto a_future_pos = a.getPosition() + a.getVelocity();

    static auto CircleCircleResponse_ = [](DynamicObject &a, DynamicObject &b, const sf::Vector2f &b_size, const sf::Vector2f &p)
    {
        sf::Vector2f distance = a.getPosition() + a.getVelocity() + a.getCollisionArea().getOffset() - p;

        sf::Vector2f unit = utils::geo::getNormalized(distance);

        a.setPosition(p - a.getCollisionArea().getOffset() +
                      unit * (a.getCollisionArea().getA() + 1));
        b.setPosition(a.getPosition() + a.getCollisionArea().getOffset() - b.getCollisionArea().getOffset() -
                      unit * (a.getCollisionArea().getA() + 1) + b_size / 2.0f);
        a.setForcedVelocity(a.getVelocity() - utils::geo::dotProduct(a.getVelocity(), unit) * unit);
        b.setForcedVelocity(b.getVelocity() - utils::geo::dotProduct(b.getVelocity(), unit) * unit);
    };

    switch (direction) {
        case 1:
            b.setPositionX(a_future_pos.x + a_offset.x - b_offset.x + a_r + std::get<2>(b_bounds).x / 2.0f + 1.0f);
            a.setPositionX(std::get<0>(b_bounds).x - a_offset.x - a_r - 1.0f);
            break;
        case 2:
            b.setPositionY(a_future_pos.y + a_offset.y - b_offset.y + a_r + std::get<2>(b_bounds).y / 2.0f + 1.0f);
            a.setPositionY(std::get<0>(b_bounds).y - a_offset.y - a_r - 1.0f);
            break;
        case 3:
            b.setPositionX(a_future_pos.x + a_offset.x - b_offset.x - a_r - std::get<2>(b_bounds).x / 2.0f - 1.0f);
            a.setPositionX(std::get<1>(b_bounds).x - a_offset.x + a_r + 1.0f);
            break;
        case 4:
            b.setPositionY(a_future_pos.y + a_offset.y - b_offset.y - a_r - std::get<2>(b_bounds).y / 2.0f - 1.0f);
            a.setPositionY(std::get<1>(b_bounds).y - a_offset.y + a_r + 1.0f);
            break;
        case 5:
            a.setPosition(a.getPosition() - a.getVelocity());
            b.setPosition(b.getPosition() - b.getVelocity());
            break;
        case 6:
            CircleCircleResponse_(a, b, std::get<2>(b_bounds), std::get<0>(b_bounds));
            break;
        case 7:
            CircleCircleResponse_(a, b, sf::Vector2f{-std::get<2>(b_bounds).x, std::get<2>(b_bounds).y}, sf::Vector2f{std::get<1>(b_bounds).x, std::get<0>(b_bounds).y});
            break;
        case 8:
            CircleCircleResponse_(a, b, -std::get<2>(b_bounds), std::get<1>(b_bounds));
            break;
        case 9:
            CircleCircleResponse_(a, b, sf::Vector2f{std::get<2>(b_bounds).x, -std::get<2>(b_bounds).y}, sf::Vector2f{std::get<0>(b_bounds).x, std::get<1>(b_bounds).y});
            break;
        default:
            return false;
    }

    Collisions::blockNormalVelocity(a, direction);
    Collisions::blockNormalVelocity(b, direction);

    return true;
}

bool Collisions::ABCircleResponse(DynamicObject& a, const StaticObject& b)
{
    short int direction = ABCircle(a, b);

    if (!direction) return false;

    float b_r = b.getCollisionArea().getA();
    auto a_bounds = utils::geo::generateCollisionAABB(a);
    auto offset = b.getCollisionArea().getOffset() - a.getCollisionArea().getOffset();

    static auto CircleCircleResponse_ = [](DynamicObject &a, const StaticObject &b, const sf::Vector2f &a_size, const sf::Vector2f &p)
    {
        sf::Vector2f distance = b.getPosition() + b.getCollisionArea().getOffset() - p;

        sf::Vector2f unit = utils::geo::getNormalized(distance);

        a.setPosition(b.getPosition() + b.getCollisionArea().getOffset() - a.getCollisionArea().getOffset() -
                      unit * (b.getCollisionArea().getA() + 1) + a_size / 2.0f);
        a.setForcedVelocity(a.getVelocity() - utils::geo::dotProduct(a.getVelocity(), unit) * unit);
    };

    switch (direction)
    {
        case 1:
            a.setPositionX(b.getPosition().x + offset.x + b_r + std::get<2>(a_bounds).x / 2.0f + 1.0f);
            break;
        case 2:
            a.setPositionY(b.getPosition().y + offset.y + b_r + std::get<2>(a_bounds).y / 2.0f + 1.0f);
            break;
        case 3:
            a.setPositionX(b.getPosition().x + offset.x - b_r - std::get<2>(a_bounds).x / 2.0f - 1.0f);
            break;
        case 4:
            a.setPositionY(b.getPosition().y + offset.y - b_r - std::get<2>(a_bounds).y / 2.0f - 1.0f);
            break;
        case 5:
            a.setPosition(a.getPosition() - a.getVelocity());
            break;
        case 6:
            CircleCircleResponse_(a, b, std::get<2>(a_bounds), std::get<0>(a_bounds));
            break;
        case 7:
            CircleCircleResponse_(a, b, sf::Vector2f{-std::get<2>(a_bounds).x, std::get<2>(a_bounds).y}, sf::Vector2f{std::get<1>(a_bounds).x, std::get<0>(a_bounds).y});
            break;
        case 8:
            CircleCircleResponse_(a, b, -std::get<2>(a_bounds), std::get<1>(a_bounds));
            break;
        case 9:
            CircleCircleResponse_(a, b, sf::Vector2f{std::get<2>(a_bounds).x, -std::get<2>(a_bounds).y}, sf::Vector2f{std::get<0>(a_bounds).x, std::get<1>(a_bounds).y});
            break;
        default:
            return false;
    }

    Collisions::blockNormalVelocity(a, direction);

    return true;
}

bool Collisions::ABCircleResponse(DynamicObject& a, DynamicObject& b)
{
    short int direction = ABCircle(a, b);

    if (!direction) return false;

    float b_r = b.getCollisionArea().getA();
    auto a_bounds = utils::geo::generateCollisionAABB(a);
    auto b_future_pos = b.getPosition() + b.getVelocity();
    auto a_offset = a.getCollisionArea().getOffset();
    auto b_offset = b.getCollisionArea().getOffset();

    static auto CircleCircleResponse_ = [](DynamicObject &a, DynamicObject &b, const sf::Vector2f &a_size, const sf::Vector2f &b_size, const sf::Vector2f &p)
    {
        sf::Vector2f distance = b.getPosition() + b.getVelocity()  + b.getCollisionArea().getOffset() - p;

        sf::Vector2f unit = utils::geo::getNormalized(distance);

        a.setPosition(b.getPosition() + b.getCollisionArea().getOffset() - a.getCollisionArea().getOffset() -
                      unit * (b.getCollisionArea().getA() + 1) + a_size / 2.0f);
        b.setPosition(a.getPosition() + a.getCollisionArea().getOffset() - b.getCollisionArea().getOffset() -
                      unit * (b.getCollisionArea().getA() + 1) + b_size / 2.0f);

        a.setForcedVelocity(a.getVelocity() - utils::geo::dotProduct(a.getVelocity(), unit) * unit);
        b.setForcedVelocity(b.getVelocity() - utils::geo::dotProduct(b.getVelocity(), unit) * unit);
    };

    switch (direction)
    {
        case 1:
            a.setPositionX(b_future_pos.x + b_offset.x - a_offset.x + b_r + std::get<2>(a_bounds).x / 2.0f + 1.0f);
            b.setPositionX(std::get<0>(a_bounds).x - b_offset.x - b_r - 1.0f);
            break;
        case 2:
            a.setPositionY(b_future_pos.y + b_offset.y - a_offset.y + b_r + std::get<2>(a_bounds).y / 2.0f + 1.0f);
            b.setPositionY(std::get<0>(a_bounds).y - b_offset.y - b_r - 1.0f);
            break;
        case 3:
            a.setPositionX(b_future_pos.x + b_offset.x - a_offset.x - b_r - std::get<2>(a_bounds).x / 2.0f - 1.0f);
            b.setPositionX(std::get<1>(a_bounds).x - b_offset.x + b_r + 1.0f);
            break;
        case 4:
            a.setPositionY(b_future_pos.y + b_offset.y - a_offset.y - b_r - std::get<2>(a_bounds).y / 2.0f - 1.0f);
            b.setPositionY(std::get<1>(a_bounds).y - b_offset.y + b_r + 1.0f);
            break;
        case 5:
            a.setPosition(a.getPosition() - a.getVelocity());
            b.setPosition(b.getPosition() - b.getVelocity());
            break;
        case 6:
            CircleCircleResponse_(a, b, std::get<2>(a_bounds), std::get<2>(a_bounds), std::get<0>(a_bounds));
            break;
        case 7:
            CircleCircleResponse_(a, b, sf::Vector2f{-std::get<2>(a_bounds).x, std::get<2>(a_bounds).y}, std::get<2>(a_bounds), sf::Vector2f{std::get<1>(a_bounds).x, std::get<0>(a_bounds).y});
            break;
        case 8:
            CircleCircleResponse_(a, b, -std::get<2>(a_bounds), std::get<2>(a_bounds), std::get<1>(a_bounds));
            break;
        case 9:
            CircleCircleResponse_(a, b, sf::Vector2f{std::get<2>(a_bounds).x, -std::get<2>(a_bounds).y}, std::get<2>(a_bounds), sf::Vector2f{std::get<0>(a_bounds).x, std::get<1>(a_bounds).y});
            break;
        default:
            return false;
    }

    Collisions::blockNormalVelocity(a, direction);
    Collisions::blockNormalVelocity(b, direction);

    return true;
}

bool Collisions::AABBResponse(DynamicObject& a, const StaticObject& b)
{
    short int direction = AABB(a, b);

    if (!direction) return false;

    auto a_bounds = utils::geo::generateCollisionAABB(a);
    auto b_bounds = utils::geo::generateCollisionAABB(b);
    auto a_offset = a.getCollisionArea().getOffset();

    switch (direction) {
        case 1:
            a.setPositionX(std::get<0>(b_bounds).x - a_offset.x - std::get<2>(a_bounds).x / 2.0f - 1.0f);
            break;
        case 2:
            a.setPositionY(std::get<0>(b_bounds).y - a_offset.y - std::get<2>(a_bounds).y / 2.0f - 1.0f);
            break;
        case 3:
            a.setPositionX(std::get<1>(b_bounds).x - a_offset.x + std::get<2>(a_bounds).x / 2.0f + 1.0f);
            break;
        case 4:
            a.setPositionY(std::get<1>(b_bounds).y - a_offset.y + std::get<2>(a_bounds).y / 2.0f + 1.0f);
            break;
        case 5:
            a.setPosition(a.getPosition() - a.getVelocity());
            break;
        default:
            return false;
    }

    Collisions::blockNormalVelocity(a, direction);

    return true;
}


bool Collisions::AABBResponse(DynamicObject& a, DynamicObject& b)
{
    short int direction = AABB(a, b);

    if (!direction) return false;

    auto a_bounds = utils::geo::generateCollisionAABB(a);
    auto b_bounds = utils::geo::generateCollisionAABB(b);

    auto a_offset = a.getCollisionArea().getOffset();
    auto b_offset = b.getCollisionArea().getOffset();

    switch (direction) {
        case 1:
            a.setPositionX(std::get<0>(b_bounds).x - a_offset.x - std::get<2>(a_bounds).x / 2.0f - 1.0f);
            b.setPositionX(std::get<1>(a_bounds).x - b_offset.x + std::get<2>(b_bounds).x / 2.0f + 1.0f);
            break;
        case 2:
            a.setPositionY(std::get<0>(b_bounds).y - a_offset.y - std::get<2>(a_bounds).y / 2.0f - 1.0f);
            b.setPositionY(std::get<1>(a_bounds).y - b_offset.y + std::get<2>(b_bounds).y / 2.0f + 1.0f);
            break;
        case 3:
            a.setPositionX(std::get<1>(b_bounds).x - a_offset.x + std::get<2>(a_bounds).x / 2.0f + 1.0f);
            b.setPositionX(std::get<0>(a_bounds).x - b_offset.x - std::get<2>(b_bounds).x / 2.0f - 1.0f);
            break;
        case 4:
            a.setPositionY(std::get<1>(b_bounds).y - a_offset.y + std::get<2>(a_bounds).y / 2.0f + 1.0f);
            b.setPositionY(std::get<0>(a_bounds).y - b_offset.y - std::get<2>(b_bounds).y / 2.0f - 1.0f);
            break;
        case 5:
            a.setPosition(a.getPosition() - a.getVelocity());
            b.setPosition(b.getPosition() - b.getVelocity());
            break;
        default:
            return false;
    }

    Collisions::blockNormalVelocity(a, direction);
    Collisions::blockNormalVelocity(b, direction);

    return true;
}

bool Collisions::circleCircleResponse(DynamicObject &a, const StaticObject &b)
{
    if (circleCircle(a, b))
    {
        sf::Vector2f distance = a.getPosition() + a.getVelocity() + a.getCollisionArea().getOffset() - b.getPosition() -
                                b.getCollisionArea().getOffset();

        sf::Vector2f unit = utils::geo::getNormalized(distance);

        a.setPosition(b.getPosition() + b.getCollisionArea().getOffset() - a.getCollisionArea().getOffset() +
                      unit * (a.getCollisionArea().getA() + b.getCollisionArea().getA() + 1));
        a.setForcedVelocity(a.getVelocity() - utils::geo::dotProduct(a.getVelocity(), unit) * unit);
        return true;
    }

    return false;
}

bool Collisions::circleCircleResponse(DynamicObject &a, DynamicObject &b)
{
    if (circleCircle(a, b))
    {
        sf::Vector2f distance = a.getPosition() + a.getVelocity() - b.getVelocity() + a.getCollisionArea().getOffset() - b.getPosition() -
                                b.getCollisionArea().getOffset();

        sf::Vector2f unit = utils::geo::getNormalized(distance);

        a.setPosition(b.getPosition() + b.getCollisionArea().getOffset() - a.getCollisionArea().getOffset() +
                      unit * (a.getCollisionArea().getA() + b.getCollisionArea().getA() + 1));
        b.setPosition(a.getPosition() + a.getCollisionArea().getOffset() - b.getCollisionArea().getOffset() -
                      unit * (a.getCollisionArea().getA() + b.getCollisionArea().getA() + 1));

        a.setForcedVelocity(a.getVelocity() - utils::geo::dotProduct(a.getVelocity(), unit) * unit);
        b.setForcedVelocity(b.getVelocity() - utils::geo::dotProduct(b.getVelocity(), unit) * unit);
        return true;
    }

    return false;
}

void Collisions::blockNormalVelocity(DynamicObject &a, short int dir)
{
    switch (dir) {
        case 1:
        case 3:
            a.setForcedVelocity({0.0f, a.getVelocity().y});
            break;
        case 2:
        case 4:
            a.setForcedVelocity({a.getVelocity().x, 0.0f});
            break;
        case 5:
            a.setForcedVelocity({0.0f, 0.0f});
        default:
            break;
    }
}