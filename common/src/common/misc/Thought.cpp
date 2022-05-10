//
// Created by jul3x on 01.04.20.
//

#include <common/ResourceManager.h>

#include <common/misc/Thought.h>


Thought::Thought(AbstractPhysicalObject* father, std::string text, float duration) :
        father_(father),
        text_(text, RM.getFont(), CONF<float>("graphics/thought_text_size")),
        time_elapsed_(duration),
        AbstractPhysicalObject(father->getPosition() + CONF<sf::Vector2f>("graphics/thought_offset"),
                               CONF<sf::Vector2f>("graphics/thought_size_bottom"), collision::None(),
                               &RM.getTexture("thought_bottom")),
        top_(father->getPosition() + CONF<sf::Vector2f>("graphics/thought_offset"),
             CONF<sf::Vector2f>("graphics/thought_size_top"), &RM.getTexture("thought_top"))
{
    this->changeOrigin({0.0f, CONF<sf::Vector2f>("graphics/thought_size_bottom").y});
    top_.changeOrigin({0.0f, CONF<sf::Vector2f>("graphics/thought_size_top").y});
    text_.setFillColor(sf::Color::White);

    static const auto max_length_per_line = CONF<int>("graphics/thought_max_line_length");
    auto wrapped_text = utils::wrapText(std::move(text), max_length_per_line);
    text_.setFont(RM.getFont());
    text_.setString(wrapped_text);
    text_.setCharacterSize(CONF<float>("graphics/thought_text_size"));

    auto count = std::count(wrapped_text.begin(), wrapped_text.end(), '\n');
    center_.reserve(count);
    auto incremental_offset = CONF<sf::Vector2f>("graphics/thought_offset") -
                              sf::Vector2f(0.0f, CONF<sf::Vector2f>("graphics/thought_size_bottom").y);
    for (size_t i = 0; i < count; ++i)
    {
        center_.emplace_back(father->getPosition() + incremental_offset,
                             CONF<sf::Vector2f>("graphics/thought_size_center"), &RM.getTexture("thought"));
        center_.at(i).changeOrigin({0.0f, CONF<sf::Vector2f>("graphics/thought_size_center").y});
        incremental_offset = CONF<sf::Vector2f>("graphics/thought_offset") - sf::Vector2f(0.0f, CONF<sf::Vector2f>(
                "graphics/thought_size_bottom").y + (i + 1) * CONF<sf::Vector2f>("graphics/thought_size_center").y);
    }

    top_.setPosition(father->getPosition() + incremental_offset);
    text_.setPosition(
            father_->getPosition() + incremental_offset + sf::Vector2f{CONF<float>("graphics/thought_text_margin"),
                                                                       CONF<sf::Vector2f>("graphics/thought_size_top")
                                                                               .y / 2.0f -
                                                                       CONF<float>("graphics/thought_text_size")});

}

bool Thought::update(float time_elapsed)
{
    time_elapsed_ -= time_elapsed;
    this->setPosition(father_->getPosition() + CONF<sf::Vector2f>("graphics/thought_offset"));

    auto incremental_offset = CONF<sf::Vector2f>("graphics/thought_offset") -
                              sf::Vector2f(0.0f, CONF<sf::Vector2f>("graphics/thought_size_bottom").y);
    for (size_t i = 0; i < center_.size(); ++i)
    {
        center_.at(i).setPosition(father_->getPosition() + incremental_offset);
        incremental_offset = CONF<sf::Vector2f>("graphics/thought_offset") - sf::Vector2f(0.0f, CONF<sf::Vector2f>(
                "graphics/thought_size_bottom").y + (i + 1) * CONF<sf::Vector2f>("graphics/thought_size_center").y);
    }

    top_.setPosition(father_->getPosition() + incremental_offset);
    text_.setPosition(father_->getPosition() + incremental_offset +
                      sf::Vector2f{CONF<float>("graphics/thought_text_margin"),
                                   CONF<sf::Vector2f>("graphics/thought_size_top").y / 2.0f -
                                   CONF<float>("graphics/thought_text_size")});

    return time_elapsed_ > 0.0f;
}

void Thought::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(shape_, states);

    for (auto& center : center_)
        target.draw(center, states);

    target.draw(top_, states);
    target.draw(text_, states);
}

AbstractPhysicalObject* Thought::getFather() const
{
    return father_;
}
