//
// Created by jul3x on 01.10.19.
//


#ifndef RAG3_ENGINE_GRAPHICS_ABSTRACTCAMERA_H
#define RAG3_ENGINE_GRAPHICS_ABSTRACTCAMERA_H

#include <SFML/System/Vector3.hpp>
#include <SFML/System/Vector2.hpp>


namespace r3e {
    namespace graphics {

        class AbstractCamera {
        public:
            AbstractCamera() = default;

            virtual void update(float time_elapsed) = 0;

            sf::Vector3f& getViewCenter()
            {
                return center_;
            }

            sf::Vector2f& getViewSize()
            {
                return view_size_;
            }

            const sf::Vector2f& getPointingTo() const
            {
                return pointing_to_;
            }

            void setCenter(const sf::Vector3f& position)
            {
                center_ = position;
            }

            void setPointingTo(const sf::Vector2f& position)
            {
                pointing_to_ = position;
            }

            void setViewNormalSize(const sf::Vector2f& size)
            {
                view_normal_size_ = size;
            }

            void setZoomTo(float zoom)
            {
                zoom_to_ = zoom;
            }

        protected:
            sf::Vector3f center_;
            sf::Vector2f pointing_to_;
            sf::Vector2f view_size_;
            sf::Vector2f view_normal_size_;

            float zoom_to_;
            float time_elapsed_;
        };

    } // namespace graphics
} // namespace r3e

#endif // RAG3_ENGINE_GRAPHICS_ABSTRACTCAMERA_H