//
// Created by jul3x on 21.12.2020.
//

#include <R3E/utils/Geometry.h>

#include <misc/Destruction.h>
#include <common/ResourceManager.h>


DestructionParticle::DestructionParticle(const sf::Vector2f& position, float dir, const DestructionParams& params) : pos_(position)
{
    double _dir = utils::geo::wrapAngle0_2PI((static_cast<double>(dir) + utils::num::getRandom(-params.spread_degree, params.spread_degree)) * M_PI / 180.0);
    dir_ = _dir;
    sf::Vector2f unit = sf::Vector2f{static_cast<float>(std::cos(_dir)), static_cast<float>(std::sin(_dir))};

    vel_ = (params.vel + params.vel_fac * utils::num::getRandom(-params.vel, params.vel)) * unit;
    acc_ = (-unit + sf::Vector2f{utils::num::getRandom(-params.acceleration_spread, params.acceleration_spread),
                                 utils::num::getRandom(-params.acceleration_spread, params.acceleration_spread)}) * (params.acc + params.acc_fac * utils::num::getRandom(-params.acc, params.acc));
    time_alive_ = params.time + params.time_fac * utils::num::getRandom(-params.time, params.time);

    if (params.full_color_fac == 0.0f)
    {
        color_.r = std::max(0.0f, std::min(255.0f, params.base_color.r + params.r_fac * utils::num::getRandom<float>(-params.base_color.r, params.base_color.r)));
        color_.g = std::max(0.0f, std::min(255.0f, params.base_color.g + params.g_fac * utils::num::getRandom<float>(-params.base_color.g, params.base_color.g)));
        color_.b = std::max(0.0f, std::min(255.0f, params.base_color.b + params.b_fac * utils::num::getRandom<float>(-params.base_color.b, params.base_color.b)));
    }
    else
    {
        float gamma_factor = utils::num::getRandom(-params.full_color_fac, params.full_color_fac);
        color_.r = std::max(0.0f, std::min(255.0f, params.base_color.r + gamma_factor * 255));
        color_.g = std::max(0.0f, std::min(255.0f, params.base_color.g + gamma_factor * 255));
        color_.b = std::max(0.0f, std::min(255.0f, params.base_color.b + gamma_factor * 255));
    }

    size_ = utils::num::getRandom(params.min_size, params.max_size);
}

bool DestructionParticle::update(float time_elapsed)
{
    vel_ += acc_ * time_elapsed;
    pos_ += time_elapsed * vel_;

    time_alive_ -= time_elapsed;

    if (!utils::num::isNearlyEqual(utils::geo::wrapAngle0_2PI(std::get<1>(utils::geo::cartesianToPolar(vel_))), dir_, M_PI_2))
    {
        vel_ = {};
        acc_ = {};
    }

    return time_alive_ > 0.0f;
}

[[nodiscard]] const sf::Vector2f& DestructionParticle::getPosition() const
{
    return pos_;
}

[[nodiscard]] float DestructionParticle::getTime() const
{
    return time_alive_;
}

[[nodiscard]] float DestructionParticle::getSize() const
{
    return size_;
}

[[nodiscard]] const sf::Color& DestructionParticle::getColor() const
{
    return color_;
}

void DestructionParticle::addToLife(float time)
{
    time_alive_ += time;
}

DestructionSystem::DestructionSystem(const sf::Vector2f& position, float dir, const DestructionParams& params) :
        AbstractDrawableObject(position, {1.0f, 1.0f}, nullptr, 1),
        particles_(params.count),
        drawables_(sf::Quads, 4 * params.count),
        time_elapsed_(0.0f),
        params_(params),
        dir_(dir)
{
    size_t i = 0;
    for (auto& particle : particles_)
    {
        particle = std::make_unique<DestructionParticle>(position, dir, params);
        updateParticle(particle, i, 255);
        ++i;
    }
}

bool DestructionSystem::update(float time_elapsed)
{
    bool any_alive = false;
    size_t i = 0;
    for (auto& particle : particles_)
    {
        if (!particle->update(time_elapsed))
        {
//                drawables_[i].color = sf::Color::Transparent;
        }
        else
        {
            any_alive = true;
        }

        updateParticle(particle, i, static_cast<sf::Uint8>(std::min(std::max(particle->getTime(), 0.0f), 1.0f) * 255));

        ++i;
    }

    if (time_elapsed_ > 0.25f) this->setZIndex(0);

    time_elapsed_ += time_elapsed;
    return any_alive;
}

void DestructionSystem::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    static sf::RenderStates states_;
    states_.shader = &RM.getShader(params_.shader);
    target.draw(drawables_, states_);
}

void DestructionSystem::updateParticle(std::unique_ptr<DestructionParticle>& particle, int i, sf::Uint8 a)
{
    for (size_t j = 0; j < 4; ++j)
    {
        drawables_[4 * i + j].position = particle->getPosition();
        drawables_[4 * i + j].color = particle->getColor();
        drawables_[4 * i + j].color.a = a;
    }
    drawables_[4 * i + 1].position += {particle->getSize(), 0};
    drawables_[4 * i + 2].position += {particle->getSize(), particle->getSize()};
    drawables_[4 * i + 3].position += {0, particle->getSize()};
}

float DestructionSystem::getDestructionDir() const
{
    return dir_;
}

const DestructionParams& DestructionSystem::getDestructionParams() const
{
    return params_;
}

void DestructionSystem::addToLife(float time)
{
    time_elapsed_ -= time;
    for (auto& particle : particles_)
    {
        particle->addToLife(time);
    }
}

