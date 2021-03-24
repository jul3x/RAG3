//
// Created by jul3x on 17.03.2021.
//

#ifndef RAG3_MULTI_INCLUDE_PACKETS_PLAYERSSTATEPACKET_H
#define RAG3_MULTI_INCLUDE_PACKETS_PLAYERSSTATEPACKET_H

#include <unordered_map>

#include <SFML/System.hpp>
#include <SFML/Network.hpp>

#include <R3E/utils/Misc.h>

#include <common/characters/Player.h>

#include <packets/PlayerInputPacket.h>


struct PlayerData {
    sf::Vector2f pos_{}, vel_{};
    float rotation_{};
    int current_weapon_{};
    std::vector<float> weapon_state_{};
    float health_{};
    bool is_shooting_{};
    int current_special_id_{};
};


class PlayersStatePacket : public sf::Packet
{
public:
    PlayersStatePacket() = default;

    explicit PlayersStatePacket(const std::unordered_map<std::string, Player>& players, std::unordered_map<std::string, PlayerInputPacket>& cached_packets)
    {
        *this << r3e::utils::timeSinceEpochMillisec() << static_cast<short int>(players.size());
        for (const auto& player : players)
        {
            PlayerData data;
            data.pos_ = player.second.getPosition();
            data.vel_ = player.second.getVelocity();
            data.rotation_ = player.second.getRotation();
            data.current_weapon_ = player.second.getCurrentWeapon();
            data.health_ = player.second.getHealth();
            data.is_shooting_ = cached_packets.count(player.first) ? cached_packets[player.first].isLeftMousePressed() : false;

            auto special = player.second.getCurrentSpecialObject();
            data.current_special_id_ = special == nullptr ? -1 : special->getUniqueId();
            for (const auto& weapon : player.second.getWeapons())
            {
                data.weapon_state_.emplace_back(weapon->getState());
            }

            *this << sf::IpAddress(player.first).toInteger() << data.pos_.x << data.pos_.y << data.vel_.x << data.vel_.y
                  << data.rotation_ << data.current_weapon_ << data.health_ << data.is_shooting_ << data.current_special_id_;
            *this << static_cast<short int>(data.weapon_state_.size());
            for (auto state : data.weapon_state_)
            {
                *this << state;
            }

            std::cout << data.current_special_id_ << std::endl;

            players_data_[player.first] = std::move(data);
        }
    }

    [[nodiscard]] const std::unordered_map<std::string, PlayerData>& getDatas() const
    {
        return players_data_;
    }

    [[nodiscard]] const PlayerData& getPlayerData(const std::string& ip) const
    {
        return players_data_.at(ip);
    }

    [[nodiscard]] uint64_t getTimestamp() const
    {
        return timestamp_;
    }

private:
    void onReceive(const void* data, std::size_t size) override
    {
        append(data, size);

        short int players_count;
        *this >> timestamp_ >> players_count;

        for (int i = 0; i < players_count; ++i)
        {
            PlayerData p_data;
            sf::Uint32 ip_int;
            short int weapon_state_size;
            *this >> ip_int >> p_data.pos_.x >> p_data.pos_.y >> p_data.vel_.x >> p_data.vel_.y
                  >> p_data.rotation_ >> p_data.current_weapon_ >> p_data.health_ >> p_data.is_shooting_ >> p_data.current_special_id_;
            *this >> weapon_state_size;
            for (auto j = 0; j < weapon_state_size; ++j)
            {
                p_data.weapon_state_.emplace_back();
                *this >> p_data.weapon_state_.back();
            }

            players_data_[sf::IpAddress(ip_int).toString()] = std::move(p_data);
        }

    }

    std::unordered_map<std::string, PlayerData> players_data_;
    uint64_t timestamp_{};
};

#endif //RAG3_MULTI_INCLUDE_PACKETS_PLAYERSSTATEPACKET_H
