//
// Created by jul3x on 01.09.20.
//

#ifndef RAG3_ENGINE_INCLUDE_R3E_OBJECTS_LIGHTABLE_H
#define RAG3_ENGINE_INCLUDE_R3E_OBJECTS_LIGHTABLE_H

#include <R3E/graphics/LightPoint.h>


namespace r3e {
    class Lightable {
    public:
        Lightable() = default;

        [[nodiscard]] graphics::LightPoint* getLightPoint() const
        {
            return light_.get();
        }

        void makeLightPoint(const sf::Vector2f& pos, float radius, sf::Texture* texture, const std::string& type, float data = 0.0f)
        {
            if (type == "const")
            {
                light_ = std::make_unique<graphics::LightPoint>(pos, sf::Vector2f{radius, radius}, texture);
            }
            else if (type == "sine")
            {
                light_ = std::make_unique<graphics::SineLightPoint>(pos, sf::Vector2f{radius, radius}, texture, data);
            }
            else if (type == "flicker")
            {
                light_ = std::make_unique<graphics::FlickeringLightPoint>(pos, sf::Vector2f{radius, radius}, texture, data);
            }
        }

        void updateLight(float time_elapsed)
        {
            if (light_ != nullptr)
                light_->update(time_elapsed);
        }

        void lightOff()
        {
            light_.reset();
        }

    protected:
        std::unique_ptr<graphics::LightPoint> light_;

    };
} // namespace r3e

#endif //RAG3_ENGINE_INCLUDE_R3E_OBJECTS_LIGHTABLE_H
