//
// Created by jul3x on 25.03.2021.
//

#ifndef RAG3_MULTI_INCLUDE_PACKETS_SERVEREVENTPACKET_H
#define RAG3_MULTI_INCLUDE_PACKETS_SERVEREVENTPACKET_H

#include <set>

#include <SFML/System.hpp>
#include <SFML/Network.hpp>


class ServerEventPacket : public sf::Packet {
public:
    enum class Type {
        NameChange = 5,
        DestroyedObstacle = 4,
        CollectedObject = 3,
        EnteredObject = 2,
        UseObject = 1,
        Exit = -1,
        None = 0
    };

    static constexpr auto STRING_DATA_ABOVE = 4;

    ServerEventPacket() = default;

    ServerEventPacket(Type type, int uid, sf::Uint32 player_ip)
    {
        type_ = type;
        uid_ = uid;
        player_ip_ = player_ip;
        *this << static_cast<sf::Uint16>(type_) << player_ip_ << uid_;
    }

    ServerEventPacket(Type type, const std::string& data, sf::Uint32 player_ip)
    {
        type_ = type;
        data_ = data;
        player_ip_ = player_ip;
        *this << static_cast<sf::Uint16>(type_) << player_ip_ << data_;
    }

    [[nodiscard]] Type getType() const
    {
        return type_;
    }

    [[nodiscard]] int getUID() const
    {
        return uid_;
    }

    [[nodiscard]] const std::string& getStrData() const
    {
        return data_;
    }

    [[nodiscard]] sf::Uint32 getIP() const
    {
        return player_ip_;
    }

private:
    void onReceive(const void* data, std::size_t size) override
    {
        append(data, size);
        sf::Uint16 type;
        *this >> type >> player_ip_;

        if (type > STRING_DATA_ABOVE)
            *this >> data_;
        else
            *this >> uid_;
        type_ = static_cast<Type>(type);
    }

    Type type_{Type::None};
    int uid_{-1};
    sf::Uint32 player_ip_{0};
    std::string data_;

};

#endif //RAG3_MULTI_INCLUDE_PACKETS_SERVEREVENTPACKET_H
