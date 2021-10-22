//
// Created by jul3x on 6.04.2021.
//

#include <R3E/utils/Geometry.h>

#include <common/misc/Weather.h>
#include <common/Framework.h>
#include <common/ResourceManager.h>


WeatherParticle::WeatherParticle(const sf::Vector2f& position, WeatherParams* params) :
        pos_(position), time_elapsed_(0.0f)
{
    dir_ = params->dir + utils::num::getRandom(-params->spread_degree, params->spread_degree);
    sf::Vector2f unit = sf::Vector2f{static_cast<float>(std::cos(dir_)), static_cast<float>(std::sin(dir_))};

    perp_vel_ = params->vel * sf::Vector2f{-unit.y, unit.x};
    vel_ = (params->vel + utils::num::getRandom(-params->spread_vel, params->spread_vel)) * unit;

    color_ = params->base_color;
    color_.a = utils::num::getRandom(params->alpha_min, params->alpha_max);
    size_ = utils::num::getRandom(params->min_size, params->max_size);
}

bool WeatherParticle::update(float time_elapsed)
{
    pos_ += time_elapsed * (vel_ + perp_vel_ * static_cast<float>(std::sin(time_elapsed_ * M_PI)));
    time_elapsed_ += time_elapsed;
    return true;
}

const sf::Vector2f& WeatherParticle::getPosition() const
{
    return pos_;
}

const sf::Color& WeatherParticle::getColor() const
{
    return color_;
}

float WeatherParticle::getSize() const
{
    return size_;
}

WeatherSystem::WeatherSystem(Framework* framework, WeatherParams* params) :
        AbstractDrawableObject({}, {1.0f, 1.0f}, nullptr, 0),
        particles_(),
        framework_(framework),
        drawables_(sf::Quads, 0),
        time_elapsed_(0.0f),
        params_(params)
{
}

bool WeatherSystem::update(float time_elapsed)
{
    int i = 0;
    auto pos = framework_->getPlayer()->getPosition();
    static auto width = CONF<int>("graphics/window_width_px") / CONF<float>("graphics/GLOBAL_ZOOM");
    static auto height = CONF<int>("graphics/window_height_px") / CONF<float>("graphics/GLOBAL_ZOOM");
    this->setPosition(pos);

    utils::eraseIf<std::unique_ptr<WeatherParticle>>(particles_,
                                                     [this, time_elapsed, &i](
                                                             std::unique_ptr<WeatherParticle>& particle) {
                                                         particle->update(time_elapsed);
                                                         updateParticle(particle, i);

                                                         ++i;

                                                         if (utils::geo::getDistance(particle->getPosition(),
                                                                                     this->getPosition()) > width)
                                                             return true;
                                                         return false;
                                                     });


    if (time_elapsed_ > params_->new_particles_time)
    {
        i = 0;

        for (i = -params_->count / 2; i < params_->count / 2; ++i)
        {
            particles_.emplace_back(std::make_unique<WeatherParticle>(
                    pos + sf::Vector2f(i * width / params_->count * 2, height), params_));
        }
        drawables_.resize(particles_.size() * 4);
        time_elapsed_ -= params_->new_particles_time;
    }

    time_elapsed_ += time_elapsed;
    return true;
}

void WeatherSystem::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    static sf::RenderStates states_;
    states_.shader = &RM.getShader(params_->shader);
    target.draw(drawables_, states_);
}

void WeatherSystem::updateParticle(std::unique_ptr<WeatherParticle>& particle, int i)
{
    for (size_t j = 0; j < 4; ++j)
    {
        drawables_[4 * i + j].position = particle->getPosition();

        if (framework_->getRag3Time() <= 0.0f)
            drawables_[4 * i + j].color = particle->getColor();
        else
        {
            drawables_[4 * i + j].color = params_->rag3_color;
            drawables_[4 * i + j].color.a = particle->getColor().a;
        }
    }
    drawables_[4 * i + 1].position += {particle->getSize(), 0};
    drawables_[4 * i + 2].position += {particle->getSize(), particle->getSize()};
    drawables_[4 * i + 3].position += {0, particle->getSize()};
}
