//
// Created by jul3x on 26.02.19.
//

#include <R3E/graphics/Graphics.h>


namespace r3e::graphics {
    Graphics::Graphics(const sf::Vector2i& size,
                       const std::string& title,
                       int style,
                       const sf::Color& bg_color) : settings_(0, 0, 8),
                                                    window_(sf::VideoMode(size.x,
                                                                          size.y),
                                                            title,
                                                            style,
                                                            settings_),
                                                    bg_color_(bg_color)
    {
        current_view_ = window_.getView();
        current_view_.setSize(size.x, size.y);
        current_view_.setCenter(size.x / 2.0f, size.y / 2.0f);
        static_view_ = current_view_;
        window_.setView(current_view_);
    }

    bool Graphics::isWindowOpen() const
    {
        return window_.isOpen();
    }

    sf::RenderWindow& Graphics::getWindow()
    {
        return window_;
    }

    sf::View& Graphics::getCurrentView()
    {
        return current_view_;
    }

    sf::View& Graphics::getStaticView()
    {
        return static_view_;
    }

    void Graphics::modifyCurrentView(const sf::View& view)
    {
        current_view_ = view;
    }

    void Graphics::modifyStaticView(const sf::View& view)
    {
        static_view_ = view;
    }

    void Graphics::setStaticView()
    {
        window_.setView(static_view_);
    }

    void Graphics::setCurrentView()
    {
        window_.setView(current_view_);
    }

    void Graphics::setViewCenter(const sf::Vector3f& view_center)
    {
        current_view_.setCenter(view_center.x, view_center.y);
        current_view_.setRotation(view_center.z);
        window_.setView(current_view_);
    }

    void Graphics::setViewSize(const sf::Vector2f& view_size)
    {
        current_view_.setSize(view_size.x, view_size.y);
        window_.setView(current_view_);
    }

    void Graphics::clear()
    {
        window_.clear(bg_color_);
        sorted_drawables_.clear();
    }

    void Graphics::draw(AbstractDrawableObject& object, sf::RenderStates states)
    {
        object.setVisibility(window_.getView());
        if (object.isVisible())
            window_.draw(object, states);
    }

    void Graphics::draw(sf::Drawable& object, sf::RenderStates states)
    {
        window_.draw(object, states);
    }

    void Graphics::drawSorted(AbstractDrawableObject& object)
    {
        object.setVisibility(window_.getView());
        if (object.isVisible())
            sorted_drawables_.insert(&object);
    }

    void Graphics::drawAlreadySorted(sf::RenderStates states)
    {
        for (auto& object : sorted_drawables_)
            window_.draw(*object, states);
    }

    void Graphics::display()
    {
        window_.display();
    }

    void Graphics::draw(StaticShadow& object, sf::RenderStates states)
    {
        object.setVisibility(window_.getView());
        if (object.isVisible())
            window_.draw(object, states);
    }

    void Graphics::setBgColor(const sf::Color& bg_color)
    {
        bg_color_ = bg_color;
    }

} // namespace r3e::graphics