//
// Created by jul3x on 26.02.19.
//

#ifndef RAG3_ENGINE_GRAPHICS_GRAPHICS_H
#define RAG3_ENGINE_GRAPHICS_GRAPHICS_H

#include <SFML/Graphics.hpp>

#include <engine/objects/AbstractDrawableObject.h>
#include <engine/system/Config.h>


class Graphics {

public:
    Graphics(const sf::Vector2i& size, const std::string& title, int style, const sf::Color& bg_color);

    Graphics(const Graphics&) = delete;

    Graphics& operator=(const Graphics&) = delete;

    bool isWindowOpen() const;

    sf::RenderWindow& getWindow();

    sf::View& getCurrentView();

    sf::View& getStaticView();

    void setStaticView();

    void setCurrentView();

    void modifyCurrentView(const sf::View& view);

    void modifyStaticView(const sf::View& view);

    void setViewCenter(const sf::Vector3f& view);

    void clear();

    void draw(AbstractDrawableObject& object);

    void display();

private:
    sf::ContextSettings settings_;
    sf::RenderWindow window_;
    sf::View current_view_;
    sf::View static_view_;

    sf::Color bg_color_;

};


#endif //RAG3_ENGINE_GRAPHICS_GRAPHICS_H
