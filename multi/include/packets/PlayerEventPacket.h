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
        Respawn = 6,
        Connection = 5,
        Message = 4,
        NameChange = 3,
        UseBackpackObject = 2,
        UseObject = 1,
        Exit = -1,
        None = 0
    };

    static constexpr auto INT_DATA_ABOVE = 4;

    PlayerEventPacket() = default;

    PlayerEventPacket(Type type, size_t data)
    {
        *this << static_cast<sf::Int16>(type) << static_cast<sf::Uint64>(data);
    }

    PlayerEventPacket(Type type, const r3e::j3x::Parameters& data = {})
    {
        std::string params;

        for (const auto& param : data)
            r3e::j3x::serializeAssign(param.first, param.second, params);
        *this << static_cast<sf::Int16>(type) << params;
    }

    [[nodiscard]] Type getType() const
    {
        return type_;
    }

    [[nodiscard]] const j3x::Parameters& getParams() const
    {
        return *data_;
    }

    [[nodiscard]] sf::Uint64 getIntData() const
    {
        return int_data_;
    }

private:
    void onReceive(const void* data, std::size_t size) override
    {
        append(data, size);
        sf::Int16 type;
        *this >> type;

        if (type > INT_DATA_ABOVE)
        {
            *this >> int_data_;
        }
        else
        {
            std::string params;
            *this >> params;
            data_ = r3e::j3x::parseContent(params);
        }

        type_ = static_cast<Type>(type);
    }

    sf::Uint64 int_data_;
    std::shared_ptr<j3x::Parameters> data_;
    Type type_;

};

#endif //RAG3_MULTI_INCLUDE_PACKETS_PLAYEREVENTPACKET_H
