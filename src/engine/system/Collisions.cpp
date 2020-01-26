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
    sf::Vector2f b_size = {b.getCollisionArea().getA(), b.getCollisionArea().getB()};
    sf::Vector2f b1 = b.getPosition() + b.getCollisionArea().getOffset() - b_size / 2.0f;
    sf::Vector2f b2 = b.getPosition() + b.getCollisionArea().getOffset() + b_size / 2.0f;


    static auto CircleCircleResponse_ = [](DynamicObject &a, const sf::Vector2f &p)
    {
        sf::Vector2f distance = a.getPosition() + a.getVelocity() + a.getCollisionArea().getOffset() - p;

        sf::Vector2f unit = utils::getNormalized(distance);

        a.setPosition(p - a.getCollisionArea().getOffset() +
                      unit * (a.getCollisionArea().getA() + 1));

        a.setForcedVelocity(a.getVelocity() - (a.getVelocity().x * unit.x + a.getVelocity().y * unit.y) * unit);
    };

    switch (direction)
    {
        case 1:
            a.setForcedVelocity({0.0f, a.getVelocity().y});
            a.setPosition(b1.x - a.getCollisionArea().getOffset().x - a_r - 1.0f, a.getPosition().y);
            break;

        case 2:
            a.setForcedVelocity({a.getVelocity().x, 0.0f});
            a.setPosition(a.getPosition().x, b1.y - a.getCollisionArea().getOffset().y - a_r - 1.0f);
            break;

        case 3:
            a.setForcedVelocity({0.0f, a.getVelocity().y});
            a.setPosition(b2.x - a.getCollisionArea().getOffset().x + a_r + 1.0f, a.getPosition().y);
            break;

        case 4:
            a.setForcedVelocity({a.getVelocity().x, 0.0f});
            a.setPosition(a.getPosition().x, b2.y - a.getCollisionArea().getOffset().y + a_r + 1.0f);
            break;

        case 5:
            a.setPosition(a.getPosition() - a.getVelocity());
            a.setForcedVelocity({0.0f, 0.0f});
            break;

        case 6:
            CircleCircleResponse_(a, b1);
            break;

        case 7:
            CircleCircleResponse_(a, sf::Vector2f{b2.x, b1.y});
            break;

        case 8:
            CircleCircleResponse_(a, b2);
            break;

        case 9:
            CircleCircleResponse_(a, sf::Vector2f{b1.x, b2.y});
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
    sf::Vector2f b_size = {b.getCollisionArea().getA(), b.getCollisionArea().getB()};
    sf::Vector2f b1 = b.getPosition() + b.getCollisionArea().getOffset() + b.getVelocity() - b_size / 2.0f;
    sf::Vector2f b2 = b.getPosition() + b.getCollisionArea().getOffset() + b.getVelocity() + b_size / 2.0f;

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

    switch (direction)
    {
        case 1:
            b.setForcedVelocity({0.0f, b.getVelocity().y});
            b.setPosition(a.getPosition().x + a.getVelocity().x + a.getCollisionArea().getOffset().x - b.getCollisionArea().getOffset().x + a_r + b.getSize().x / 2.0f + 1.0f, b.getPosition().y);
            a.setForcedVelocity({0.0f, a.getVelocity().y});
            a.setPosition(b1.x - a.getCollisionArea().getOffset().x - a_r - 1.0f, a.getPosition().y);
            break;

        case 2:
            b.setForcedVelocity({b.getVelocity().x, 0.0f});
            b.setPosition(b.getPosition().x, a.getPosition().y + a.getVelocity().y + a.getCollisionArea().getOffset().y - b.getCollisionArea().getOffset().y + a_r + b.getSize().y / 2.0f + 1.0f);
            a.setForcedVelocity({a.getVelocity().x, 0.0f});
            a.setPosition(a.getPosition().x, b1.y - a.getCollisionArea().getOffset().y - a_r - 1.0f);
            break;

        case 3:
            b.setForcedVelocity({0.0f, b.getVelocity().y});
            b.setPosition(a.getPosition().x + a.getVelocity().x + a.getCollisionArea().getOffset().x - b.getCollisionArea().getOffset().x - a_r - b.getSize().x / 2.0f - 1.0f, b.getPosition().y);
            a.setForcedVelocity({0.0f, a.getVelocity().y});
            a.setPosition(b2.x - a.getCollisionArea().getOffset().x + a_r + 1.0f, a.getPosition().y);
            break;

        case 4:
            b.setForcedVelocity({b.getVelocity().x, 0.0f});
            b.setPosition(b.getPosition().x, a.getPosition().y + a.getVelocity().y + a.getCollisionArea().getOffset().y - b.getCollisionArea().getOffset().y - a_r - b.getSize().y / 2.0f - 1.0f);
            a.setForcedVelocity({a.getVelocity().x, 0.0f});
            a.setPosition(a.getPosition().x, b2.y - a.getCollisionArea().getOffset().y + a_r + 1.0f);
            break;

        case 5:
            a.setPosition(a.getPosition() - a.getVelocity());
            a.setForcedVelocity({0.0f, 0.0f});
            b.setPosition(b.getPosition() - b.getVelocity());
            b.setForcedVelocity({0.0f, 0.0f});
            break;

        case 6:
            CircleCircleResponse_(a, b, b_size, b1);
            break;

        case 7:
            CircleCircleResponse_(a, b, sf::Vector2f{-b_size.x, b_size.y}, sf::Vector2f{b2.x, b1.y});
            break;

        case 8:
            CircleCircleResponse_(a, b, -b_size, b2);
            break;

        case 9:
            CircleCircleResponse_(a, b, sf::Vector2f{b_size.x, -b_size.y}, sf::Vector2f{b1.x, b2.y});
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
    sf::Vector2f a_size = {a.getCollisionArea().getA(), a.getCollisionArea().getB()};
    sf::Vector2f a1 = a.getPosition() + a.getCollisionArea().getOffset() + a.getVelocity() - a_size / 2.0f;
    sf::Vector2f a2 = a.getPosition() + a.getCollisionArea().getOffset() + a.getVelocity() + a_size / 2.0f;

    static auto CircleCircleResponse_ = [](DynamicObject &a, const StaticObject &b, const sf::Vector2f &a_size, const sf::Vector2f &p)
    {
        sf::Vector2f distance = b.getPosition() + b.getCollisionArea().getOffset() - p;

        sf::Vector2f unit = utils::getNormalized(distance);

        a.setPosition(b.getPosition() + b.getCollisionArea().getOffset() - a.getCollisionArea().getOffset() -
                      unit * (b.getCollisionArea().getA() + 1) + a_size / 2.0f);
        a.setForcedVelocity(a.getVelocity() - (a.getVelocity().x * unit.x + a.getVelocity().y * unit.y) * unit);
    };

    switch (direction)
    {
        case 1:
            a.setForcedVelocity({0.0f, a.getVelocity().y});
            a.setPosition(b.getPosition().x + b.getCollisionArea().getOffset().x - a.getCollisionArea().getOffset().x + b_r + a.getSize().x / 2.0f + 1.0f, a.getPosition().y);
            break;

        case 2:
            a.setForcedVelocity({a.getVelocity().x, 0.0f});
            a.setPosition(a.getPosition().x, b.getPosition().y + b.getCollisionArea().getOffset().y - a.getCollisionArea().getOffset().y + b_r + a.getSize().y / 2.0f + 1.0f);
            break;

        case 3:
            a.setForcedVelocity({0.0f, a.getVelocity().y});
            a.setPosition(b.getPosition().x + b.getCollisionArea().getOffset().x - a.getCollisionArea().getOffset().x - b_r - a.getSize().x / 2.0f - 1.0f, a.getPosition().y);
            break;

        case 4:
            a.setForcedVelocity({a.getVelocity().x, 0.0f});
            a.setPosition(a.getPosition().x, b.getPosition().y + b.getCollisionArea().getOffset().y - a.getCollisionArea().getOffset().y - b_r - a.getSize().y / 2.0f - 1.0f);
            break;

        case 5:
            a.setPosition(a.getPosition() - a.getVelocity());
            a.setForcedVelocity({0.0f, 0.0f});
            break;

        case 6:
            CircleCircleResponse_(a, b, a_size, a1);
            break;

        case 7:
            CircleCircleResponse_(a, b, sf::Vector2f{-a_size.x, a_size.y}, sf::Vector2f{a2.x, a1.y});
            break;

        case 8:
            CircleCircleResponse_(a, b, -a_size, a2);
            break;

        case 9:
            CircleCircleResponse_(a, b, sf::Vector2f{a_size.x, -a_size.y}, sf::Vector2f{a1.x, a2.y});
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
    sf::Vector2f a_size = {a.getCollisionArea().getA(), a.getCollisionArea().getB()};
    sf::Vector2f a1 = a.getPosition() + a.getCollisionArea().getOffset() + a.getVelocity() - a_size / 2.0f;
    sf::Vector2f a2 = a.getPosition() + a.getCollisionArea().getOffset() + a.getVelocity() + a_size / 2.0f;

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

    switch (direction)
    {
        case 1:
            a.setForcedVelocity({0.0f, a.getVelocity().y});
            a.setPosition(b.getPosition().x + b.getVelocity().x + b.getCollisionArea().getOffset().x - a.getCollisionArea().getOffset().x + b_r + a.getSize().x / 2.0f + 1.0f, a.getPosition().y);
            b.setForcedVelocity({0.0f, b.getVelocity().y});
            b.setPosition(a1.x - b.getCollisionArea().getOffset().x - b_r - 1.0f, b.getPosition().y);
            break;

        case 2:
            a.setForcedVelocity({a.getVelocity().x, 0.0f});
            a.setPosition(a.getPosition().x, b.getPosition().y + b.getVelocity().y + b.getCollisionArea().getOffset().y - a.getCollisionArea().getOffset().y + b_r + a.getSize().y / 2.0f + 1.0f);
            b.setForcedVelocity({b.getVelocity().y, 0.0f});
            b.setPosition(b.getPosition().x, a1.y - b.getCollisionArea().getOffset().y - b_r - 1.0f);
            break;

        case 3:
            a.setForcedVelocity({0.0f, a.getVelocity().y});
            a.setPosition(b.getPosition().x + b.getVelocity().x + b.getCollisionArea().getOffset().x - a.getCollisionArea().getOffset().x - b_r - a.getSize().x / 2.0f - 1.0f, a.getPosition().y);
            b.setForcedVelocity({0.0f, b.getVelocity().y});
            b.setPosition(a2.x - b.getCollisionArea().getOffset().x + b_r + 1.0f, b.getPosition().y);
            break;

        case 4:
            a.setForcedVelocity({a.getVelocity().x, 0.0f});
            a.setPosition(a.getPosition().x, b.getPosition().y + b.getVelocity().y + b.getCollisionArea().getOffset().y - a.getCollisionArea().getOffset().y - b_r - a.getSize().y / 2.0f - 1.0f);
            b.setForcedVelocity({b.getVelocity().y, 0.0f});
            b.setPosition(b.getPosition().x, a2.y - b.getCollisionArea().getOffset().y + b_r + 1.0f);
            break;

        case 5:
            a.setPosition(a.getPosition() - a.getVelocity());
            a.setForcedVelocity({0.0f, 0.0f});
            b.setPosition(b.getPosition() - b.getVelocity());
            b.setForcedVelocity({0.0f, 0.0f});
            break;

        case 6:
            CircleCircleResponse_(a, b, a_size, a_size, a1);
            break;

        case 7:
            CircleCircleResponse_(a, b, sf::Vector2f{-a_size.x, a_size.y}, a_size, sf::Vector2f{a2.x, a1.y});
            break;

        case 8:
            CircleCircleResponse_(a, b, -a_size, a_size, a2);
            break;

        case 9:
            CircleCircleResponse_(a, b, sf::Vector2f{a_size.x, -a_size.y}, a_size, sf::Vector2f{a1.x, a2.y});
            break;

        default:
            return false;
    }

    return true;
}

bool Collisions::AABBResponse(DynamicObject& a, const StaticObject& b)
{
    sf::Vector2f a_size = {a.getCollisionArea().getA(), a.getCollisionArea().getB()};
    sf::Vector2f b_size = {b.getCollisionArea().getA(), b.getCollisionArea().getB()};
    sf::Vector2f a1 = a.getPosition() + a.getCollisionArea().getOffset() + a.getVelocity() - a_size / 2.0f;
    sf::Vector2f a2 = a.getPosition() + a.getCollisionArea().getOffset() + a.getVelocity() + a_size / 2.0f;
    sf::Vector2f b1 = b.getPosition() + b.getCollisionArea().getOffset() - b_size / 2.0f;
    sf::Vector2f b2 = b.getPosition() + b.getCollisionArea().getOffset() + b_size / 2.0f;

    short int direction = AABB(a, b);
    if (direction == 1)
    {
        a.setForcedVelocity({0.0f, a.getVelocity().y});
        a.setPosition(b1.x - a.getCollisionArea().getOffset().x - a_size.x / 2.0f - 1.0f, a.getPosition().y);
    }
    else if (direction == 2)
    {
        a.setForcedVelocity({a.getVelocity().x, 0.0f});
        a.setPosition(a.getPosition().x, b1.y - a.getCollisionArea().getOffset().y - a_size.y / 2.0f - 1.0f);
    }
    else if (direction == 3)
    {
        a.setForcedVelocity({0.0f, a.getVelocity().y});
        a.setPosition(b2.x - a.getCollisionArea().getOffset().x + a_size.x / 2.0f + 1.0f, a.getPosition().y);
    }
    else if (direction == 4)
    {
        a.setForcedVelocity({a.getVelocity().x, 0.0f});
        a.setPosition(a.getPosition().x, b2.y - a.getCollisionArea().getOffset().y + a_size.y / 2.0f + 1.0f);
    }
    else if (direction == 5)
    {
        a.setPosition(a.getPosition() - a.getVelocity());
        a.setForcedVelocity({0.0f, 0.0f});
    }
    else
    {
        return false;
    }

    return true;
}


bool Collisions::AABBResponse(DynamicObject& a, DynamicObject& b)
{
    sf::Vector2f a_size = {a.getCollisionArea().getA(), a.getCollisionArea().getB()};
    sf::Vector2f b_size = {b.getCollisionArea().getA(), b.getCollisionArea().getB()};
    sf::Vector2f a1 = a.getPosition() + a.getCollisionArea().getOffset() + a.getVelocity() - a_size / 2.0f;
    sf::Vector2f a2 = a.getPosition() + a.getCollisionArea().getOffset() + a.getVelocity() + a_size / 2.0f;
    sf::Vector2f b1 = b.getPosition() + b.getCollisionArea().getOffset() + b.getVelocity() - b_size / 2.0f;
    sf::Vector2f b2 = b.getPosition() + b.getCollisionArea().getOffset() + b.getVelocity() + b_size / 2.0f;

    short int direction = AABB(a, b);
    if (direction == 1)
    {
        a.setForcedVelocity({0.0f, a.getVelocity().y});
        a.setPosition(b1.x - a.getCollisionArea().getOffset().x - a_size.x / 2.0f - 1.0f, a.getPosition().y);
        b.setForcedVelocity({0.0f, b.getVelocity().y});
        b.setPosition(a2.x - b.getCollisionArea().getOffset().x + b_size.x / 2.0f + 1.0f, b.getPosition().y);
    }
    else if (direction == 2)
    {
        a.setForcedVelocity({a.getVelocity().x, 0.0f});
        a.setPosition(a.getPosition().x, b1.y - a.getCollisionArea().getOffset().y - a_size.y / 2.0f - 1.0f);
        b.setForcedVelocity({b.getVelocity().x, 0.0f});
        b.setPosition(b.getPosition().x, a2.y - b.getCollisionArea().getOffset().y + b_size.y / 2.0f + 1.0f);
    }
    else if (direction == 3)
    {
        a.setForcedVelocity({0.0f, a.getVelocity().y});
        a.setPosition(b2.x - a.getCollisionArea().getOffset().x + a_size.x / 2.0f + 1.0f, a.getPosition().y);
        b.setForcedVelocity({0.0f, b.getVelocity().y});
        b.setPosition(a1.x - b.getCollisionArea().getOffset().x - b_size.x / 2.0f - 1.0f, b.getPosition().y);
    }
    else if (direction == 4)
    {
        a.setForcedVelocity({a.getVelocity().x, 0.0f});
        a.setPosition(a.getPosition().x, b2.y - a.getCollisionArea().getOffset().y + a_size.y / 2.0f + 1.0f);
        b.setForcedVelocity({b.getVelocity().x, 0.0f});
        b.setPosition(b.getPosition().x, a1.y - b.getCollisionArea().getOffset().y - b_size.y / 2.0f - 1.0f);
    }
    else if (direction == 5)
    {
        a.setPosition(a.getPosition() - a.getVelocity());
        a.setForcedVelocity({0.0f, 0.0f});
        b.setPosition(b.getPosition() - b.getVelocity());
        b.setForcedVelocity({0.0f, 0.0f});
    }
    else
    {
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

