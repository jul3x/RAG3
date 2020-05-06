//
// Created by jul3x on 01.10.19.
//

#include <R3E/system/Config.h>

#include <common/Bullet.h>
#include <common/ResourceManager.h>

const float Bullet::TRAIL_TIME_STEP_ = CFG.get<float>("graphics/full_trail_time") / Bullet::TRAIL_COUNT_;

Bullet::Bullet(const sf::Vector2f& position,
               const std::string& id,
               const float direction) :
        Functional(utils::j3x::get<std::string>(RM.getObjectParams("bullets", id), "default_activation"),
                   utils::j3x::get<std::vector<std::string>>(RM.getObjectParams("bullets", id), "default_functions"),
                   utils::j3x::get<std::vector<std::string>>(RM.getObjectParams("bullets", id), "default_datas"),
                   id, -2),
        HoveringObject(position,
                       utils::j3x::get<float>(RM.getObjectParams("bullets", id), "speed") *
                               sf::Vector2f(std::cos(direction), std::sin(direction)),
                       {utils::j3x::get<float>(RM.getObjectParams("bullets", id), "size_x"),
                        utils::j3x::get<float>(RM.getObjectParams("bullets", id), "size_y")},
                       collision::Box(utils::j3x::get<float>(RM.getObjectParams("bullets", id), "size_x"),
                                      utils::j3x::get<float>(RM.getObjectParams("bullets", id), "size_y")),
                       &RM.getTexture("bullets/" + id),
                       utils::j3x::get<int>(RM.getObjectParams("bullets", id), "z_index"),
                       0, 0.0f, 0.0f),
        trail_color_(sf::Color(CFG.get<int>("graphics/trail_color"))),
        trail_time_elapsed_(0.0f),
        life_(utils::j3x::get<float>(RM.getObjectParams("bullets", id), "life")),
        deadly_factor_(utils::j3x::get<float>(RM.getObjectParams("bullets", id), "deadly_factor"))
{
    this->setRotation(direction * 180.0f / static_cast<float>(M_PI));
}

float Bullet::getDeadlyFactor() const
{
    return deadly_factor_;
}

void Bullet::setDead()
{
    life_ = -1.0f;
}

bool Bullet::update(float time_elapsed)
{
    DynamicObject::update(time_elapsed);

    trail_time_elapsed_ += time_elapsed;

    if (trail_time_elapsed_ > TRAIL_TIME_STEP_)
    {
        trail_time_elapsed_ -= TRAIL_TIME_STEP_;

        trail_.push_back(this->getPosition());

        if (trail_.size() > TRAIL_COUNT_)
        {
            trail_.pop_front();
        }
    }

    life_ -= time_elapsed;

    return life_ > 0.0f;
}

void Bullet::draw(sf::RenderTarget& target, sf::RenderStates states) const
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