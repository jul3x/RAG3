//
// Created by jul3x on 04.01.22.
//

#ifndef RAG3_GAME_INCLUDE_UI_MESSAGE_H
#define RAG3_GAME_INCLUDE_UI_MESSAGE_H

#include <iomanip>
#include <ctime>

#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Drawable.hpp>

#include <common/ResourceManager.h>


using namespace r3e;

class Message : public sf::Drawable {
public:
    explicit Message(const sf::Vector2f& pos, const std::string& text) :
            msg_(),
            time_elapsed_(CONF<float>("graphics/message_timeout"))
    {
        auto time = std::time(nullptr);
        auto tm = *std::localtime(&time);
        std::ostringstream oss;
        oss << std::put_time(&tm, "%H:%M:%S") << ": " << text;

        msg_.setString(oss.str());
        msg_.setFont(RM.getFont());
        msg_.setCharacterSize(CONF<float>("graphics/message_font_size"));
        msg_.setFillColor(sf::Color(CONF<int>("graphics/message_font_color")));
        msg_.setPosition(pos);
    }

    bool update(float time_elapsed)
    {
        time_elapsed_ -= time_elapsed;
        return time_elapsed_ > 0.0f;
    }

    void setPosition(const sf::Vector2f& pos)
    {
        msg_.setPosition(pos);
    }

    const sf::Vector2f& getPosition() const
    {
        return msg_.getPosition();
    }

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        target.draw(msg_, states);
    }

    sf::Text msg_;
    float time_elapsed_;
};

#endif //RAG3_GAME_INCLUDE_UI_MESSAGE_H
