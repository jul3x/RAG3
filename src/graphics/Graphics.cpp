//
// Created by jprolejko on 26.02.19.
//

#include <graphics/Graphics.h>


Graphics::Graphics() : settings_(0, 0, 8),
                       window_(sf::VideoMode(CFG.getInt("window_width_px"), CFG.getInt("window_height_px")),
                                             "Codename: Rag3", sf::Style::Default, settings_) {
    current_view_ = window_.getView();
    current_view_.setSize(CFG.getInt("window_width_px"), CFG.getInt("window_height_px"));
    current_view_.setCenter(CFG.getInt("window_width_px") / 2.0f, CFG.getInt("window_height_px") / 2.0f);
    window_.setView(current_view_);
}

const bool Graphics::isWindowOpen() const {
    return window_.isOpen();
}

sf::RenderWindow& Graphics::getWindow() {
    return window_;
}

sf::View& Graphics::getView() {
    return current_view_;
}

void Graphics::setView(const sf::View &view) {
    current_view_ = view;
    window_.setView(current_view_);
}

void Graphics::setViewCenter(const sf::Vector3f &view_center) {
    current_view_.setCenter(view_center.x, view_center.y);
    current_view_.setRotation(view_center.z);
    window_.setView(current_view_);
}

void Graphics::clear() {
    window_.clear(sf::Color(CFG.getInt("background_color")));
}

void Graphics::draw(const AbstractDrawableObject &object) {
    window_.draw(object);
}

void Graphics::display() {
    window_.display();
}