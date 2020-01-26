//
// Created by jul3x on 25.01.2020.
//

#include <engine/system/Collisions.h>
#include <engine/utils/Geometry.h>


short int Collisions::AABB(const StaticObject& a, const StaticObject& b)
{
    return utils::AABB(a.getPosition() + a.getCollisionArea().getOffset(),
                       {a.getCollisionArea().getA(), a.getCollisionArea().getB()},
                       b.getPosition() + b.getCollisionArea().getOffset(),
                       {b.getCollisionArea().getA(), b.getCollisionArea().getB()});
}

short int Collisions::AABB(const DynamicObject& a, const StaticObject& b)
{
    return utils::AABB(a.getPosition() + a.getCollisionArea().getOffset() + a.getVelocity(),
                       {a.getCollisionArea().getA(), a.getCollisionArea().getB()},
                       b.getPosition() + b.getCollisionArea().getOffset(),
                       {b.getCollisionArea().getA(), b.getCollisionArea().getB()});
}

short int Collisions::AABB(const DynamicObject& a, const DynamicObject& b)
{
    return utils::AABB(a.getPosition() + a.getCollisionArea().getOffset() + a.getVelocity(),
                       {a.getCollisionArea().getA(), a.getCollisionArea().getB()},
                       b.getPosition() + b.getCollisionArea().getOffset() + b.getVelocity(),
                       {b.getCollisionArea().getA(), b.getCollisionArea().getB()});
}

bool Collisions::CircleCircle(const StaticObject& a, const StaticObject& b)
{
    return utils::CircleCircle(a.getPosition() + a.getCollisionArea().getOffset(), a.getCollisionArea().getA(),
                               b.getPosition() + b.getCollisionArea().getOffset(), b.getCollisionArea().getA());
}

bool Collisions::CircleCircle(const DynamicObject& a, const StaticObject& b)
{
    return utils::CircleCircle(a.getPosition() + a.getCollisionArea().getOffset() + a.getVelocity(), a.getCollisionArea().getA(),
                               b.getPosition() + b.getCollisionArea().getOffset(), b.getCollisionArea().getA());
}

bool Collisions::CircleCircle(const DynamicObject& a, const DynamicObject& b)
{
    return utils::CircleCircle(a.getPosition() + a.getCollisionArea().getOffset() + a.getVelocity(), a.getCollisionArea().getA(),
                               b.getPosition() + b.getCollisionArea().getOffset() + b.getVelocity(), b.getCollisionArea().getA());
}

short int Collisions::ABCircle(const StaticObject& a, const StaticObject& b)
{
    return utils::ABCircle(a.getPosition() + a.getCollisionArea().getOffset(),
                           {a.getCollisionArea().getA(), a.getCollisionArea().getB()},
                           b.getPosition() + b.getCollisionArea().getOffset(), b.getCollisionArea().getA());
}

short int Collisions::ABCircle(const DynamicObject& a, const StaticObject& b)
{
    return utils::ABCircle(a.getPosition() + a.getCollisionArea().getOffset() + a.getVelocity(),
                           {a.getCollisionArea().getA(), a.getCollisionArea().getB()},
                           b.getPosition() + b.getCollisionArea().getOffset(), b.getCollisionArea().getA());
}

short int Collisions::ABCircle(const StaticObject& a, const DynamicObject& b)
{
    return utils::ABCircle(a.getPosition() + a.getCollisionArea().getOffset(),
                           {a.getCollisionArea().getA(), a.getCollisionArea().getB()},
                           b.getPosition() + b.getCollisionArea().getOffset() + b.getVelocity(), b.getCollisionArea().getA());
}

short int Collisions::ABCircle(const DynamicObject& a, const DynamicObject& b)
{
    return utils::ABCircle(a.getPosition() + a.getCollisionArea().getOffset() + a.getVelocity(),
                           {a.getCollisionArea().getA(), a.getCollisionArea().getB()},
                           b.getPosition() + b.getCollisionArea().getOffset() + b.getVelocity(), b.getCollisionArea().getA());
}

bool Collisions::CircleABResponse(DynamicObject& a, const StaticObject& b)
{
    short int direction = ABCircle(b, a);

    float a_r = a.getCollisionArea().getA();
    auto b_bounds = utils::generateCollisionAABB(b);
    auto offset = a.getCollisionArea().getOffset();

    static auto CircleCircleResponse_ = [](DynamicObject &a, const sf::Vector2f &p)
    {
        sf::Vector2f distance = a.getPosition() + a.getVelocity() + a.getCollisionArea().getOffset() - p;

        sf::Vector2f unit = utils::getNormalized(distance);

        a.setPosition(p - a.getCollisionArea().getOffset() +
                      unit * (a.getCollisionArea().getA() + 1));

        a.setForcedVelocity(a.getVelocity() - (a.getVelocity().x * unit.x + a.getVelocity().y * unit.y) * unit);
    };

    switch (direction) {
        case 1:
        case 3:
            a.setForcedVelocity({0.0f, a.getVelocity().y});
            break;
        case 2:
        case 4:
            a.setForcedVelocity({a.getVelocity().x, 0.0f});
            break;
        default:
            break;
    }

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
            a.setForcedVelocity({0.0f, 0.0f});
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

    return true;
}

bool Collisions::CircleABResponse(DynamicObject& a, DynamicObject& b)
{
    short int direction = ABCircle(b, a);

    float a_r = a.getCollisionArea().getA();
    auto b_bounds = utils::generateCollisionAABB(b);
    auto a_offset = a.getCollisionArea().getOffset();
    auto b_offset = b.getCollisionArea().getOffset();
    auto a_future_pos = a.getPosition() + a.getVelocity();

    static auto CircleCircleResponse_ = [](DynamicObject &a, DynamicObject &b, const sf::Vector2f &b_size, const sf::Vector2f &p)
    {
        sf::Vector2f distance = a.getPosition() + a.getVelocity() + a.getCollisionArea().getOffset() - p;

        sf::Vector2f unit = utils::getNormalized(distance);

        a.setPosition(p - a.getCollisionArea().getOffset() +
                      unit * (a.getCollisionArea().getA() + 1));
        b.setPosition(a.getPosition() + a.getCollisionArea().getOffset() - b.getCollisionArea().getOffset() -
                      unit * (a.getCollisionArea().getA() + 1) + b_size / 2.0f);
        a.setForcedVelocity(a.getVelocity() - (a.getVelocity().x * unit.x + a.getVelocity().y * unit.y) * unit);
        b.setForcedVelocity(b.getVelocity() - (b.getVelocity().x * unit.x + b.getVelocity().y * unit.y) * unit);
    };

    switch (direction) {
        case 1:
        case 3:
            a.setForcedVelocity({0.0f, a.getVelocity().y});
            b.setForcedVelocity({0.0f, b.getVelocity().y});
            break;
        case 2:
        case 4:
            a.setForcedVelocity({a.getVelocity().x, 0.0f});
            b.setForcedVelocity({b.getVelocity().x, 0.0f});
            break;
        default:
            break;
    }

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
            a.setForcedVelocity({0.0f, 0.0f});
            b.setPosition(b.getPosition() - b.getVelocity());
            b.setForcedVelocity({0.0f, 0.0f});
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

    return true;
}

bool Collisions::ABCircleResponse(DynamicObject& a, const StaticObject& b)
{
    short int direction = ABCircle(a, b);

    float b_r = b.getCollisionArea().getA();
    auto a_bounds = utils::generateCollisionAABB(a);
    auto offset = b.getCollisionArea().getOffset() - a.getCollisionArea().getOffset();

    static auto CircleCircleResponse_ = [](DynamicObject &a, const StaticObject &b, const sf::Vector2f &a_size, const sf::Vector2f &p)
    {
        sf::Vector2f distance = b.getPosition() + b.getCollisionArea().getOffset() - p;

        sf::Vector2f unit = utils::getNormalized(distance);

        a.setPosition(b.getPosition() + b.getCollisionArea().getOffset() - a.getCollisionArea().getOffset() -
                      unit * (b.getCollisionArea().getA() + 1) + a_size / 2.0f);
        a.setForcedVelocity(a.getVelocity() - (a.getVelocity().x * unit.x + a.getVelocity().y * unit.y) * unit);
    };

    switch (direction) {
        case 1:
        case 3:
            a.setForcedVelocity({0.0f, a.getVelocity().y});
            break;
        case 2:
        case 4:
            a.setForcedVelocity({a.getVelocity().x, 0.0f});
            break;
        default:
            break;
    }

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
            a.setForcedVelocity({0.0f, 0.0f});
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

    return true;
}

bool Collisions::ABCircleResponse(DynamicObject& a, DynamicObject& b)
{
    short int direction = ABCircle(a, b);

    float b_r = b.getCollisionArea().getA();
    auto a_bounds = utils::generateCollisionAABB(a);
    auto b_future_pos = b.getPosition() + b.getVelocity();
    auto a_offset = a.getCollisionArea().getOffset();
    auto b_offset = b.getCollisionArea().getOffset();

    static auto CircleCircleResponse_ = [](DynamicObject &a, DynamicObject &b, const sf::Vector2f &a_size, const sf::Vector2f &b_size, const sf::Vector2f &p)
    {
        sf::Vector2f distance = b.getPosition() + b.getVelocity()  + b.getCollisionArea().getOffset() - p;

        sf::Vector2f unit = utils::getNormalized(distance);

        a.setPosition(b.getPosition() + b.getCollisionArea().getOffset() - a.getCollisionArea().getOffset() -
                      unit * (b.getCollisionArea().getA() + 1) + a_size / 2.0f);
        b.setPosition(a.getPosition() + a.getCollisionArea().getOffset() - b.getCollisionArea().getOffset() -
                      unit * (b.getCollisionArea().getA() + 1) + b_size / 2.0f);

        a.setForcedVelocity(a.getVelocity() - (a.getVelocity().x * unit.x + a.getVelocity().y * unit.y) * unit);
        b.setForcedVelocity(b.getVelocity() - (b.getVelocity().x * unit.x + b.getVelocity().y * unit.y) * unit);
    };

    switch (direction) {
        case 1:
        case 3:
            a.setForcedVelocity({0.0f, a.getVelocity().y});
            b.setForcedVelocity({0.0f, b.getVelocity().y});
            break;
        case 2:
        case 4:
            a.setForcedVelocity({a.getVelocity().x, 0.0f});
            b.setForcedVelocity({b.getVelocity().x, 0.0f});
            break;
        default:
            break;
    }

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
            a.setForcedVelocity({0.0f, 0.0f});
            b.setPosition(b.getPosition() - b.getVelocity());
            b.setForcedVelocity({0.0f, 0.0f});
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

    return true;
}

bool Collisions::AABBResponse(DynamicObject& a, const StaticObject& b)
{
    short int direction = AABB(a, b);

    auto a_bounds = utils::generateCollisionAABB(a);
    auto b_bounds = utils::generateCollisionAABB(b);
    auto offset = a.getCollisionArea().getOffset();

    switch (direction) {
        case 1:
        case 3:
            a.setForcedVelocity({0.0f, a.getVelocity().y});
            break;
        case 2:
        case 4:
            a.setForcedVelocity({a.getVelocity().x, 0.0f});
            break;
        default:
            break;
    }

    switch (direction) {
        case 1:
            a.setPositionX(std::get<0>(b_bounds).x - offset.x - std::get<2>(a_bounds).x / 2.0f - 1.0f);
            break;
        case 2:
            a.setPositionY(std::get<0>(b_bounds).y - offset.y - std::get<2>(a_bounds).y / 2.0f - 1.0f);
            break;
        case 3:
            a.setPositionX(std::get<1>(b_bounds).x - offset.x + std::get<2>(a_bounds).x / 2.0f + 1.0f);
            break;
        case 4:
            a.setPositionY(std::get<1>(b_bounds).y - offset.y + std::get<2>(a_bounds).y / 2.0f + 1.0f);
            break;
        case 5:
            a.setPosition(a.getPosition() - a.getVelocity());
            a.setForcedVelocity({0.0f, 0.0f});
            break;
        default:
            return false;
    }

    return true;
}


bool Collisions::AABBResponse(DynamicObject& a, DynamicObject& b)
{
    auto a_bounds = utils::generateCollisionAABB(a);
    auto b_bounds = utils::generateCollisionAABB(b);

    auto a_offset = a.getCollisionArea().getOffset();
    auto b_offset = b.getCollisionArea().getOffset();

    short int direction = AABB(a, b);

    switch (direction) {
        case 1:
        case 3:
            a.setForcedVelocity({0.0f, a.getVelocity().y});
            b.setForcedVelocity({0.0f, b.getVelocity().y});
            break;
        case 2:
        case 4:
            a.setForcedVelocity({a.getVelocity().x, 0.0f});
            b.setForcedVelocity({b.getVelocity().x, 0.0f});
            break;
        default:
            break;
    }

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
            a.setForcedVelocity({0.0f, 0.0f});
            b.setPosition(b.getPosition() - b.getVelocity());
            b.setForcedVelocity({0.0f, 0.0f});
            break;
        default:
            return false;
    }

    return true;
}

bool Collisions::CircleCircleResponse(DynamicObject& a, const StaticObject& b)
{
    if (CircleCircle(a, b))
    {
        sf::Vector2f distance = a.getPosition() + a.getVelocity() + a.getCollisionArea().getOffset() - b.getPosition() -
                                b.getCollisionArea().getOffset();

        sf::Vector2f unit = utils::getNormalized(distance);

        a.setPosition(b.getPosition() + b.getCollisionArea().getOffset() - a.getCollisionArea().getOffset() +
                      unit * (a.getCollisionArea().getA() + b.getCollisionArea().getA() + 1));
        return true;
    }

    return false;
}

bool Collisions::CircleCircleResponse(DynamicObject& a, DynamicObject& b)
{
    if (CircleCircle(a, b))
    {
        sf::Vector2f distance = a.getPosition() + a.getVelocity() - b.getVelocity() + a.getCollisionArea().getOffset() - b.getPosition() -
                                b.getCollisionArea().getOffset();

        sf::Vector2f unit = utils::getNormalized(distance);

        a.setPosition(b.getPosition() + b.getCollisionArea().getOffset() - a.getCollisionArea().getOffset() +
                      unit * (a.getCollisionArea().getA() + b.getCollisionArea().getA() + 1));
        b.setPosition(a.getPosition() + a.getCollisionArea().getOffset() - b.getCollisionArea().getOffset() -
                      unit * (a.getCollisionArea().getA() + b.getCollisionArea().getA() + 1));

        a.setForcedVelocity(a.getVelocity() - (a.getVelocity().x * unit.x + a.getVelocity().y * unit.y) * unit);
        b.setForcedVelocity(b.getVelocity() - (b.getVelocity().x * unit.x + b.getVelocity().y * unit.y) * unit);
        return true;
    }

    return false;
}

