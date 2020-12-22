//
// Created by jul3x on 21.12.2020.
//

#ifndef RAG3_ENGINE_INCLUDE_R3E_OBJECTS_DESTRUCTION_H
#define RAG3_ENGINE_INCLUDE_R3E_OBJECTS_DESTRUCTION_H

#include <list>
#include <memory>

#include <R3E/objects/AbstractDrawableObject.h>
#include <R3E/utils/Numeric.h>


using namespace r3e;

struct DestructionParams {
    float vel, acc, time;
    float spread_degree, acceleration_spread;
    float vel_fac, acc_fac, time_fac;

    sf::Color base_color;
    float r_fac, g_fac, b_fac, full_color_fac;
    float min_size, max_size;

    int count;

    std::string shader;
};


class DestructionParticle {
public:
    DestructionParticle(const sf::Vector2f& position, float dir, const DestructionParams& params);

    bool update(float time_elapsed);

    [[nodiscard]] const sf::Vector2f& getPosition() const;
    [[nodiscard]] float getTime() const;
    [[nodiscard]] const sf::Color& getColor() const;
    [[nodiscard]] float getSize() const;

    void addToLife(float time);

private:
    sf::Vector2f pos_, vel_, acc_;
    sf::Color color_;

    double dir_;
    float size_;
    float time_alive_;
};


class DestructionSystem : public AbstractDrawableObject {
public:
    DestructionSystem(const sf::Vector2f& position, float dir, const DestructionParams& params);

    bool update(float time_elapsed);
    [[nodiscard]] float getDestructionDir() const;
    [[nodiscard]] const DestructionParams& getDestructionParams() const;

    void addToLife(float time);

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    void updateParticle(std::unique_ptr<DestructionParticle>& particle, int i, sf::Uint8 a);

    const DestructionParams& params_;
    std::list<std::unique_ptr<DestructionParticle>> particles_;
    sf::VertexArray drawables_;
    float time_elapsed_, dir_;

};


#endif //RAG3_ENGINE_INCLUDE_R3E_OBJECTS_DESTRUCTION_H
