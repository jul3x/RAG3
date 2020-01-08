//
// Created by jul3x on 30.09.19.
//

#include <engine/utils/Geometry.h>
#include <engine/utils/Numeric.h>
#include <engine/system/Config.h>
#include <engine/objects/AbstractPhysicalObject.h>

//
// AbstractPhysicalObject
//

AbstractPhysicalObject::AbstractPhysicalObject(const sf::Vector2f& position,
                                               const sf::Vector2f& size,
                                               const Collision::Area& c_area,
                                               sf::Texture* texture) :
        AbstractDrawableObject(position, size, texture),
        c_area_(c_area) {}

const Collision::Area& AbstractPhysicalObject::getCollisionArea() const
{
    return c_area_;
}

//
// StaticObject
//

StaticObject::StaticObject(const sf::Vector2f& position,
                           const sf::Vector2f& size,
                           const Collision::Area& c_area,
                           sf::Texture* texture) :
        AbstractPhysicalObject(position, size, c_area, texture) {}

bool StaticObject::update(float time_elapsed) {}

//
// DynamicObject
//

DynamicObject::DynamicObject(const sf::Vector2f& position,
                             const sf::Vector2f& velocity,
                             const sf::Vector2f& size,
                             const Collision::Area& c_area,
                             sf::Texture* texture,
                             const sf::Color& trail_color,
                             const float acceleration) :
        StaticObject(position, size, c_area, texture),
        trail_color_(trail_color),
        curr_v_(velocity), set_v_(velocity),
        acceleration_(acceleration) {}

const sf::Vector2f& DynamicObject::getVelocity() const
{
    return curr_v_;
}

void DynamicObject::setVelocity(const sf::Vector2f& velocity)
{
    set_v_ = velocity;
}

void DynamicObject::setVelocity(const float x, const float y)
{
    set_v_.x = x;
    set_v_.y = y;
}

void DynamicObject::setForcedVelocity(const sf::Vector2f& velocity)
{
    curr_v_ = velocity;
}

bool DynamicObject::update(float time_elapsed)
{
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

void DynamicObject::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (this->isVisible())
    {
        auto pixel_size = this->getSize().x / 5.0f;

        {
            std::vector<sf::Vertex> trail_vert;

            float factor = pixel_size / static_cast<float>(trail_.size() * trail_.size());

            if (trail_.size() >= 2)
            {
                trail_vert.emplace_back(trail_.front(), trail_color_, sf::Vector2f{});

                for (size_t i = 1; i < trail_.size(); ++i)
                {
                    float temp_r = factor * i * i;
                    // make 2 points
                    sf::Vector2f diff = trail_.at(i) - trail_.at(i - 1);
                    auto dir = static_cast<float>(std::atan2(diff.y, diff.x) + M_PI_2);

                    sf::Vector2f norm = {std::cos(dir), std::sin(dir)};
                    trail_vert.emplace_back(trail_.at(i) - temp_r * norm,
                                            trail_color_, sf::Vector2f{});
                    trail_vert.emplace_back(trail_.at(i) + temp_r * norm,
                                            trail_color_, sf::Vector2f{});
                }

                target.draw(&trail_vert[0], trail_vert.size(), sf::TriangleStrip, states);
            }
        }
        target.draw(shape_, states);
    }
}

//
// HoveringObject
//

HoveringObject::HoveringObject(const sf::Vector2f& position,
                               const sf::Vector2f& velocity,
                               const sf::Vector2f& size,
                               const Collision::Area& c_area,
                               sf::Texture* texture,
                               const sf::Color& trail_color,
                               float acceleration) :
        DynamicObject(position, velocity, size, c_area, texture, trail_color, acceleration) {}
