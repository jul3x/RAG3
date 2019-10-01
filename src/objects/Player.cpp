//
// Created by jprolejko on 30.09.19.
//

#include <system/Engine.h>

#include <objects/Player.h>


Player::Player(const sf::Vector2f &position,
               const sf::Vector2f &velocity) :
        DynamicObject(position,
                      velocity,
                      {SIZE_X_, SIZE_Y_},
                      "player",
                      CFG.getFloat("player_max_acceleration")) {
    last_bullet_time_ = std::chrono::system_clock::now();
}

void Player::update(float time_elapsed) {
    trail_.push_back(this->getPosition());

    if (trail_.size() > TRAIL_COUNT_)
    {
        trail_.pop_front();
    }

    DynamicObject::update(time_elapsed);
}

void Player::shot() {
    auto time_now = std::chrono::system_clock::now();
    if (std::chrono::duration_cast<std::chrono::milliseconds>(
            time_now - last_bullet_time_).count() >= CFG.getFloat("bullet_timeout"))
    {
        float sine = std::sin(this->getRotation() * M_PI / 180.0f);
        float cosine = std::cos(this->getRotation() * M_PI / 180.0f);
        auto offset_position = this->getPosition();
        offset_position.x += GUN_OFFSET_X_ * cosine;
        offset_position.y += GUN_OFFSET_X_ * sine;
        Engine::getInstance().spawnBullet(offset_position, this->getRotation() * M_PI / 180.0f);
        last_bullet_time_ = time_now;

        this->setForcedVelocity(- CFG.getFloat("recoil") * sf::Vector2f{cosine, sine});
        Engine::getInstance().forceCameraShaking();
    }
}

void Player::draw(sf::RenderTarget &target, sf::RenderStates states) const {
    if (this->isVisible())
    {
        auto pixel_size = this->getSize().x / 5.0f;

        {
            std::vector<sf::Vertex> trail_vert;

            float factor = pixel_size / static_cast<float>(trail_.size() * trail_.size());

            if (trail_.size() >= 2)
            {
                trail_vert.emplace_back(trail_.front(), sf::Color(CFG.getInt("trail_color")),
                        sf::Vector2f{});
            }

            for (size_t i = 1; i < trail_.size(); ++i)
            {
                float temp_r = factor * i * i;
                // make 2 points
                sf::Vector2f diff = trail_.at(i) - trail_.at(i - 1);
                float dir = std::atan2(diff.y, diff.x) + M_PI_2;

                sf::Vector2f norm = {std::cos(dir), std::sin(dir)};
                trail_vert.emplace_back(trail_.at(i) - temp_r * norm,
                        sf::Color(CFG.getInt("trail_color")), sf::Vector2f{});
                trail_vert.emplace_back(trail_.at(i) + temp_r * norm,
                        sf::Color(CFG.getInt("trail_color")), sf::Vector2f{});
            }

            target.draw(&trail_vert[0], trail_vert.size(), sf::TriangleStrip, states);
        }
        target.draw(shape_, states);
    }
}