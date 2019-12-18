//
// Created by jprolejko on 30.09.19.
//

#include <engine/utils/Geometry.h>
#include <engine/utils/Numeric.h>
#include <engine/system/Config.h>
#include <engine/objects/AbstractPhysicalObject.h>

//
// AbstractPhysicalObject
//

AbstractPhysicalObject::AbstractPhysicalObject(const sf::Vector2f &position,
                                               const sf::Vector2f &size,
                                               const std::string &texture_name) :
        AbstractDrawableObject(position, size, texture_name) {}

//
// StaticObject
//

StaticObject::StaticObject(const sf::Vector2f &position,
                           const sf::Vector2f &size,
                           const std::string &texture_name) :
        AbstractPhysicalObject(position, size, texture_name) {}

bool StaticObject::update(float time_elapsed) {}

//
// DynamicObject
//

DynamicObject::DynamicObject(const sf::Vector2f &position,
                             const sf::Vector2f &velocity,
                             const sf::Vector2f &size,
                             const std::string &texture_name,
                             const double acceleration) :
        StaticObject(position, size, texture_name),
        curr_v_(velocity), set_v_(velocity),
        acceleration_(acceleration) {}

const sf::Vector2f& DynamicObject::getVelocity() const {
    return curr_v_;
}

void DynamicObject::setVelocity(const sf::Vector2f &velocity) {
    set_v_ = velocity;
}

void DynamicObject::setVelocity(const float x, const float y) {
    set_v_.x = x;
    set_v_.y = y;
}

void DynamicObject::setForcedVelocity(const sf::Vector2f &velocity) {
    curr_v_ = velocity;
}

void DynamicObject::setVisibility(const sf::View &view) {
    // visibility is checked on smaller view than static obstacles
    is_visible_ = utils::AABB(view.getCenter(), view.getSize(),
                              this->getPosition(), this->getSize());
}

bool DynamicObject::update(float time_elapsed) {
    trail_.push_back(this->getPosition());

    if (trail_.size() > TRAIL_COUNT_)
    {
        trail_.pop_front();
    }

    if (utils::isNearlyEqual(set_v_.x, 0.0f, 0.01f) &&
        utils::isNearlyEqual(curr_v_.x, 0.0f, 0.05f))
    {
        curr_v_.x = 0.0f;
    }
    else
    {
        curr_v_.x = curr_v_.x - std::copysign(acceleration_ * time_elapsed, curr_v_.x - set_v_.x);
    }
    if (utils::isNearlyEqual(set_v_.y, 0.0f, 0.01f) &&
        utils::isNearlyEqual(curr_v_.y, 0.0f, 0.05f))
    {
        curr_v_.y = 0.0f;
    }
    else
    {
        curr_v_.y = curr_v_.y - std::copysign(acceleration_ * time_elapsed, curr_v_.y - set_v_.y);
    }

    this->setPosition(this->getPosition() + curr_v_);

    return true;
}

void DynamicObject::draw(sf::RenderTarget &target, sf::RenderStates states) const {
    if (this->isVisible())
    {
        auto pixel_size = this->getSize().x / 5.0f;

        {
            std::vector<sf::Vertex> trail_vert;

            float factor = pixel_size / static_cast<float>(trail_.size() * trail_.size());

            if (trail_.size() >= 2)
            {
                trail_vert.emplace_back(trail_.front(), sf::Color(CFG.getInt("trail_color")),
                        sf::Vector2f{});
            }

            for (size_t i = 1; i < trail_.size(); ++i)
            {
                float temp_r = factor * i * i;
                // make 2 points
                sf::Vector2f diff = trail_.at(i) - trail_.at(i - 1);
                float dir = std::atan2(diff.y, diff.x) + M_PI_2;

                sf::Vector2f norm = {std::cos(dir), std::sin(dir)};
                trail_vert.emplace_back(trail_.at(i) - temp_r * norm,
                        sf::Color(CFG.getInt("trail_color")), sf::Vector2f{});
                trail_vert.emplace_back(trail_.at(i) + temp_r * norm,
                        sf::Color(CFG.getInt("trail_color")), sf::Vector2f{});
            }

            target.draw(&trail_vert[0], trail_vert.size(), sf::TriangleStrip, states);
        }
        target.draw(shape_, states);
    }
}

//
// HoveringObject
//

HoveringObject::HoveringObject(const sf::Vector2f &position,
                               const sf::Vector2f &velocity,
                               const sf::Vector2f &size,
                               const std::string &texture_name,
                               const double acceleration) :
        DynamicObject(position, velocity, size, texture_name, acceleration) {}
