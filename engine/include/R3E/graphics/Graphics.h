//
// Created by jul3x on 26.02.19.
//

#ifndef RAG3_ENGINE_INCLUDE_R3E_GRAPHICS_GRAPHICS_H
#define RAG3_ENGINE_INCLUDE_R3E_GRAPHICS_GRAPHICS_H

#include <set>

#include <SFML/Graphics.hpp>

#include <R3E/objects/AbstractDrawableObject.h>
#include <R3E/system/Config.h>
#include <R3E/graphics/StaticShadow.h>


namespace r3e::graphics {

    class Graphics {

    public:
        Graphics(const sf::Vector2i& size, const std::string& title, int style, const sf::Color& bg_color);
        Graphics(const Graphics&) = delete;
        Graphics& operator=(const Graphics&) = delete;

        [[nodiscard]] bool isWindowOpen() const;
        [[nodiscard]] sf::RenderWindow& getWindow();
        [[nodiscard]] sf::View& getCurrentView();
        [[nodiscard]] sf::View& getStaticView();

        void setStaticView();
        void setCurrentView();
        void setViewCenter(const sf::Vector3f& view);
        void setViewSize(const sf::Vector2f& view);
        void modifyCurrentView(const sf::View& view);
        void modifyStaticView(const sf::View& view);

        void clear();
        void draw(AbstractDrawableObject& object, sf::RenderStates states = sf::RenderStates::Default);
        void draw(sf::Drawable& object, sf::RenderStates states = sf::RenderStates::Default);
        void draw(StaticShadow& object, sf::RenderStates states = sf::RenderStates::Default);
        void drawSorted(AbstractDrawableObject& object);
        void drawAlreadySorted(sf::RenderStates states = sf::RenderStates::Default);
        void display();

    private:
        sf::ContextSettings settings_;
        sf::RenderWindow window_;
        sf::View current_view_;
        sf::View static_view_;

        sf::Color bg_color_;

        std::set<AbstractDrawableObject*, AbstractDrawableObjectsCmp> sorted_drawables_;

    };

} // namespace r3e::graphics

#endif //RAG3_ENGINE_INCLUDE_R3E_GRAPHICS_GRAPHICS_H
