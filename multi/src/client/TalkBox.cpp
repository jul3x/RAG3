//
// Created by jul3x on 30.01.22.
//

#include <common/ResourceManager.h>

#include <client/TalkBox.h>


TalkBox::TalkBox(tgui::Gui* gui, tgui::Theme* theme, const sf::Vector2f& pos)
{
    info_talk_box_ = tgui::Label::create("Type what you want to say:");
    info_talk_box_->setRenderer(theme->getRenderer("Label"));
    info_talk_box_->setVisible(false);
    info_talk_box_->setSize({info_talk_box_->getText().getSize() * CONF<float>("graphics/message_font_size"),
                             CONF<float>("graphics/message_font_size")});
    info_talk_box_->setTextSize(CONF<float>("graphics/message_font_size"));;
    info_talk_box_->setPosition(pos.x - info_talk_box_->getFullSize().x / 2.0f,
                                pos.y);
    info_talk_box_->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
    gui->add(info_talk_box_);

    talk_box_ = tgui::EditBox::create();
    talk_box_->setInputValidator(RM.getSafeRegex());
    talk_box_->setRenderer(theme->getRenderer("TalkBox"));
    talk_box_->setVisible(false);
    talk_box_->setEnabled(false);
    talk_box_->setPosition(pos.x - CONF<sf::Vector2f>("graphics/talk_box_size").x / 2.0f,
                           pos.y + CONF<float>("graphics/talk_box_box_offset"));
    talk_box_->setSize(CONF<sf::Vector2f>("graphics/talk_box_size"));
    talk_box_->setMaximumCharacters(CONF<int>("max_player_message_length"));
    talk_box_->setTextSize(CONF<float>("graphics/message_font_size"));
    gui->add(talk_box_);

    left_ = tgui::Label::create(">");
    left_->setRenderer(theme->getRenderer("Label"));
    left_->setVisible(false);
    left_->setTextSize(CONF<float>("graphics/message_font_size"));;
    left_->setPosition(
            pos.x - CONF<sf::Vector2f>("graphics/talk_box_size").x / 2.0f -
            CONF<float>("graphics/talk_box_box_offset") / 2.0f,
            pos.y + CONF<float>("graphics/talk_box_box_offset"));
    left_->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Left);
    gui->add(left_);
}

void TalkBox::update(bool show)
{
    if (show)
    {
        talk_box_->setFocused(true);
    }
}

void TalkBox::show(bool show)
{
    talk_box_->setEnabled(show);
    talk_box_->setVisible(show);
    talk_box_->setFocused(show);
    talk_box_->setText("");

    left_->setVisible(show);

    info_talk_box_->setVisible(show);
}

bool TalkBox::isEmpty() const
{
    return talk_box_->getText().getSize() <= 0;
}

std::string TalkBox::getText() const
{
    return static_cast<std::string>(talk_box_->getText());
}

