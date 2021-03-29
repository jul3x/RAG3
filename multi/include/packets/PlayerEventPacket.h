//
// Created by jul3x on 25.03.2021.
//

#ifndef RAG3_MULTI_INCLUDE_PACKETS_PLAYEREVENTPACKET_H
#define RAG3_MULTI_INCLUDE_PACKETS_PLAYEREVENTPACKET_H

#include <set>

#include <SFML/System.hpp>
#include <SFML/Network.hpp>


class PlayerEventPacket : public sf::Packet {
public:
    enum class Type {
        NameChange = 3,
        UseBackpackObject = 2,
        UseObject = 1,
        Exit = -1,
        None = 0
    };

    PlayerEventPacket() = default;

    PlayerEventPacket(Type type, const std::string& data = "")
    {
        type_ = type;
        data_ = data;
        *this << static_cast<sf::Uint16>(type_) << data_;
    }

    [[nodiscard]] Type getType() const
    {
        return type_;
    }

    [[nodiscard]] const std::string& getStrData() const
    {
        return data_;
    }

private:
    void onReceive(const void* data, std::size_t size) override
    {
        append(data, size);
        sf::Uint16 type;
        *this >> type >> data_;

        type_ = static_cast<Type>(type);
    }

    std::string data_;
    Type type_;

};

#endif //RAG3_MULTI_INCLUDE_PACKETS_PLAYEREVENTPACKET_H
