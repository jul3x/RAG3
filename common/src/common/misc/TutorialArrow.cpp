//
// Created by jul3x on 12.04.21.
//

#include <common/ResourceManager.h>

#include <common/misc/TutorialArrow.h>


TutorialArrow::TutorialArrow(AbstractPhysicalObject* father) :
        father_(father),
        arrow_("^", RM.getFont(), CONF<float>("graphics/tutorial_arrow_text_size")),
        time_elapsed_(0.0f)
{
    arrow_.setFillColor(sf::Color::White);
    arrow_.setRotation(180.0f);
    arrow_.setOrigin(arrow_.getGlobalBounds().width / 2.0f, arrow_.getGlobalBounds().height / 2.0f);

    base_pos_ = father_->getPosition() + sf::Vector2f{0.0f, -father_->getSize().y / 2.0f + CONF<float>("graphics/tutorial_arrow_offset")};
    arrow_.setPosition(base_pos_);
}

bool TutorialArrow::update(float time_elapsed)
{
    time_elapsed_ += time_elapsed;
    arrow_.setPosition(base_pos_ +
        sf::Vector2f(0.0f, std::sin(time_elapsed_ * M_PI * 2.0f) * CONF<float>("graphics/tutorial_arrow_amplitude")));

    return true;
}

void TutorialArrow::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(arrow_, states);
}

AbstractPhysicalObject* TutorialArrow::getFather() const
{
    return father_;
}
