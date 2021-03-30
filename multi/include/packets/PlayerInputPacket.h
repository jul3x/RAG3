//
// Created by jul3x on 17.03.2021.
//

#ifndef RAG3_MULTI_INCLUDE_PACKETS_PLAYERINPUTPACKET_H
#define RAG3_MULTI_INCLUDE_PACKETS_PLAYERINPUTPACKET_H

#include <set>

#include <SFML/System.hpp>
#include <SFML/Network.hpp>


class PlayerInputPacket : public sf::Packet {
public:
    PlayerInputPacket() = default;

    PlayerInputPacket(const std::set<sf::Keyboard::Key>& keys, bool mouse_pressed, float rotation,
                      short int current_weapon)
    {
        timestamp_ = utils::timeSinceEpochMillisec();
        *this << static_cast<sf::Uint64>(timestamp_);
        for (auto key : IMPLEMENTED_KEYS)
        {
            keys_[key] = keys.count(key);
            *this << keys_[key];
        }
        *this << mouse_pressed << rotation << current_weapon;
    }

    [[nodiscard]] bool isKey(sf::Keyboard::Key key) const
    {
        return keys_.at(key);
    }

    [[nodiscard]] bool isLeftMousePressed() const
    {
        return mouse_pressed_;
    }

    [[nodiscard]] float getRotation() const
    {
        return rotation_;
    }

    [[nodiscard]] short int getCurrentWeapon() const
    {
        return current_weapon_;
    }

    [[nodiscard]] uint64_t getTimestamp() const
    {
        return timestamp_;
    }

private:
    static constexpr auto IMPLEMENTED_KEYS = {sf::Keyboard::W, sf::Keyboard::S, sf::Keyboard::A, sf::Keyboard::D,
                                              sf::Keyboard::LShift};

    void onReceive(const void* data, std::size_t size) override
    {
        append(data, size);
        sf::Uint64 time;
        *this >> time;
        timestamp_ = static_cast<uint64_t>(time);
        for (auto key : IMPLEMENTED_KEYS)
            *this >> keys_[key];

        *this >> mouse_pressed_ >> rotation_ >> current_weapon_;
    }

    std::unordered_map<sf::Keyboard::Key, bool> keys_{};
    bool mouse_pressed_{};
    float rotation_{};
    short int current_weapon_{};
    uint64_t timestamp_{};
};

#endif //RAG3_MULTI_INCLUDE_PACKETS_PLAYERINPUTPACKET_H
