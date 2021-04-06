//
// Created by jul3x on 6.04.2021.
//

#ifndef COMMON_MISC_DESTRUCTION_H
#define COMMON_MISC_DESTRUCTION_H

#include <list>
#include <memory>

#include <R3E/graphics/Graphics.h>
#include <R3E/objects/AbstractDrawableObject.h>
#include <R3E/utils/Numeric.h>


using namespace r3e;

class Framework;

struct WeatherParams {
    float vel, acc, time;
    float spread_degree, acceleration_spread;
    float vel_fac, acc_fac, time_fac;

    sf::Color base_color;
    float r_fac, g_fac, b_fac, full_color_fac;
    float min_size, max_size;

    int count;

    std::string shader;
};


class WeatherParticle {
public:
    WeatherParticle(const sf::Vector2f& position, float dir, const WeatherParams& params);

    bool update(float time_elapsed);

    [[nodiscard]] const sf::Vector2f& getPosition() const;
    [[nodiscard]] const sf::Color& getColor() const;
    [[nodiscard]] float getSize() const;

private:
    sf::Vector2f pos_, vel_, perp_vel_;
    sf::Color color_;

    double dir_;
    float size_;
    float time_elapsed_;
};


class WeatherSystem : public AbstractDrawableObject {
public:
    WeatherSystem(Framework* framework, float dir, const WeatherParams& params);

    bool update(float time_elapsed);

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    void updateParticle(std::unique_ptr<WeatherParticle>& particle, int i);

    std::list<std::unique_ptr<WeatherParticle>> particles_;
    sf::VertexArray drawables_;
    float dir_;
    float time_elapsed_;

    Framework* framework_;

};


#endif //COMMON_MISC_DESTRUCTION_H
