//
// Created by jul3x on 6.04.2021.
//

#include <R3E/utils/Geometry.h>

#include <common/misc/Weather.h>
#include <common/Framework.h>
#include <common/ResourceManager.h>


WeatherParticle::WeatherParticle(const sf::Vector2f& position, float dir, const WeatherParams& params) :
        pos_(position), dir_(dir), time_elapsed_(0.0f)
{
    dir_ = dir + utils::num::getRandom(-10.0f, 10.0f);
    sf::Vector2f unit = sf::Vector2f{static_cast<float>(std::cos(dir_)), static_cast<float>(std::sin(dir_))};

    perp_vel_ = 100.0f * sf::Vector2f{-unit.y, unit.x};
    vel_ = (100.0f + utils::num::getRandom(-30.0f, 30.0f)) * unit;

//    float gamma_factor = utils::num::getRandom(-params.full_color_fac, params.full_color_fac);
//    color_.r = std::max(0.0f, std::min(255.0f, params.base_color.r + gamma_factor * 255));
//    color_.g = std::max(0.0f, std::min(255.0f, params.base_color.g + gamma_factor * 255));
//    color_.b = std::max(0.0f, std::min(255.0f, params.base_color.b + gamma_factor * 255));
    color_ = sf::Color(160, 160, 160, utils::num::getRandom(50, 255));
    size_ = utils::num::getRandom(1, 2);
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

WeatherSystem::WeatherSystem(Framework* framework, float dir, const WeatherParams& params) :
        AbstractDrawableObject({}, {1.0f, 1.0f}, nullptr, 0),
        particles_(),
        framework_(framework),
        drawables_(sf::Quads, 0),
        time_elapsed_(0.0f),
        dir_(dir)
{
}

bool WeatherSystem::update(float time_elapsed)
{
    int i = 0;
    auto pos = framework_->getPlayer()->getPosition();
    this->setPosition(pos);

    utils::eraseIf<std::unique_ptr<WeatherParticle>>(particles_,
            [this, time_elapsed, &i](std::unique_ptr<WeatherParticle>& particle) {
        particle->update(time_elapsed);
        updateParticle(particle, i);

        ++i;

        if (utils::geo::getDistance(particle->getPosition(), this->getPosition()) > CONF<int>("graphics/window_width_px"))
            return true;
        return false;
    });


    if (time_elapsed_ > 1.0f)
    {
        i = 0;

        for (i = -100; i < 100; ++i)
        {
            particles_.emplace_back(std::make_unique<WeatherParticle>(
                    pos + sf::Vector2f(i * CONF<int>("graphics/window_width_px") / 200 * 2, CONF<int>("graphics/window_height_px") / 2.0f), dir_, WeatherParams{}));
        }
        drawables_.resize(particles_.size() * 4);
        time_elapsed_ -= 0.5f;
    }

    time_elapsed_ += time_elapsed;
    return true;
}

void WeatherSystem::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    static sf::RenderStates states_;
    states_.shader = &RM.getShader("debris.frag");
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
            drawables_[4 * i + j].color = sf::Color(0, 0, 255, particle->getColor().a);
    }
    drawables_[4 * i + 1].position += {particle->getSize(), 0};
    drawables_[4 * i + 2].position += {particle->getSize(), particle->getSize()};
    drawables_[4 * i + 3].position += {0, particle->getSize()};
}
