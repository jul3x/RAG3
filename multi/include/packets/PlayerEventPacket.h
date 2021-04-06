//
// Created by jul3x on 25.03.2021.
//

#ifndef RAG3_MULTI_INCLUDE_PACKETS_PLAYEREVENTPACKET_H
#define RAG3_MULTI_INCLUDE_PACKETS_PLAYEREVENTPACKET_H

#include <set>

#include <SFML/System.hpp>
#include <SFML/Network.hpp>

#include <R3E/j3x/J3X.h>


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

    PlayerEventPacket(Type type, const r3e::j3x::Parameters& data = {})
    {
        std::string params;

        for (const auto& param : data)
            r3e::j3x::serializeAssign(param.first, param.second, params);
        *this << static_cast<sf::Uint16>(type) << params;
    }

    [[nodiscard]] Type getType() const
    {
        return type_;
    }

    [[nodiscard]] const j3x::Parameters& getParams() const
    {
        return *data_;
    }

private:
    void onReceive(const void* data, std::size_t size) override
    {
        append(data, size);
        sf::Uint16 type;
        std::string params;
        *this >> type >> params;

        data_ = r3e::j3x::parseContent(params);

        type_ = static_cast<Type>(type);
    }

    std::shared_ptr<j3x::Parameters> data_;
    Type type_;

};

#endif //RAG3_MULTI_INCLUDE_PACKETS_PLAYEREVENTPACKET_H
