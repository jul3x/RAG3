//
// Created by jul3x on 01.10.19.
//

#include <R3E/system/Config.h>

#include <common/weapons/Bullet.h>
#include <common/ResourceManager.h>


Bullet::Bullet(Character* user,
               const sf::Vector2f& position,
               const std::string& id,
               const float direction) :
        Functional(RMGET<std::string>("bullets", id, "default_activation"),
                   RMGET<j3x::List>("bullets", id, "default_functions"),
                   RMGET<j3x::List>("bullets", id, "default_datas"),
                   id, -2),
        HoveringObject(position,
                       RMGET<float>("bullets", id, "speed") *
                               sf::Vector2f(std::cos(direction), std::sin(direction)),
                       RMGET<sf::Vector2f>("bullets", id, "size"),
                       collision::Box(RMGET<sf::Vector2f>("bullets", id, "collision_offset").x,
                                      RMGET<sf::Vector2f>("bullets", id, "collision_offset").y,
                                      RMGET<sf::Vector2f>("bullets", id, "collision_offset")),
                       &RM.getTexture("bullets/" + id),
                       RMGET<int>("bullets", id, "z_index"),
                       0, 0.0f, 0.0f),
        user_(user),
        trail_color_(sf::Color(CONF<int>("graphics/trail_color"))),
        trail_time_elapsed_(0.0f),
        life_(RMGET<float>("bullets", id, "life")),
        deadly_factor_(RMGET<float>("bullets", id, "deadly_factor")),
        TRAIL_TIME_STEP_(CONF<float>("graphics/full_trail_time") / CONF<int>("graphics/bullet_trail_vertices"))
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

        if (trail_.size() > CONF<int>("graphics/bullet_trail_vertices"))
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

Character* Bullet::getUser() const
{
    return user_;
}
