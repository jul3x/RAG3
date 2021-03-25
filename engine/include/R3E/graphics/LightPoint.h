//
// Created by jul3x on 19.05.2020.
//

#ifndef RAG3_ENGINE_INCLUDE_R3E_GRAPHICS_LIGHTPOINT_H
#define RAG3_ENGINE_INCLUDE_R3E_GRAPHICS_LIGHTPOINT_H

#include <cmath>

#include <R3E/graphics/Graphics.h>
#include <R3E/objects/AbstractDrawableObject.h>
#include <R3E/utils/Utils.h>


namespace r3e::graphics {

    class LightPoint : public AbstractDrawableObject {
    public:
        LightPoint(const sf::Vector2f& pos, const sf::Vector2f& size, sf::Texture* texture) :
                AbstractDrawableObject(pos, size, texture),
                graphics_(nullptr)
        {

        }

        void registerGraphics(graphics::Graphics& graphics)
        {
            graphics_ = &graphics;
        }

        void setPosition(float x, float y) override
        {
            auto mapped_pos = graphics_->getWindow().mapCoordsToPixel({x, y}, graphics_->getCurrentView());
            auto mapped_pos_2 = graphics_->getWindow().mapPixelToCoords(mapped_pos, graphics_->getStaticView());
            AbstractDrawableObject::setPosition(mapped_pos_2);
        }

        void setPosition(const sf::Vector2f& pos) override
        {
            auto mapped_pos = graphics_->getWindow().mapCoordsToPixel(pos, graphics_->getCurrentView());
            auto mapped_pos_2 = graphics_->getWindow().mapPixelToCoords(mapped_pos, graphics_->getStaticView());
            AbstractDrawableObject::setPosition(mapped_pos_2.x,
                                                graphics_->getStaticView().getSize().y - mapped_pos_2.y);
        }

        virtual void update(float time_elapsed)
        {

        }

    private:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override
        {
            target.draw(shape_, sf::BlendAdd);
        }

        graphics::Graphics* graphics_;

    };

    class SineLightPoint : public LightPoint {
    public:
        SineLightPoint(const sf::Vector2f& pos, const sf::Vector2f& size, sf::Texture* texture, float period) :
                LightPoint(pos, size, texture), period_(period), time_elapsed_(0.0f)
        {
            this->setRandomPhase();
        }

        void setRandomPhase()
        {
            time_elapsed_ = utils::num::getRandom(0.0f, period_);
        }

        void update(float time_elapsed) override
        {
            time_elapsed_ += time_elapsed;
            this->setColor(255, 255, 255,
                           static_cast<sf::Uint8>(255 * std::abs(std::sin(time_elapsed_ / period_ * 2 * M_PI))));
        }

    private:
        float time_elapsed_, period_;

    };

    class FlickeringLightPoint : public LightPoint {
    public:
        FlickeringLightPoint(const sf::Vector2f& pos, const sf::Vector2f& size, sf::Texture* texture, float period) :
                LightPoint(pos, size, texture), period_(period), time_elapsed_(0.0f), randomized_period_(0.0f)
        {
            this->setRandomPhase();
        }

        void setRandomPhase()
        {
            time_elapsed_ = utils::num::getRandom(0.0f, period_);
        }

        void update(float time_elapsed) override
        {
            time_elapsed_ += time_elapsed;

            if (time_elapsed_ > period_ / 2.0f && time_elapsed_ < period_ / 2.0f + randomized_period_)
            {
                this->setColor(255, 255, 255, 0);
            }
            else
            {
                randomized_period_ = utils::num::getRandom(0.2f, period_ / 2.0f);
                this->setColor(255, 255, 255, 255);
                if (time_elapsed_ > period_)
                    time_elapsed_ -= period_;
            }
        }

    private:
        float time_elapsed_, period_, randomized_period_;

    };
} // namespace r3e::graphics

#endif //RAG3_ENGINE_INCLUDE_R3E_GRAPHICS_LIGHTPOINT_H
