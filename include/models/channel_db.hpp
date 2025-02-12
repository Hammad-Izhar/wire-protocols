// #pragma once
// #include <stdint.h>
// #include <unordered_map>
// #include "models/channel.hpp"

// class channel_db
// {
// public:
//     channel_db();

//     // Getters
//     [[nodiscard]] const std::unordered_map<std::array<uint8_t,16>, Channel> &get_data() const;
//     [[nodiscard]] Channel get_by_uid(std::array<uint8_t,16> channel_uid) const;

//     // Setters
//     void add_channel(Channel &channel);
//     void remove_channel(std::array<uint8_t,16> channel_uid);
// private:
//     std::unordered_map<std::array<uint8_t,16>, Channel> data;
// };