//
// Created by jul3x on 25.01.2020.
//

#include <engine/system/Collisions.h>
#include <engine/utils/Geometry.h>
#include <cmath>


namespace r3e {

    void Collisions::initialize(const sf::Vector2f &size, float grid) {
        grid_ = grid;
        grid_size_x_ = static_cast<size_t>(std::ceil(size.x / grid));
        grid_size_y_ = static_cast<size_t>(std::ceil(size.y / grid));

        s_grid_.resize(grid_size_x_);
        d_grid_.resize(grid_size_x_);
        h_grid_.resize(grid_size_x_);

        for (size_t i = 0; i < grid_size_x_; ++i) {
            s_grid_.at(i).resize(grid_size_y_);
            d_grid_.at(i).resize(grid_size_y_);
            h_grid_.at(i).resize(grid_size_y_);
        }
    }

    void Collisions::update(AbstractGame *game) {
        // s_grid_ does not need to be updated - only erasing and adding new objects is going on there
        updateGrid(d_grid_);
        updateGrid(h_grid_);
        checkCollisions(game, h_grid_, d_grid_);
        checkCollisions(game, h_grid_, s_grid_);
        checkCollisions(game, d_grid_, d_grid_, true);
        checkCollisions(game, d_grid_, s_grid_, true);
    }

    void Collisions::insert(StaticObject *obj) {
        updateGridPosition(obj);
        insert(obj, s_grid_);
    }

    void Collisions::insert(DynamicObject *obj) {
        updateGridPosition(obj);
        insert(obj, d_grid_);
    }

    void Collisions::insert(HoveringObject *obj) {
        updateGridPosition(obj);
        insert(obj, h_grid_);
    }

    void Collisions::erase(StaticObject *obj) {
        eraseIfExists(obj, s_grid_);
        obj->grid_position_ = {};
    }

    void Collisions::erase(DynamicObject *obj) {
        eraseIfExists(obj, d_grid_);
        obj->grid_position_ = {};
    }

    void Collisions::erase(HoveringObject *obj) {
        eraseIfExists(obj, h_grid_);
        obj->grid_position_ = {};
    }

    void Collisions::updateGridPosition(StaticObject *obj) {
        auto position = sf::Vector2i{obj->getPosition() / grid_};

        obj->grid_position_.x = static_cast<size_t>(std::max(0,
                                                             std::min(static_cast<int>(grid_size_x_) - 1, position.x)));
        obj->grid_position_.y = static_cast<size_t>(std::max(0,
                                                             std::min(static_cast<int>(grid_size_y_) - 1, position.y)));
    }

    short int Collisions::AABB(const StaticObject &a, const StaticObject &b) {
        return utils::geo::AABB(a.getPosition() + a.getCollisionArea().getOffset(),
                                {a.getCollisionArea().getA(), a.getCollisionArea().getB()},
                                b.getPosition() + b.getCollisionArea().getOffset(),
                                {b.getCollisionArea().getA(), b.getCollisionArea().getB()});
    }

    bool Collisions::circleCircle(const StaticObject &a, const StaticObject &b) {
        return utils::geo::circleCircle(a.getPosition() + a.getCollisionArea().getOffset(), a.getCollisionArea().getA(),
                                        b.getPosition() + b.getCollisionArea().getOffset(),
                                        b.getCollisionArea().getA());
    }

    short int Collisions::ABCircle(const StaticObject &a, const StaticObject &b) {
        return utils::geo::ABCircle(a.getPosition() + a.getCollisionArea().getOffset(),
                                    {a.getCollisionArea().getA(), a.getCollisionArea().getB()},
                                    b.getPosition() + b.getCollisionArea().getOffset(), b.getCollisionArea().getA());
    }

    bool Collisions::circleABResponse(DynamicObject &a, const StaticObject &b) {
        short int direction = ABCircle(b, a);

        if (!direction) return false;

        float a_r = a.getCollisionArea().getA();
        auto b_bounds = utils::geo::generateCollisionAABB(b);
        auto offset = a.getCollisionArea().getOffset();

        static auto CircleCircleResponse_ = [](DynamicObject &a, const sf::Vector2f &p) {
            sf::Vector2f distance = a.getPosition() + a.getCollisionArea().getOffset() - p;

            if (utils::num::isNearlyEqual(distance, {}, 0.001))
                distance = {1.0f, 1.0f};
            sf::Vector2f unit = utils::geo::getNormalized(distance);

            Collisions::setVerifiedPosition(a, p - a.getCollisionArea().getOffset() +
                                               unit * (a.getCollisionArea().getA() + 1));

            a.setForcedVelocity(a.getVelocity() - utils::geo::dotProduct(a.getVelocity(), unit) * unit);
        };

        switch (direction) {
            case 1:
                Collisions::setVerifiedPositionX(a, std::get<0>(b_bounds).x - offset.x - a_r - 1.0f);
                break;
            case 2:
                Collisions::setVerifiedPositionY(a, std::get<0>(b_bounds).y - offset.y - a_r - 1.0f);
                break;
            case 3:
                Collisions::setVerifiedPositionX(a, std::get<1>(b_bounds).x - offset.x + a_r + 1.0f);
                break;
            case 4:
                Collisions::setVerifiedPositionY(a, std::get<1>(b_bounds).y - offset.y + a_r + 1.0f);
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

    bool Collisions::circleABResponse(DynamicObject &a, DynamicObject &b) {
        short int direction = ABCircle(b, a);

        if (!direction) return false;

        float a_r = a.getCollisionArea().getA();
        auto b_bounds = utils::geo::generateCollisionAABB(b);
        auto a_offset = a.getCollisionArea().getOffset();
        auto b_offset = b.getCollisionArea().getOffset();
        auto a_pos = a.getPosition();

        static auto CircleCircleResponse_ = [](DynamicObject &a, DynamicObject &b, const sf::Vector2f &b_size,
                                               const sf::Vector2f &p) {
            sf::Vector2f distance = a.getPosition() + a.getCollisionArea().getOffset() - p;

            if (utils::num::isNearlyEqual(distance, {}, 0.001))
                distance = {1.0f, 1.0f};
            sf::Vector2f unit = utils::geo::getNormalized(distance);

            Collisions::setVerifiedPosition(a, p - a.getCollisionArea().getOffset() +
                                               unit * (a.getCollisionArea().getA() + 1));
            Collisions::setVerifiedPosition(b, a.getPosition() + a.getCollisionArea().getOffset() -
                                               b.getCollisionArea().getOffset() -
                                               unit * (a.getCollisionArea().getA() + 1) + b_size / 2.0f);
            a.setForcedVelocity(a.getVelocity() - utils::geo::dotProduct(a.getVelocity(), unit) * unit);
            b.setForcedVelocity(b.getVelocity() - utils::geo::dotProduct(b.getVelocity(), unit) * unit);
        };

        switch (direction) {
            case 1:
                Collisions::setVerifiedPositionX(b, a_pos.x + a_offset.x - b_offset.x + a_r +
                                                    std::get<2>(b_bounds).x / 2.0f + 1.0f);
                Collisions::setVerifiedPositionX(a, std::get<0>(b_bounds).x - a_offset.x - a_r - 1.0f);
                break;
            case 2:
                Collisions::setVerifiedPositionY(b, a_pos.y + a_offset.y - b_offset.y + a_r +
                                                    std::get<2>(b_bounds).y / 2.0f + 1.0f);
                Collisions::setVerifiedPositionY(a, std::get<0>(b_bounds).y - a_offset.y - a_r - 1.0f);
                break;
            case 3:
                Collisions::setVerifiedPositionX(b, a_pos.x + a_offset.x - b_offset.x - a_r -
                                                    std::get<2>(b_bounds).x / 2.0f - 1.0f);
                Collisions::setVerifiedPositionX(a, std::get<1>(b_bounds).x - a_offset.x + a_r + 1.0f);
                break;
            case 4:
                Collisions::setVerifiedPositionY(b, a_pos.y + a_offset.y - b_offset.y - a_r -
                                                    std::get<2>(b_bounds).y / 2.0f - 1.0f);
                Collisions::setVerifiedPositionY(a, std::get<1>(b_bounds).y - a_offset.y + a_r + 1.0f);
                break;
            case 6:
                CircleCircleResponse_(a, b, std::get<2>(b_bounds), std::get<0>(b_bounds));
                break;
            case 7:
                CircleCircleResponse_(a, b, sf::Vector2f{-std::get<2>(b_bounds).x, std::get<2>(b_bounds).y},
                                      sf::Vector2f{std::get<1>(b_bounds).x, std::get<0>(b_bounds).y});
                break;
            case 8:
                CircleCircleResponse_(a, b, -std::get<2>(b_bounds), std::get<1>(b_bounds));
                break;
            case 9:
                CircleCircleResponse_(a, b, sf::Vector2f{std::get<2>(b_bounds).x, -std::get<2>(b_bounds).y},
                                      sf::Vector2f{std::get<0>(b_bounds).x, std::get<1>(b_bounds).y});
                break;
            default:
                return false;
        }

        Collisions::blockNormalVelocity(a, direction);
        Collisions::blockNormalVelocity(b, direction);

        return true;
    }

    bool Collisions::ABCircleResponse(DynamicObject &a, const StaticObject &b) {
        short int direction = ABCircle(a, b);

        if (!direction) return false;

        float b_r = b.getCollisionArea().getA();
        auto a_bounds = utils::geo::generateCollisionAABB(a);
        auto offset = b.getCollisionArea().getOffset() - a.getCollisionArea().getOffset();

        static auto CircleCircleResponse_ = [](DynamicObject &a, const StaticObject &b, const sf::Vector2f &a_size,
                                               const sf::Vector2f &p) {
            sf::Vector2f distance = b.getPosition() + b.getCollisionArea().getOffset() - p;

            if (utils::num::isNearlyEqual(distance, {}, 0.001))
                distance = {1.0f, 1.0f};
            sf::Vector2f unit = utils::geo::getNormalized(distance);

            Collisions::setVerifiedPosition(a, b.getPosition() + b.getCollisionArea().getOffset() -
                                               a.getCollisionArea().getOffset() -
                                               unit * (b.getCollisionArea().getA() + 1) + a_size / 2.0f);
            a.setForcedVelocity(a.getVelocity() - utils::geo::dotProduct(a.getVelocity(), unit) * unit);
        };

        switch (direction) {
            case 1:
                Collisions::setVerifiedPositionX(a,
                                                 b.getPosition().x + offset.x + b_r + std::get<2>(a_bounds).x / 2.0f +
                                                 1.0f);
                break;
            case 2:
                Collisions::setVerifiedPositionY(a,
                                                 b.getPosition().y + offset.y + b_r + std::get<2>(a_bounds).y / 2.0f +
                                                 1.0f);
                break;
            case 3:
                Collisions::setVerifiedPositionX(a,
                                                 b.getPosition().x + offset.x - b_r - std::get<2>(a_bounds).x / 2.0f -
                                                 1.0f);
                break;
            case 4:
                Collisions::setVerifiedPositionY(a,
                                                 b.getPosition().y + offset.y - b_r - std::get<2>(a_bounds).y / 2.0f -
                                                 1.0f);
                break;
            case 6:
                CircleCircleResponse_(a, b, std::get<2>(a_bounds), std::get<0>(a_bounds));
                break;
            case 7:
                CircleCircleResponse_(a, b, sf::Vector2f{-std::get<2>(a_bounds).x, std::get<2>(a_bounds).y},
                                      sf::Vector2f{std::get<1>(a_bounds).x, std::get<0>(a_bounds).y});
                break;
            case 8:
                CircleCircleResponse_(a, b, -std::get<2>(a_bounds), std::get<1>(a_bounds));
                break;
            case 9:
                CircleCircleResponse_(a, b, sf::Vector2f{std::get<2>(a_bounds).x, -std::get<2>(a_bounds).y},
                                      sf::Vector2f{std::get<0>(a_bounds).x, std::get<1>(a_bounds).y});
                break;
            default:
                return false;
        }

        Collisions::blockNormalVelocity(a, direction);

        return true;
    }

    bool Collisions::ABCircleResponse(DynamicObject &a, DynamicObject &b) {
        short int direction = ABCircle(a, b);

        if (!direction) return false;

        float b_r = b.getCollisionArea().getA();
        auto a_bounds = utils::geo::generateCollisionAABB(a);
        auto b_pos = b.getPosition();
        auto a_offset = a.getCollisionArea().getOffset();
        auto b_offset = b.getCollisionArea().getOffset();

        static auto CircleCircleResponse_ = [](DynamicObject &a, DynamicObject &b, const sf::Vector2f &a_size,
                                               const sf::Vector2f &b_size, const sf::Vector2f &p) {
            sf::Vector2f distance = b.getPosition() + b.getCollisionArea().getOffset() - p;

            if (utils::num::isNearlyEqual(distance, {}, 0.001))
                distance = {1.0f, 1.0f};
            sf::Vector2f unit = utils::geo::getNormalized(distance);

            Collisions::setVerifiedPosition(a, b.getPosition() + b.getCollisionArea().getOffset() -
                                               a.getCollisionArea().getOffset() -
                                               unit * (b.getCollisionArea().getA() + 1) + a_size / 2.0f);
            Collisions::setVerifiedPosition(b, a.getPosition() + a.getCollisionArea().getOffset() -
                                               b.getCollisionArea().getOffset() -
                                               unit * (b.getCollisionArea().getA() + 1) + b_size / 2.0f);

            a.setForcedVelocity(a.getVelocity() - utils::geo::dotProduct(a.getVelocity(), unit) * unit);
            b.setForcedVelocity(b.getVelocity() - utils::geo::dotProduct(b.getVelocity(), unit) * unit);
        };

        switch (direction) {
            case 1:
                Collisions::setVerifiedPositionX(a, b_pos.x + b_offset.x - a_offset.x + b_r +
                                                    std::get<2>(a_bounds).x / 2.0f + 1.0f);
                Collisions::setVerifiedPositionX(b, std::get<0>(a_bounds).x - b_offset.x - b_r - 1.0f);
                break;
            case 2:
                Collisions::setVerifiedPositionY(a, b_pos.y + b_offset.y - a_offset.y + b_r +
                                                    std::get<2>(a_bounds).y / 2.0f + 1.0f);
                Collisions::setVerifiedPositionY(b, std::get<0>(a_bounds).y - b_offset.y - b_r - 1.0f);
                break;
            case 3:
                Collisions::setVerifiedPositionX(a, b_pos.x + b_offset.x - a_offset.x - b_r -
                                                    std::get<2>(a_bounds).x / 2.0f - 1.0f);
                Collisions::setVerifiedPositionX(b, std::get<1>(a_bounds).x - b_offset.x + b_r + 1.0f);
                break;
            case 4:
                Collisions::setVerifiedPositionY(a, b_pos.y + b_offset.y - a_offset.y - b_r -
                                                    std::get<2>(a_bounds).y / 2.0f - 1.0f);
                Collisions::setVerifiedPositionY(b, std::get<1>(a_bounds).y - b_offset.y + b_r + 1.0f);
                break;
            case 6:
                CircleCircleResponse_(a, b, std::get<2>(a_bounds), std::get<2>(a_bounds), std::get<0>(a_bounds));
                break;
            case 7:
                CircleCircleResponse_(a, b, sf::Vector2f{-std::get<2>(a_bounds).x, std::get<2>(a_bounds).y},
                                      std::get<2>(a_bounds),
                                      sf::Vector2f{std::get<1>(a_bounds).x, std::get<0>(a_bounds).y});
                break;
            case 8:
                CircleCircleResponse_(a, b, -std::get<2>(a_bounds), std::get<2>(a_bounds), std::get<1>(a_bounds));
                break;
            case 9:
                CircleCircleResponse_(a, b, sf::Vector2f{std::get<2>(a_bounds).x, -std::get<2>(a_bounds).y},
                                      std::get<2>(a_bounds),
                                      sf::Vector2f{std::get<0>(a_bounds).x, std::get<1>(a_bounds).y});
                break;
            default:
                return false;
        }

        Collisions::blockNormalVelocity(a, direction);
        Collisions::blockNormalVelocity(b, direction);

        return true;
    }

    bool Collisions::AABBResponse(DynamicObject &a, const StaticObject &b) {
        short int direction = AABB(a, b);

        if (!direction) return false;

        auto a_bounds = utils::geo::generateCollisionAABB(a);
        auto b_bounds = utils::geo::generateCollisionAABB(b);
        auto a_offset = a.getCollisionArea().getOffset();

        switch (direction) {
            case 1:
                Collisions::setVerifiedPositionX(a,
                                                 std::get<0>(b_bounds).x - a_offset.x - std::get<2>(a_bounds).x / 2.0f -
                                                 1.0f);
                break;
            case 2:
                Collisions::setVerifiedPositionY(a,
                                                 std::get<0>(b_bounds).y - a_offset.y - std::get<2>(a_bounds).y / 2.0f -
                                                 1.0f);
                break;
            case 3:
                Collisions::setVerifiedPositionX(a,
                                                 std::get<1>(b_bounds).x - a_offset.x + std::get<2>(a_bounds).x / 2.0f +
                                                 1.0f);
                break;
            case 4:
                Collisions::setVerifiedPositionY(a,
                                                 std::get<1>(b_bounds).y - a_offset.y + std::get<2>(a_bounds).y / 2.0f +
                                                 1.0f);
                break;
            default:
                return false;
        }

        Collisions::blockNormalVelocity(a, direction);

        return true;
    }


    bool Collisions::AABBResponse(DynamicObject &a, DynamicObject &b) {
        short int direction = AABB(a, b);

        if (!direction) return false;

        auto a_bounds = utils::geo::generateCollisionAABB(a);
        auto b_bounds = utils::geo::generateCollisionAABB(b);

        auto a_offset = a.getCollisionArea().getOffset();
        auto b_offset = b.getCollisionArea().getOffset();

        switch (direction) {
            case 1:
                Collisions::setVerifiedPositionX(a,
                                                 std::get<0>(b_bounds).x - a_offset.x - std::get<2>(a_bounds).x / 2.0f -
                                                 1.0f);
                Collisions::setVerifiedPositionX(b,
                                                 std::get<1>(a_bounds).x - b_offset.x + std::get<2>(b_bounds).x / 2.0f +
                                                 1.0f);
                break;
            case 2:
                Collisions::setVerifiedPositionY(a,
                                                 std::get<0>(b_bounds).y - a_offset.y - std::get<2>(a_bounds).y / 2.0f -
                                                 1.0f);
                Collisions::setVerifiedPositionY(b,
                                                 std::get<1>(a_bounds).y - b_offset.y + std::get<2>(b_bounds).y / 2.0f +
                                                 1.0f);
                break;
            case 3:
                Collisions::setVerifiedPositionX(a,
                                                 std::get<1>(b_bounds).x - a_offset.x + std::get<2>(a_bounds).x / 2.0f +
                                                 1.0f);
                Collisions::setVerifiedPositionX(b,
                                                 std::get<0>(a_bounds).x - b_offset.x - std::get<2>(b_bounds).x / 2.0f -
                                                 1.0f);
                break;
            case 4:
                Collisions::setVerifiedPositionY(a,
                                                 std::get<1>(b_bounds).y - a_offset.y + std::get<2>(a_bounds).y / 2.0f +
                                                 1.0f);
                Collisions::setVerifiedPositionY(b,
                                                 std::get<0>(a_bounds).y - b_offset.y - std::get<2>(b_bounds).y / 2.0f -
                                                 1.0f);
                break;
            default:
                return false;
        }

        Collisions::blockNormalVelocity(a, direction);
        Collisions::blockNormalVelocity(b, direction);

        return true;
    }

    bool Collisions::circleCircleResponse(DynamicObject &a, const StaticObject &b) {
        if (circleCircle(a, b)) {
            sf::Vector2f distance = a.getPosition() + a.getCollisionArea().getOffset() - b.getPosition() -
                                    b.getCollisionArea().getOffset();

            if (utils::num::isNearlyEqual(distance, {}, 0.001))
                distance = {1.0f, 1.0f};
            sf::Vector2f unit = utils::geo::getNormalized(distance);

            Collisions::setVerifiedPosition(a, b.getPosition() + b.getCollisionArea().getOffset() -
                                               a.getCollisionArea().getOffset() +
                                               unit * (a.getCollisionArea().getA() + b.getCollisionArea().getA() + 1));
            a.setForcedVelocity(a.getVelocity() - utils::geo::dotProduct(a.getVelocity(), unit) * unit);
            return true;
        }

        return false;
    }

    bool Collisions::circleCircleResponse(DynamicObject &a, DynamicObject &b) {
        if (circleCircle(a, b)) {
            sf::Vector2f distance = a.getPosition() + a.getCollisionArea().getOffset() - b.getPosition() -
                                    b.getCollisionArea().getOffset();

            if (utils::num::isNearlyEqual(distance, {}, 0.001))
                distance = {1.0f, 1.0f};
            sf::Vector2f unit = utils::geo::getNormalized(distance);

            Collisions::setVerifiedPosition(a, b.getPosition() + b.getCollisionArea().getOffset() -
                                               a.getCollisionArea().getOffset() +
                                               unit * (a.getCollisionArea().getA() + b.getCollisionArea().getA() + 1));
            Collisions::setVerifiedPosition(b, a.getPosition() + a.getCollisionArea().getOffset() -
                                               b.getCollisionArea().getOffset() -
                                               unit * (a.getCollisionArea().getA() + b.getCollisionArea().getA() + 1));

            a.setForcedVelocity(a.getVelocity() - utils::geo::dotProduct(a.getVelocity(), unit) * unit);
            b.setForcedVelocity(b.getVelocity() - utils::geo::dotProduct(b.getVelocity(), unit) * unit);
            return true;
        }

        return false;
    }

    void Collisions::blockNormalVelocity(DynamicObject &a, short int dir) {
        switch (dir) {
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
    }

    void Collisions::setVerifiedPosition(StaticObject &a, const sf::Vector2f &pos) {
        float threshold = a.getCollisionArea().getA() / 2.0f;
        if (utils::geo::getDistance(pos, a.getPosition()) < threshold)
            a.setPosition(pos);
        else
            a.setPosition(a.getPosition() + utils::geo::vectorLengthLimit(pos - a.getPosition(), threshold));
    }

    void Collisions::setVerifiedPositionX(StaticObject &a, float x) {
        Collisions::setVerifiedPosition(a, {x, a.getPosition().y});
    }

    void Collisions::setVerifiedPositionY(StaticObject &a, float y) {
        Collisions::setVerifiedPosition(a, {a.getPosition().x, y});
    }

} // namespace r3e